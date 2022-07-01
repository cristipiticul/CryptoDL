#include "../performance_test_neural_network.h"
#include "architecture/ActivationFunction.h"
#include "architecture/Layer.h"
#include "tools/Config.h"
#include "tools/IOStream.h"
#include "tools/SystemTools.h"
#include "tools/json/JSONModel.h"
using namespace std;

vector<string> program_argument_names{"num_filters"};
vector<int> program_argument_values;

ofstream fout("test_results/tmp/5_cnn_test.csv", ios_base::app);
ofstream fout_avg("test_results/tmp/5_cnn_test_averages.csv", ios_base::app);

// Relative to test_comp_performance (parent) folder
#define WEIGHTS_FOLDER "chest/weights/"
#define NUM_OUTPUTS 14

// TODO: move Dataset stuff to src/data/
struct ChestDataset {
    vector<vector<double>> images;
    vector<vector<int>> labels;
};

ChestDataset readChestDataset(string pathToCsv);
template <> ChestDataset readDataset() {
    string pathToCsv =
        Config::getConfig()->get<string>("datasets", "chest-data");
    return readChestDataset(pathToCsv);
}

vector<vector<double>> readPredictionsPlainEval(string pathToCsv);

int main(int argc, char *argv[]) {
    do_all_tests_nn<ChestDataset>(argc, argv);

    return 0;
}

/** 
 * Model: "sequential"
_________________________________________________________________
 Layer (type)                Output Shape              Param #   
=================================================================
 conv2d_1 (Conv2D)           (None, 12, 12, 32)        832       
 conv2d_2 (Conv2D)           (None, 4, 4, 32)          25632     
 flatten (Flatten)           (None, 512)               0         
 dense_1 (Dense)             (None, 32)                16416     
 dense_2 (Dense)             (None, 14)                462      
=================================================================
 **/
template <typename FactoryT, typename CiphertextT>
MODEL_TEMPLATE createModel(FactoryT &factory) {
    /// create Tensor factories for both weights and data
    HETensorFactory<CiphertextT> hetfactory(&factory);
    PlainTensorFactory<double> ptFactory;

    // TODO: is this ok?
    TensorP<CiphertextT> input = hetfactory.create(Shape({1, 1, 28, 28}));

    /// instantiate model
    /// we need to specify the way memory is allocated and pass factories for the data tensors and weight tensors
    Model<CiphertextT, double, HETensor<CiphertextT>, PlainTensor<double>>
        model(MemoryUsage::greedy, &hetfactory, &ptFactory);

    int num_filters = program_argument_values[0];

    /// the first layer needs to be passed an input tensor. all other tensors will be created automatically
    model.addLayer(
        std::make_shared<Convolution2D<
            CiphertextT, double, HETensor<CiphertextT>, PlainTensor<double>>>(
            "conv2d_1", SquareActivation<CiphertextT>::getSharedPointer(),
            num_filters, 5, 2, PADDING_MODE::VALID, input, &hetfactory,
            &ptFactory));
    // model.addLayer(
    //     std::make_shared<Convolution2D<
    //         CiphertextT, double, HETensor<CiphertextT>, PlainTensor<double>>>(
    //         "conv2d_2", SquareActivation<CiphertextT>::getSharedPointer(), 32,
    //         5, 2, PADDING_MODE::VALID, &hetfactory, &ptFactory));

    model.addLayer(
        std::make_shared<Flatten<CiphertextT, double, HETensor<CiphertextT>,
                                 PlainTensor<double>>>("flatten", &hetfactory,
                                                       &ptFactory));

    // model.addLayer(
    //     std::make_shared<Dense<CiphertextT, double, HETensor<CiphertextT>,
    //                            PlainTensor<double>>>(
    //         "dense_1", SquareActivation<CiphertextT>::getSharedPointer(), 32,
    //         &hetfactory, &ptFactory));

    model.addLayer(
        std::make_shared<Dense<CiphertextT, double, HETensor<CiphertextT>,
                               PlainTensor<double>>>(
            "dense_2", LinearActivation<CiphertextT>::getSharedPointer(),
            NUM_OUTPUTS, &hetfactory, &ptFactory));

    /// since we have named the layers the same way they were named in keras we only need to specify the directory that
    /// the exported weights
    model.loadWeights(WEIGHTS_FOLDER);

    return model;
}

template <typename FactoryT, typename CiphertextT>
void feedDatasetToModel(FactoryT &factory, ChestDataset &dataset,
                        MODEL_TEMPLATE &model) {
    uint batchSize = factory.batchsize();
    /// we don't need to crop the testdata to fit our batchsize
    vector<vector<double>> X(dataset.images.begin(),
                             dataset.images.begin() + batchSize);

    /// encrypt the test data
    /// The "shape" of the data as it is loaded is [ instances, features ] but we need to reshape it to [ features, instances ]
    /// so that we can encrypt all features with the same coordinates from multiple images into one ciphertext.
    vector<CiphertextT> encData;
    for (size_t i = 0; i < dataset.images[0].size(); i++) {
        /// contains every i th feature from all data points in the test batch
        vector<double> lp;
        /// iterate over all data points in the batch and extract ith pixel
        for (uint b = 0; b < X.size(); b++) {
            lp.push_back(dataset.images[b][i]);
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
                       ChestDataset dataset) {
    int num_filters = program_argument_values[0];
    stringstream filename;
    filename << "chest/prediction_K";
    filename << num_filters;
    filename << ".csv";
    vector<vector<double>> predictions_plain =
        readPredictionsPlainEval(filename.str());
    double sum_errors = 0.0;

    for (uint i = 0; i < decrypted->shape[0]; ++i) {
        vector<double> current_decryption;
        for (uint j = 0; j < decrypted->shape[1]; ++j) {
            current_decryption.push_back(decrypted->operator[]({i, j}));
        }
        sum_errors +=
            mean_squared_error(current_decryption, predictions_plain[i]);
    }
    return sum_errors / decrypted->shape[0];
}

// Inspired from https://java2blog.com/read-csv-file-in-cpp/
ChestDataset readChestDataset(string pathToCsv) {
    ChestDataset result;
    string line, word;

    fstream file(pathToCsv, ios::in);
    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream str(line);
            vector<double> row;
            vector<int> labels;
            int columnNumber = 0;
            while (getline(str, word, ',')) {
                if (columnNumber < 28 * 28) {
                    row.push_back(stod(word));
                } else {
                    labels.push_back(stoi(word));
                }
                columnNumber++;
            }
            result.images.push_back(row);
            result.labels.push_back(labels);
        }
    } else {
        cout << "File " << pathToCsv << " could not be found!" << endl;
        exit(-1);
    }
    return result;
}

vector<vector<double>> readPredictionsPlainEval(string pathToCsv) {
    vector<vector<double>> result;
    string line, word;

    fstream file(pathToCsv, ios::in);
    if (file.is_open()) {
        while (getline(file, line)) {
            stringstream str(line);
            vector<double> pred_labels;
            while (getline(str, word, ',')) {
                pred_labels.push_back(stod(word));
            }
            result.push_back(pred_labels);
        }
    } else {
        cout << "File " << pathToCsv << " could not be found!" << endl;
        exit(-1);
    }
    return result;
}