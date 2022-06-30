#include "performance_test_multiply.h"
using namespace std;

ofstream fout("test_results/tmp/4_mock_neuron_test.csv", ios_base::app);
ofstream fout_avg("test_results/tmp/4_mock_neuron_test_averages.csv",
                  ios_base::app);

void operationsOnPlain(vector<double> &plain) {
}

template <typename CiphertextT>
void operationsOnEncrypted(CiphertextT &encrypted) {
}

template <typename FactoryT, typename CiphertextT, typename ParamsT>
void test_neuron(ParamsT params) {
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

    vector<double> plain = createRandomVector(factory.batchsize(), 0, max_nr);
    vector<double> plain_copy = plain;

    /** Encryption */
    time_start = chrono::high_resolution_clock::now();
    CiphertextT encrypted = factory.createCipherText(plain);
    CiphertextT encrypted_copy = factory.createCipherText(plain);
    time_end = chrono::high_resolution_clock::now();
    encryption_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    /** Operations on ciphertext */
    time_start = chrono::high_resolution_clock::now();
    int num_multiplications = 32;
    for (int i = 0; i < times_to_square; i++) {
        CiphertextT result = factory.createCipherText(0.0);
        vector<double> plain_result =
            createRandomVector(factory.batchsize(), 0, 0);
        for (int j = 0; j < num_multiplications; j++) {
            CiphertextT encrypted_copy = factory.empty();
            encrypted_copy = encrypted;
            vector<double> plain_copy;
            plain_copy = plain;

            double weight =
                static_cast<double>(std::rand()) / RAND_MAX * 0.5 - 0.25;
            // double weight = 0.3;
            encrypted_copy *= weight;
            multiply_inplace(plain_copy, weight);

            result += encrypted_copy;
            add_inplace(plain_result, plain_copy);
        }
        // result *= result;
        // multiply_inplace(plain_result, plain_result);

        encrypted = result;
        plain = plain_result;
    }
    time_end = chrono::high_resolution_clock::now();
    operations_duration =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);

    /** Operations on plaintext */
    // operationsOnPlain(plain);

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
void read_params_and_test_neuron(int argc, char *argv[]) {
    ParamsT params = getParams<ParamsT>(argc, argv, 5);
    printGenericParamsHeader(fout);
    fout << endl;
    printParams<ParamsT>(fout, params);
    fout << endl;
    printExperimentsHeader(fout);
    fout << endl;
    for (int i = 0; i < NUM_TEST_REPETITIONS; i++) {
        test_neuron<FactoryT, CiphertextT>(params);
    }
    printAverages(params);
}

int main(int argc, char *argv[]) {
    times_to_square = atoi(argv[1]);
    print_avg_header = atoi(argv[2]);
    max_nr = atoi(argv[3]);
    char *library = argv[4];

    fout << "times_to_square: " << times_to_square << ",max_nr:" << max_nr
         << endl;

    if (strcmp(library, "seal") == 0) {
        read_params_and_test_neuron<SealCipherTextFactory, SealCipherText,
                                    SealParams>(argc, argv);
    } else if (strcmp(library, "helib") == 0) {
        read_params_and_test_neuron<HELibCipherTextFactory, HELibCipherText,
                                    HELibParams>(argc, argv);
    } else {
        cerr << "Unknown library: " << library << endl;
        return 1;
    }
    fout.close();
    fout_avg.close();
    return 0;
}