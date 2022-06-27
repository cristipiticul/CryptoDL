#include <chrono>
#include <ios>
#include <unistd.h>

#include "../TestCommons.h"
#include "architecture/HEBackend/HETensor.h"
#include "architecture/HEBackend/helib/HELIbCipherText.h"
#include "architecture/HEBackend/seal/SealCipherText.h"
#include "architecture/Model.h"
#include "architecture/PlainTensor.h"
#include "performance_tests_common.h"
#include "tools/IOStream.h"

using namespace std;

#define MODEL_TEMPLATE                                                         \
    Model<CiphertextT, double, HETensor<CiphertextT>, PlainTensor<double>>

int print_avg_header;
int number_of_tests;

chrono::microseconds sum_key_generation_duration(0);
chrono::microseconds sum_model_initialization_duration(0);
chrono::microseconds sum_encryption_duration(0);
chrono::microseconds sum_operations_duration(0);
chrono::microseconds sum_decryption_duration(0);
double sum_virtual_memory;
double sum_resident_memory;
double sum_accuracy;

template <typename DatasetT> DatasetT readDataset();
template <typename FactoryT, typename CiphertextT>
MODEL_TEMPLATE createModel(FactoryT &factory);
template <typename FactoryT, typename CiphertextT, typename DatasetT>
void feedDatasetToModel(FactoryT &factory, DatasetT &dataset,
                        MODEL_TEMPLATE &model);
template <typename CiphertextT, typename DatasetT>
double computeAccuracy(TensorP<double> &decrypted, MODEL_TEMPLATE &model,
                       DatasetT dataset);

void printExperimentsHeader(ofstream &out) {
    out << "virtual_memory[MB],";
    out << "resident_memory[MB],";
    out << "keygen_duration[us],";
    out << "model_initialization_duration[us],";
    out << "encryption_duration[us],";
    out << "operations_duration[us],";
    out << "decryption_duration[us],";
    out << "accuracy";
}

template <typename FactoryT, typename CiphertextT, typename DatasetT,
          typename ParamsT>
void test_nn(ParamsT params) {
    double virtual_memory, resident_memory;
    chrono::high_resolution_clock::time_point time_start, time_end;
    chrono::microseconds key_generation_duration, encryption_duration,
        operations_duration, decryption_duration, model_initialization_duration;

    /** Key generation */
    time_start = chrono::high_resolution_clock::now();
    FactoryT factory = initializeFactory<FactoryT>(params);
    time_end = chrono::high_resolution_clock::now();
    key_generation_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    DatasetT dataset = readDataset<DatasetT>();

    /** Model initialization */
    time_start = chrono::high_resolution_clock::now();
    MODEL_TEMPLATE model = createModel<FactoryT, CiphertextT>(factory);
    time_end = chrono::high_resolution_clock::now();
    model_initialization_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    /** Encrypt and feed dataset to model */
    time_start = chrono::high_resolution_clock::now();
    feedDatasetToModel(factory, dataset, model);
    time_end = chrono::high_resolution_clock::now();
    encryption_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    /** Run model */
    time_start = chrono::high_resolution_clock::now();
    model.run();
    time_end = chrono::high_resolution_clock::now();
    operations_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    /** Decryption */
    time_start = chrono::high_resolution_clock::now();
    TensorP<double> decrypted =
        ((HETensor<CiphertextT> *)model.mLayers.back()->output().get())
            ->decryptDouble();
    time_end = chrono::high_resolution_clock::now();
    decryption_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    process_mem_usage(virtual_memory, resident_memory);
    double accuracy = computeAccuracy(decrypted, model, dataset);
    fout << virtual_memory << "," << resident_memory << ","
         << key_generation_duration.count() << ","
         << model_initialization_duration.count() << ","
         << encryption_duration.count() << "," << operations_duration.count()
         << "," << decryption_duration.count() << "," << accuracy << endl;
    sum_virtual_memory += virtual_memory;
    sum_resident_memory += resident_memory;
    sum_key_generation_duration += key_generation_duration;
    sum_model_initialization_duration += model_initialization_duration;
    sum_encryption_duration += encryption_duration;
    sum_operations_duration += operations_duration;
    sum_decryption_duration += decryption_duration;
    sum_accuracy += accuracy;
}

template <typename FactoryT, typename CiphertextT, typename DatasetT,
          typename ParamsT>
void read_params_and_test_nn(int argc, char *argv[]) {
    ParamsT params = getParams<ParamsT>(argc, argv, 4);
    printGenericParamsHeader(fout);
    fout << endl;
    printParams<ParamsT>(fout, params);
    fout << endl;
    printExperimentsHeader(fout);
    fout << endl;
    for (int i = 0; i < number_of_tests; i++) {
        test_nn<FactoryT, CiphertextT, DatasetT>(params);
    }
    printAverages(params);
}

template <typename ParamsT> void printAverages(ParamsT params) {
    if (print_avg_header == 1) {
        printGenericParamsHeader(fout_avg);
        fout_avg << ",number_of_tests,";
        printExperimentsHeader(fout_avg);
        fout_avg << endl;
    }
    printParams(fout_avg, params);
    fout_avg << ",";
    fout_avg << number_of_tests << ",";
    fout_avg << sum_virtual_memory / number_of_tests << ",";
    fout_avg << sum_resident_memory / number_of_tests << ",";
    fout_avg << sum_key_generation_duration.count() / number_of_tests << ",";
    fout_avg << sum_model_initialization_duration.count() / number_of_tests
             << ",";
    fout_avg << sum_encryption_duration.count() / number_of_tests << ",";
    fout_avg << sum_operations_duration.count() / number_of_tests << ",";
    fout_avg << sum_decryption_duration.count() / number_of_tests << ",";
    fout_avg << sum_accuracy / number_of_tests << ",";
    fout_avg << endl;
}

template <typename DatasetT> void do_all_tests_nn(int argc, char *argv[]) {
    if (argc < 8) {
        cerr << "Usage:" << endl;
        cerr << "Variant 1: " << argv[0]
             << " number_of_tests print_avg_header(0/1) seal N "
                "log_big_primes "
                "nr_small_primes log_small_primes scale"
             << endl;
        cerr << "Variant 2: " << argv[0]
             << " number_of_tests print_avg_header(0/1) helib m bits "
                "scale c"
             << endl;
        return;
    }
    number_of_tests = atoi(argv[1]);
    print_avg_header = atoi(argv[2]);
    char *library = argv[3];

    fout << "number_of_tests: " << number_of_tests << endl;
    if (strcmp(library, "seal") == 0) {
        read_params_and_test_nn<SealCipherTextFactory, SealCipherText, DatasetT,
                                SealParams>(argc, argv);
    } else if (strcmp(library, "helib") == 0) {
        read_params_and_test_nn<HELibCipherTextFactory, HELibCipherText,
                                DatasetT, HELibParams>(argc, argv);
    } else {
        cerr << "Unknown library: " << library << endl;
        return;
    }
    fout.close();
    fout_avg.close();
}