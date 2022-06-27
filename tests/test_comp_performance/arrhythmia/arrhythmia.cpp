#include "../performance_test_neural_network.h"
#include "architecture/ActivationFunction.h"
#include "architecture/Layer.h"
#include "tools/Config.h"
#include "tools/IOStream.h"
#include "tools/SystemTools.h"
#include "tools/json/JSONModel.h"
using namespace std;

ofstream fout("test_results/tmp/3_nn_test.csv", ios_base::app);
ofstream fout_avg("test_results/tmp/3_nn_test_averages.csv", ios_base::app);

// Relative to test_comp_performance (parent) folder
#define WEIGHTS_FOLDER "arrhythmia/weights/"

// TODO: move Dataset stuff to src/data/
struct ArrhythmiaDataset {
    vector<vector<double>> dataPoints;
    vector<int> classes;
};

ArrhythmiaDataset readArrhythmiaDataset(string pathToCsv);
template <> ArrhythmiaDataset readDataset() {
    string pathToCsv =
        Config::getConfig()->get<string>("datasets", "arrhythmia-data");
    return readArrhythmiaDataset(pathToCsv);
}

int main(int argc, char *argv[]) {
    do_all_tests_nn<ArrhythmiaDataset>(argc, argv);

    return 0;
}

/** 
 * Model: "sequential"
 * _________________________________________________________________
 *  Layer (type)                Output Shape              Param #   
 * =================================================================
 * dense1 (Dense)              (None, 64)                16704     
 * 
 * dense2 (Dense)              (None, 16)                1040
 * 
 * =================================================================
 **/
template <typename FactoryT, typename CiphertextT>
MODEL_TEMPLATE createModel(FactoryT &factory) {
    /// create Tensor factories for both weights and data
    HETensorFactory<CiphertextT> hetfactory(&factory);
    PlainTensorFactory<double> ptFactory;

    TensorP<CiphertextT> input = hetfactory.create(Shape({1, 260}));

    /// instantiate model
    /// we need to specify the way memory is allocated and pass factories for the data tensors and weight tensors
    Model<CiphertextT, double, HETensor<CiphertextT>, PlainTensor<double>>
        model(MemoryUsage::greedy, &hetfactory, &ptFactory);

    /// add layers
    /// the first layer needs to be passed an input tensor. all other tensors will be created automatically
    // dense 1
    model.addLayer(
        std::make_shared<Dense<CiphertextT, double, HETensor<CiphertextT>,
                               PlainTensor<double>>>(
            "dense1", SquareActivation<CiphertextT>::getSharedPointer(), 64,
            input, &hetfactory, &ptFactory));

    // dense 2
    model.addLayer(
        std::make_shared<Dense<CiphertextT, double, HETensor<CiphertextT>,
                               PlainTensor<double>>>(
            "dense2", LinearActivation<CiphertextT>::getSharedPointer(), 16,
            &hetfactory, &ptFactory));

    /// since we have named the layers the same way they were named in keras we only need to specify the directory that
    /// the exported weights
    model.loadWeights(WEIGHTS_FOLDER);

    return model;
}

template <typename FactoryT, typename CiphertextT>
void feedDatasetToModel(FactoryT &factory, ArrhythmiaDataset &dataset,
                        MODEL_TEMPLATE &model) {
    uint batchSize = factory.batchsize();
    /// we don't need to crop the testdata to fit our batchsize
    assert(dataset.dataPoints.size() < batchSize);

    /// encrypt the test data
    /// The "shape" of the data as it is loaded is [ instances, features ] but we need to reshape it to [ features, instances ]
    /// so that we can encrypt all features with the same coordinates from multiple images into one ciphertext.
    vector<CiphertextT> encData;
    for (size_t i = 0; i < dataset.dataPoints[0].size(); i++) {
        /// contains every i th feature from all data points in the test batch
        vector<double> lp;
        /// iterate over all data points in the batch and extract ith pixel
        for (uint b = 0; b < batchSize && b < dataset.dataPoints.size(); b++) {
            lp.push_back(dataset.dataPoints[b][i]);
        }
        /// create a ciphertext contatining all the ith pixles
        auto enc = factory.createCipherText(lp);
        encData.push_back(enc);
    }
    // feed inputs to the model
    Shape orgShape = model.input()->shape;
    model.input()->flatten();
    model.input()->init(encData);
    model.input()->reshape(orgShape);
}

template <typename CiphertextT>
double computeAccuracy(TensorP<double> &decrypted, MODEL_TEMPLATE &model,
                       ArrhythmiaDataset dataset) {
    /// calcuate the accuracy and print results
    auto preds = decrypted->argmaxVector();
    /// Since the number of examples is smaller than batch size, we need to
    /// remove the extra slots
    vector<int> predsForData(preds.begin(),
                             preds.begin() + dataset.classes.size());
    return model.accuracy(predsForData, dataset.classes);
}

// Inspired from https://java2blog.com/read-csv-file-in-cpp/
ArrhythmiaDataset readArrhythmiaDataset(string pathToCsv) {
    ArrhythmiaDataset result;
    vector<string> columnNames;
    string line, word;
    int lineNumber = 0;

    fstream file(pathToCsv, ios::in);
    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream str(line);
            vector<double> row;
            int columnNumber = 0;
            while (getline(str, word, ',')) {
                if (lineNumber == 0) {
                    columnNames.push_back(word);
                } else {
                    if (columnNames[columnNumber] == "Class") {
                        result.classes.push_back(stoi(word));
                    } else {
                        row.push_back(stod(word));
                    }
                }
                columnNumber++;
            }
            if (lineNumber != 0) {
                result.dataPoints.push_back(row);
            }
            lineNumber++;
        }
    } else {
        cout << "File " << pathToCsv << " could not be found!" << endl;
        exit(-1);
    }
    return result;
}