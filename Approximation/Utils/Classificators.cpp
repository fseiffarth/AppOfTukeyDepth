//
// Created by florian on 12.04.22.
//

#include "Classificators.h"
#include "StaticFunctions.h"
#include <random>

void Classificators::RunClassificator(const ClassificationParameters& params, std::vector<std::vector<float>>& data, std::vector<int>& labels, int inputDimension) {
    omp_set_num_threads(params.threads);
    torch::manual_seed(1);
    torch::DeviceType device_type;
    if (torch::cuda::is_available()) {
        std::cout << "CUDA available! Training on GPU." << std::endl;
        device_type = torch::kCUDA;
    } else {
        std::cout << "Training on CPU." << std::endl;
        device_type = torch::kCPU;
    }


    int seed = 0;
    std::vector<std::pair<std::vector<int>, std::vector<int>>> indices;

    StaticFunctions::GetkFoldIndices(params.kFold, (int) data.size(), indices, seed);

    for (auto & x : indices) {
        std::cout << "Indices sizes: " << x.first.size() << ", " << x.second.size() << std::endl;
    }


    std::vector<double> accuracies;
#pragma omp parallel for firstprivate(data, labels, indices) shared(params,device_type,inputDimension,accuracies) default(none)
    for (int k = 0; k < params.kFold; ++k) {
        double accuracy;
        torch::Device device(device_type);
        Net model = Net(inputDimension);
        model.to(device);
        auto train_dataset = DataSet(data,labels,&indices,k,true).map(torch::data::transforms::Stack<>());
        const size_t train_dataset_size = train_dataset.size().value();
        auto train_loader = torch::data::make_data_loader<torch::data::samplers::RandomSampler>(std::move(train_dataset), params.kTrainBatchSize);
        auto test_dataset = DataSet(data, labels, &indices, k, false).map(torch::data::transforms::Stack<>());
        const size_t test_dataset_size = test_dataset.size().value();
        auto test_loader =torch::data::make_data_loader(std::move(test_dataset), indices[k].first.size());


        torch::optim::SGD optimizer(model.parameters(), torch::optim::SGDOptions(0.01).momentum(0.9));
        for (size_t epoch = 1; epoch <= params.kNumberOfEpochs; ++epoch) {
            train(epoch, model, device, *train_loader, optimizer, train_dataset_size, (int) indices[k].second.size() - 1);
            test(model, device, *test_loader, test_dataset_size, accuracy);
            if (epoch == params.kNumberOfEpochs){
#pragma omp critical
                accuracies.emplace_back(accuracy);
            }
        }
        std::stringstream ss;
        ss << "Validation Step: " << k << std::endl;
#pragma omp critical
        StaticFunctions::PrintStream(ss);
    }
    std::printf("Accuracy: %.2f (+-%.2f)",StaticFunctions::mean(accuracies), StaticFunctions::standard_deviation(accuracies));
}

void Classificators::GraphFeatureBasedClassification(const ClassificationParameters& params, std::vector<GraphData>& graphs, std::vector<int>& graphLabels, std::vector<std::vector<int>>& graphEdgeLabels, std::vector<std::vector<int>>& nodeFeatures) {
    int numNodeLabels = 0;
    int numEdgeLabels = 0;
    std::set<int> nodeLabels;
    std::map<int, int> nodeLabelMap;

    std::set<int> edgeLabels;
    std::map<int, int> edgeLabelMap;

    for (auto const & graph : graphs) {
        for(auto const label : graph.GetLabels()){
            nodeLabels.insert(label);
        }
        numNodeLabels = (int) nodeLabels.size();
    }
    int c = 0;
    for (auto const & label : nodeLabels) {
        if (nodeLabelMap.find(label) == nodeLabelMap.end()){
            nodeLabelMap.insert({label, c});
            ++c;
        }
    }

    if (!graphEdgeLabels.empty()) {
        for (auto const &x: graphEdgeLabels) {
            for (auto const label: x) {
                edgeLabels.insert(label);
            }
            numEdgeLabels = (int) edgeLabels.size();
        }
        c = 0;
        for (auto const &label: edgeLabels) {
            if (edgeLabelMap.find(label) == edgeLabelMap.end()){
                edgeLabelMap.insert({label, c});
                ++c;
            }
        }
    }

    int maxNodeFeatureVal = 0;
    for (auto const & x : nodeFeatures) {
        maxNodeFeatureVal = std::max(maxNodeFeatureVal,*std::max_element(x.begin(), x.end()));
    }


    std::vector<std::vector<float>> features = std::vector<std::vector<float>>(nodeFeatures.size(), std::vector<float>(maxNodeFeatureVal * numNodeLabels + numEdgeLabels ));
    for (int i = 0; i < nodeFeatures.size(); ++i) {
        for (int j = 0; j < nodeFeatures[i].size(); ++j) {
            //std::cout << "Graph: " << i << " Node: " << j << std::endl;
            //std::cout << "Node depth: " << nodeFeatures[i][j] << std::endl;
            int label = nodeLabelMap[graphs[i].labels()[j]];
            int depth = nodeFeatures[i][j] - 1;
            ++features[i][numNodeLabels*depth + label];
        }
        if (!graphEdgeLabels.empty()) {
            for (int j = 0; j < graphEdgeLabels[i].size(); ++j) {
                int graphEdgeLabel = edgeLabelMap[graphEdgeLabels[i][j]];
                ++features[i][numNodeLabels * maxNodeFeatureVal + graphEdgeLabel];
            }
        }
    }
    Classificators::RunClassificator(params, features, graphLabels, maxNodeFeatureVal * numNodeLabels + numEdgeLabels);


}

