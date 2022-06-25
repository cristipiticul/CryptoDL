#include <chrono>
#include <ios>
#include <unistd.h>

#include "../../src/architecture/HEBackend/helib/HELIbCipherText.h"
#include "../../src/architecture/HEBackend/seal/SealCipherText.h"
#include "../../src/tools/IOStream.h"
#include "../TestCommons.h"

using namespace std;

#define NUM_TEST_REPETITIONS 5

extern ofstream fout;
extern ofstream fout_avg;

void operationsOnPlain(vector<double> &plain);

template <typename CiphertextT>
void operationsOnEncrypted(CiphertextT &encrypted);

chrono::microseconds sum_key_generation_duration(0), sum_encryption_duration(0),
    sum_operations_duration(0), sum_decryption_duration(0);
double sum_virtual_memory, sum_resident_memory, sum_mean_squared_error;
int times_to_square;
int max_nr;

// From https://stackoverflow.com/a/671389
//////////////////////////////////////////////////////////////////////////////
//
// process_mem_usage(double &, double &) - takes two doubles by reference,
// attempts to read the system-dependent data for a process' virtual memory
// size and resident set size, and return the results in MB.
//
// On failure, returns 0.0, 0.0

void process_mem_usage(double &vm_usage, double &resident_set) {
    using std::ifstream;
    using std::ios_base;
    using std::string;

    vm_usage = 0.0;
    resident_set = 0.0;

    // 'file' stat seems to give the most reliable results
    //
    ifstream stat_stream("/proc/self/stat", ios_base::in);

    // dummy vars for leading entries in stat that we don't care about
    //
    string pid, comm, state, ppid, pgrp, session, tty_nr;
    string tpgid, flags, minflt, cminflt, majflt, cmajflt;
    string utime, stime, cutime, cstime, priority, nice;
    string O, itrealvalue, starttime;

    // the two fields we want
    //
    unsigned long vsize;
    long rss;

    stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
        stime >> cutime >> cstime >> priority >> nice >> O >> itrealvalue >>
        starttime >> vsize >> rss; // don't care about the rest

    stat_stream.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) /
                        1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / (1024.0 * 1024.0);
    resident_set = rss * page_size_kb / 1024.0;
}

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

double mean_squared_error(const vector<double> &v1, const vector<double> &v2) {
    assert(v1.size() == v2.size());
    double sum_of_distances = 0.0;
    for (unsigned int i = 0; i < v1.size(); i++) {
        sum_of_distances += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }
    return sum_of_distances / v1.size();
}

template <typename FactoryT>
FactoryT initializeFactory(int argc, char *argv[], bool print_params);

int getLogQ(int log_big_primes, int num_small_primes, int log_small_primes) {
    return 2 * log_big_primes + num_small_primes * log_small_primes;
}

void printSealParams(ofstream &o, const size_t poly_modulus_degree,
                     const int log_big_primes, const int num_small_primes,
                     const int log_small_primes, const int scale_bits,
                     SealCipherTextFactory &factory,
                     bool print_header_and_newline) {
    if (print_header_and_newline) {
        o << "library,batch_size,N,logQ,log_big_primes,nr_small_primes,log_"
             "small_primes,";
        o << "scale,," << endl;
    }
    o << "SEAL," << factory.batchsize() << "," << poly_modulus_degree << ",";
    o << getLogQ(log_big_primes, num_small_primes, log_small_primes) << ",";
    o << log_big_primes << "," << num_small_primes << "," << log_small_primes
      << ",";
    o << scale_bits
      << ",,"; // the empty columns are for c and securityLevel from HELib
    if (print_header_and_newline) {
        o << endl;
    }
}

template <>
SealCipherTextFactory initializeFactory(int argc, char *argv[],
                                        bool print_params) {
    assert(argc == 10);
    size_t poly_modulus_degree = atoi(argv[5]);
    int log_big_primes = atoi(argv[6]);
    int num_small_primes = atoi(argv[7]);
    int log_small_primes = atoi(argv[8]);
    int scale_bits = atoi(argv[9]);
    vector<int> coeff_sizes;
    // SEAL recommendation:
    // Start with biggest prime for best accuracy
    coeff_sizes.push_back(log_big_primes);
    // Then put small primes
    for (int i = 0; i < num_small_primes; i++) {
        coeff_sizes.push_back(log_small_primes);
    }
    // Then put special prime (equal to biggest prime)
    coeff_sizes.push_back(log_big_primes);

    SealCipherTextFactory factory(poly_modulus_degree, coeff_sizes, scale_bits);
    if (print_params) {
        printSealParams(fout, poly_modulus_degree, log_big_primes,
                        num_small_primes, log_small_primes, scale_bits, factory,
                        true);
        printSealParams(fout_avg, poly_modulus_degree, log_big_primes,
                        num_small_primes, log_small_primes, scale_bits, factory,
                        false);
    }
    return factory;
}
void printHELibParams(ofstream &o, const int m, const int bits, const int scale,
                      const int c, HELibCipherTextFactory &factory,
                      bool print_header_and_newline) {
    if (print_header_and_newline) {
        o << "library,batch_size,m,bits,,,,";
        o << "scale,c,security" << endl;
    }
    o << "HELib," << factory.batchsize() << "," << m << "," << bits << ",,,,";
    o << scale << "," << c << "," << factory.securityLevel();
    if (print_header_and_newline) {
        o << endl;
    }
}

template <>
HELibCipherTextFactory initializeFactory(int argc, char *argv[],
                                         bool print_params) {
    assert(argc == 9);
    int m = atoi(argv[5]);
    int bits = atoi(argv[6]);
    int scale = atoi(argv[7]);
    int c = atoi(argv[8]);
    HELibCipherTextFactory factory(bits, m, scale, c);
    if (print_params) {
        printHELibParams(fout, m, bits, scale, c, factory, true);
        printHELibParams(fout_avg, m, bits, scale, c, factory, false);
    }
    return factory;
}

template <typename FactoryT, typename CiphertextT>
void test(int argc, char *argv[], bool print_params) {
    double virtual_memory, resident_memory;
    chrono::high_resolution_clock::time_point time_start, time_end;
    chrono::microseconds key_generation_duration, encryption_duration,
        operations_duration, decryption_duration;

    /** Key generation */
    time_start = chrono::high_resolution_clock::now();
    FactoryT factory = initializeFactory<FactoryT>(argc, argv, print_params);
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
    if (print_params) {
        fout << "virtual_memory[MB],resident_memory[MB],keygen_duration[us],"
                "encryption_duration[us],operations_duration[us],"
                "decryption_duration[us],mean_squared_error"
             << endl;
    }
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

void do_all_tests(int argc, char *argv[]) {
    if (argc < 9) {
        cerr << "Usage:" << endl;
        cerr << "Variant 1: " << argv[0]
             << " times_to_square print_avg_header(0/1) max_nr seal N "
                "log_big_primes "
                "nr_small_primes log_small_primes scale"
             << endl;
        cerr << "Variant 2: " << argv[0]
             << " times_to_square print_avg_header(0/1) max_nr helib m bits "
                "scale c"
             << endl;
        return;
    }
    times_to_square = atoi(argv[1]);
    int print_avg_header = atoi(argv[2]);
    max_nr = atoi(argv[3]);
    char *library = argv[4];
    if (print_avg_header == 1) {
        fout_avg << "library,batch_size,m or N,bits or "
                    "logQ,log_big_primes,nr_small_primes,log_small_primes,";
        fout_avg << "scale,c,security,";
        fout_avg << "times_to_square,max_nr,virtual_memory[MB],"
                    "resident_memory[MB],keygen_duration[us],"
                    "encryption_duration[us],operations_duration[us],"
                    "decryption_duration[us],mean_squared_error"
                 << endl;
    }

    fout << "times_to_square: " << times_to_square << ",max_nr:" << max_nr
         << endl;
    for (int i = 0; i < NUM_TEST_REPETITIONS; i++) {
        bool print_params = (i == 0);
        if (strcmp(library, "seal") == 0) {
            test<SealCipherTextFactory, SealCipherText>(argc, argv,
                                                        print_params);
        } else if (strcmp(library, "helib") == 0) {
            test<HELibCipherTextFactory, HELibCipherText>(argc, argv,
                                                          print_params);
        } else {
            cerr << "Unknown library: " << library << endl;
            return;
        }
    }
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
    fout.close();
    fout_avg.close();
}