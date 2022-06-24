//
// Created by Florian on 12.04.2021.
//

#ifndef CLOSURES_DATA_H
#define CLOSURES_DATA_H

#include "../Utils/typedefs.h"

template <typename T>
class Data {
    /// Base data class
public:
    Data()= default;
    ~Data();

    /// Constructor for the data object
    /// \param dataObject the object
    /// \param name name of the object
    explicit Data(const T& dataObject, std::string name = "");

    /// Constructor for the data if data comes with labels
    /// \param dataObject the object
    /// \param name name of the object
    /// \param labels elementwise labels of the data
    Data(const T& dataObject, std::string name, Labels& labels);

    /// Copy constructor
    /// \param other
    Data(const Data<T>& other);

    /// Return a const reference to the data
    /// \return
    const T& GetData() const;
    /// Return the data
    /// \return
    T & data();
    /// Change the data
    /// \param data
    void set_data(T& data);
    /// Get the size of the data
    /// \return
    [[nodiscard]] virtual size_t size() const = 0;
    /// Get the number of classes, if data is labeled
    /// \return
    size_t class_num(){return classNumber;};
    virtual Element elem(Element Id) = 0;
    /// Get all data elements of the given label
    /// \param label
    /// \return
    Elements& elementsOfLabel(Label label) {return _labelMap[label];};
    /// Gets the name of the data
    /// \return
    [[nodiscard]] std::string GetName() const;
    /// Sets the name of the data
    /// \param name
    void SetName(const std::string& name);
    /// Gets a reference to the labels
    /// \return
    Labels& labels() {return _labels;};
    /// Gets a const reference to the labels
    /// \return
    [[nodiscard]] const Labels& GetLabels() const {return _labels;};
    /// Sets elementsPerClass training elements randomly
    /// \param elementsPerClass
    /// \param gen
    void SetTrainingElements(int elementsPerClass, std::mt19937_64& gen);
    /// Gets training set
    /// \return
    std::vector<std::set<Element>>& GetTrainingSet(){return trainingSet;};

    std::vector<std::set<Element>> trainingSet;
protected:
    T dataObject;
    std::string _name;
    std::map<Label, Elements> _labelMap;
    Labels _labels;
    size_t classNumber = 0;
};

#include "Data.txx"

#endif //CLOSURES_DATA_H

