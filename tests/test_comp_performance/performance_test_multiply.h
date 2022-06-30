#include "performance_tests_common.h"
using namespace std;
#define NUM_TEST_REPETITIONS 5

chrono::microseconds sum_key_generation_duration(0);
chrono::microseconds sum_encryption_duration(0);
chrono::microseconds sum_operations_duration(0);
chrono::microseconds sum_decryption_duration(0);
double sum_virtual_memory;
double sum_resident_memory;
double sum_mean_squared_error;
int print_avg_header;
int times_to_square;
int max_nr;

void printExperimentsHeader(ofstream &out) {
    out << "virtual_memory[MB],";
    out << "resident_memory[MB],";
    out << "keygen_duration[us],";
    out << "encryption_duration[us],";
    out << "operations_duration[us],";
    out << "decryption_duration[us],";
    out << "mean_squared_error";
}

void operationsOnPlain(vector<double> &plain);

template <typename CiphertextT>
void operationsOnEncrypted(CiphertextT &encrypted);

void multiply_inplace(vector<double> &v1, vector<double> &v2) {
    for (unsigned int i = 0; i < v1.size(); i++) {
        v1[i] *= v2[i];
    }
}
void multiply_inplace(vector<double> &v1, double constant) {
    for (unsigned int i = 0; i < v1.size(); i++) {
        v1[i] *= constant;
    }
}
void add_inplace(vector<double> &v1, vector<double> &v2) {
    for (unsigned int i = 0; i < v1.size(); i++) {
        v1[i] += v2[i];
    }
}

double mean_squared_error(const vector<double> &v1, const vector<double> &v2) {
    assert(v1.size() == v2.size());
    double sum_of_distances = 0.0;
    for (unsigned int i = 0; i < v1.size(); i++) {
        sum_of_distances += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return sum_of_distances / v1.size();
}

template <typename FactoryT, typename CiphertextT, typename ParamsT>
void test(ParamsT params) {
    double virtual_memory, resident_memory;
    chrono::high_resolution_clock::time_point time_start, time_end;
    chrono::microseconds key_generation_duration, encryption_duration,
        operations_duration, decryption_duration;

    /** Key generation */
    time_start = chrono::high_resolution_clock::now();
    FactoryT factory = initializeFactory<FactoryT>(params);
    assert(factory.batchsize() == params.batch_size);
    time_end = chrono::high_resolution_clock::now();
    key_generation_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    vector<double> plain =
        createRandomVector(factory.batchsize(), -max_nr, max_nr);

    /** Encryption */
    time_start = chrono::high_resolution_clock::now();
    CiphertextT encrypted = factory.createCipherText(plain);
    time_end = chrono::high_resolution_clock::now();
    encryption_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    /** Operations on ciphertext */
    time_start = chrono::high_resolution_clock::now();
    operationsOnEncrypted(encrypted);
    time_end = chrono::high_resolution_clock::now();
    operations_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    /** Operations on plaintext */
    operationsOnPlain(plain);

    /** Decryption */
    time_start = chrono::high_resolution_clock::now();
    vector<double> decrypted = factory.decryptDouble(encrypted);
    time_end = chrono::high_resolution_clock::now();
    decryption_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    process_mem_usage(virtual_memory, resident_memory);
    double mse = mean_squared_error(decrypted, plain);
    fout << virtual_memory << "," << resident_memory << ","
         << key_generation_duration.count() << ","
         << encryption_duration.count() << "," << operations_duration.count()
         << "," << decryption_duration.count() << "," << mse << endl;
    sum_virtual_memory += virtual_memory;
    sum_resident_memory += resident_memory;
    sum_key_generation_duration += key_generation_duration;
    sum_encryption_duration += encryption_duration;
    sum_operations_duration += operations_duration;
    sum_decryption_duration += decryption_duration;
    sum_mean_squared_error += mse;
}

template <typename FactoryT, typename CiphertextT, typename ParamsT>
void read_params_and_test(int argc, char *argv[]) {
    ParamsT params = getParams<ParamsT>(argc, argv, 5);
    printGenericParamsHeader(fout);
    fout << endl;
    printParams<ParamsT>(fout, params);
    fout << endl;
    printExperimentsHeader(fout);
    fout << endl;
    for (int i = 0; i < NUM_TEST_REPETITIONS; i++) {
        test<FactoryT, CiphertextT>(params);
    }
    printAverages(params);
}

template <typename ParamsT> void printAverages(ParamsT params) {
    if (print_avg_header == 1) {
        printGenericParamsHeader(fout_avg);
        fout_avg << ",times_to_square,max_nr,";
        printExperimentsHeader(fout_avg);
        fout_avg << endl;
    }
    printParams(fout_avg, params);
    fout_avg << ",";
    fout_avg << times_to_square << "," << max_nr << ",";
    fout_avg << sum_virtual_memory / NUM_TEST_REPETITIONS << ",";
    fout_avg << sum_resident_memory / NUM_TEST_REPETITIONS << ",";
    fout_avg << sum_key_generation_duration.count() / NUM_TEST_REPETITIONS
             << ",";
    fout_avg << sum_encryption_duration.count() / NUM_TEST_REPETITIONS << ",";
    fout_avg << sum_operations_duration.count() / NUM_TEST_REPETITIONS << ",";
    fout_avg << sum_decryption_duration.count() / NUM_TEST_REPETITIONS << ",";
    fout_avg << sum_mean_squared_error / NUM_TEST_REPETITIONS << ",";
    fout_avg << endl;
}

void do_all_tests(int argc, char *argv[]) {
    times_to_square = atoi(argv[1]);
    print_avg_header = atoi(argv[2]);
    max_nr = atoi(argv[3]);
    char *library = argv[4];

    fout << "times_to_square: " << times_to_square << ",max_nr:" << max_nr
         << endl;

    if (strcmp(library, "seal") == 0) {
        read_params_and_test<SealCipherTextFactory, SealCipherText, SealParams>(
            argc, argv);
    } else if (strcmp(library, "helib") == 0) {
        read_params_and_test<HELibCipherTextFactory, HELibCipherText,
                             HELibParams>(argc, argv);
    } else {
        cerr << "Unknown library: " << library << endl;
        return;
    }
    fout.close();
    fout_avg.close();
}