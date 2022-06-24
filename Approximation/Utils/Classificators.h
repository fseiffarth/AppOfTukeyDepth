//
// Created by florian on 12.04.22.
//
#include <torch/torch.h>
#include "../Data/GraphData.h"

#ifndef CLOSURES_CLASSIFICATORS_H
#define CLOSURES_CLASSIFICATORS_H


struct ClassificationParameters{
    const std::string path = "../../GraphData/DS_all/";
    const std::string db = "NCI1";
    const int64_t kTrainBatchSize = 64;
    const int64_t kNumberOfEpochs = 1000;
    const int kFold = 10;
    int threads = 1;
    const int64_t kLogInterval = 1;
};

class Classificators {

    class DataSet : public torch::data::Dataset<DataSet> {
    public:
// Constructor
        DataSet(std::vector<std::vector<float>>& data, std::vector<int>& labels, std::vector<std::pair<std::vector<int>, std::vector<int>>>* indices = nullptr, int k = -1, bool training = true) {
            process_data(data, _data, indices, k, training);
            process_labels(labels, _labels, indices, k, training);
        };

        static void process_data(std::vector<std::vector<float>>& data, std::vector<torch::Tensor>& torchData, std::vector<std::pair<std::vector<int>, std::vector<int>>>* indices = nullptr, int k = -1, bool training = true) {
            int counter = 0;
            auto options = torch::TensorOptions().dtype(at::kFloat);
            for (auto & vector : data) {
                if (k != -1){
                    std::vector<int>* ind;
                    if (training){
                        ind = &((*indices)[k].second);
                    }
                    else{
                        ind = &((*indices)[k].first);
                    }
                    if(std::find((*ind).begin(), (*ind).end(), counter) != (*ind).end()){
                        torch::Tensor t = torch::from_blob(vector.data(), {1, (int) vector.size()}, options);
                        torchData.emplace_back(t.clone());
                    }
                    ++counter;
                }
                else {
                    torch::Tensor t = torch::from_blob(vector.data(), {1, (int) vector.size()}, options);
                    torchData.emplace_back(t.clone());
                }
            }
        }

        static void process_labels(std::vector<int>& labels, std::vector<torch::Tensor>& torchLabels, std::vector<std::pair<std::vector<int>, std::vector<int>>>* indices = nullptr, int k = -1, bool training = true) {
            int counter = 0;
            auto options = torch::TensorOptions().dtype(at::kFloat);
            for (auto &label: labels) {
                if (k != -1) {
                    std::vector<int> *ind = nullptr;
                    if (training) {
                        ind = &((*indices)[k].second);
                    } else {
                        ind = &((*indices)[k].first);
                    }
                    if (std::find((*ind).begin(), (*ind).end(), counter) != (*ind).end()) {
                        torch::Tensor t = torch::zeros({1}, options);
                        if (label == -1){
                            label = 0;
                        }
                        t.fill_((float) label);
                        torchLabels.emplace_back(t);
                    }
                    ++counter;
                } else {
                    torch::Tensor t = torch::zeros({1}, options);
                    if (label == -1){
                        label = 0;
                    }
                    t.fill_((float) label);
                    torchLabels.emplace_back(t);
                }
            }
        }

        // Override get() function to return tensor at location index
        torch::data::Example<> get(size_t index) override {
            torch::Tensor sample_data = _data.at(index);
            torch::Tensor sample_label = _labels.at(index);
            return {sample_data.clone(), sample_label.clone()};
        };

        // Return the length of data
        [[nodiscard]] torch::optional<size_t> size() const override {return _labels.size();};
    private:
        // Declare 2 vectors of tensors for images and labels
        std::vector<torch::Tensor> _data, _labels;
    };

    struct Net : torch::nn::Module {
        Net(int inputDim) : _inputDim(inputDim), fc1(inputDim, 500), fc2(500, 500), fc3(500,1) {
            register_module("fc1", fc1);
            register_module("fc2", fc2);
            register_module("fc3", fc3);
        }

        torch::Tensor forward(torch::Tensor x, bool train) {
            x = x.view({-1, _inputDim});
            x = torch::relu(fc1->forward(x));
            x = torch::dropout(x, 0.5, train);
            x = torch::relu(fc2->forward(x));
            x = fc3->forward(x);
            return torch::sigmoid(x);
        }
        int _inputDim;
        torch::nn::Linear fc1;
        torch::nn::Linear fc2;
        torch::nn::Linear fc3;
        torch::nn::Dropout dropout;
    };

    template <typename DataSet>
    static void train(
            size_t epoch,
            Net& model,
            torch::Device device,
            DataSet& data_loader,
            torch::optim::Optimizer& optimizer,
            size_t dataset_size, const int64_t kLogInterval) {
        model.train();
        size_t batch_idx = 0;
        long int size = 0;
        for (auto& batch : data_loader) {
            auto data = batch.data.to(device), targets = batch.target.to(device);
            optimizer.zero_grad();
            auto output = model.forward(data, true);
            auto loss = torch::mse_loss(output, targets);
            AT_ASSERT(!std::isnan(loss.template item<float>()));
            loss.backward();
            optimizer.step();
            size += batch.data.size(0);

            if (batch_idx++ % kLogInterval == 0) {
                std::printf(
                        "\rTrain Epoch: %ld [%5ld/%5ld] Loss: %.4f\n",
                        epoch,
                        size,
                        dataset_size,
                        loss.template item<float>());
            }
        }
    }

    template <typename DataSet>
    static void test(Net& model,torch::Device device,DataSet& data_loader,size_t dataset_size, double& accuracy) {
        torch::NoGradGuard no_grad;
        model.eval();
        double test_loss = 0;
        int32_t correct = 0;
        for (const auto& batch : data_loader) {
            auto data = batch.data.to(device), targets = batch.target.to(device);
            auto output = model.forward(data, false);
            test_loss += torch::mse_loss(output,targets).template item<float>();
            auto pred = output.clone();
            torch::Tensor x = torch::ones(pred.sizes());
            pred = pred.where(pred <= 0.5, x);
            x = torch::zeros(pred.sizes());
            pred = pred.where(pred > 0.5, x);
            correct += pred.eq(targets).sum().template item<int64_t>();
        }

        test_loss /= (double) dataset_size;
        accuracy = static_cast<double>(correct) / (double) dataset_size;
        std::printf("Test set: [%d] Average loss: %.4f | Accuracy: %.3f\n\n", (int) dataset_size, test_loss,accuracy);
    }

public:
    static void RunClassificator(const ClassificationParameters& params, std::vector<std::vector<float>> &data, std::vector<int>& labels, int inputDimension);
    static void GraphFeatureBasedClassification(const ClassificationParameters& params,std::vector<GraphData>& graphs, std::vector<int>& graphLabels, std::vector<std::vector<int>>& graphEdgeLabels, std::vector<std::vector<int>>& nodeFeatures);
};


#endif //CLOSURES_CLASSIFICATORS_H
