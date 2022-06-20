#include <chrono>
#include <ios>
#include <unistd.h>

#include "../../src/architecture/HEBackend/helib/HELIbCipherText.h"
#include "../../src/architecture/HEBackend/seal/SealCipherText.h"
#include "../../src/tools/IOStream.h"
#include "../TestCommons.h"

using namespace std;

#define NUM_TEST_REPETITIONS 5
#define MAX_SEAL_COEFFS 8

ofstream fout("1_test.csv", ios_base::app);
ofstream fout_avg("1_test_averages.csv", ios_base::app);

chrono::microseconds sum_duration(0);
double sum_virtual_memory, sum_resident_memory, sum_mean_squared_error;
int times_to_square;

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

void printSealParams(ofstream &o, const size_t poly_modulus_degree,
                     const vector<int> &coeff_sizes, const int scale_bits,
                     SealCipherTextFactory &factory,
                     bool print_header_and_newline) {
    if (print_header_and_newline) {
        o << "library,N,";
        for (unsigned int i = 0; i < MAX_SEAL_COEFFS; i++) {
            o << "coeff" << i << ",";
        }
        o << "scale,batch_size,," << endl;
    }
    o << "SEAL," << poly_modulus_degree << ",";
    for (int coeff : coeff_sizes) {
        o << coeff << ",";
    }
    assert(coeff_sizes.size() <= MAX_SEAL_COEFFS);
    for (unsigned int i = 0; i < MAX_SEAL_COEFFS - coeff_sizes.size(); i++) {
        o << ",";
    }
    o << scale_bits << "," << factory.batchsize()
      << ",,"; // the empty columns are for c and securityLevel from HELib
    if (print_header_and_newline) {
        o << endl;
    }
}

template <>
SealCipherTextFactory initializeFactory(int argc, char *argv[],
                                        bool print_params) {
    size_t poly_modulus_degree = atoi(argv[4]);
    vector<int> coeff_sizes;
    for (int i = 5; i < argc - 1; i++) {
        coeff_sizes.push_back(atoi(argv[i]));
    }
    int scale_bits = atoi(argv[argc - 1]);
    SealCipherTextFactory factory(poly_modulus_degree, coeff_sizes, scale_bits);
    if (print_params) {
        printSealParams(fout, poly_modulus_degree, coeff_sizes, scale_bits,
                        factory, true);
        printSealParams(fout_avg, poly_modulus_degree, coeff_sizes, scale_bits,
                        factory, false);
    }
    return factory;
}
void printHELibParams(ofstream &o, const int m, const int bits, const int scale,
                      const int c, HELibCipherTextFactory &factory,
                      bool print_header_and_newline) {
    if (print_header_and_newline) {
        o << "library,m,bits,";
        for (unsigned int i = 0; i < MAX_SEAL_COEFFS - 1; i++) {
            o << ",";
        }
        o << "scale,batch_size,c,security" << endl;
    }
    o << "HELib," << m << "," << bits << ",";
    for (unsigned int i = 0; i < MAX_SEAL_COEFFS - 1; i++) {
        o << ",";
    }
    o << scale << "," << factory.batchsize() << "," << c << ","
      << factory.securityLevel();
    if (print_header_and_newline) {
        o << endl;
    }
}

template <>
HELibCipherTextFactory initializeFactory(int argc, char *argv[],
                                         bool print_params) {
    assert(argc == 8);
    int m = atoi(argv[4]);
    int bits = atoi(argv[5]);
    int scale = atoi(argv[6]);
    int c = atoi(argv[7]);
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
    time_start = chrono::high_resolution_clock::now();

    FactoryT factory = initializeFactory<FactoryT>(argc, argv, print_params);
    vector<double> plain = createRandomVector(factory.batchsize(), -1, 1);
    CiphertextT encrypted = factory.createCipherText(plain);
    for (int i = 0; i < times_to_square; i++) {
        encrypted *= encrypted;
        multiply_inplace(plain, plain);
    }
    vector<double> decrypted = factory.decryptDouble(encrypted);

    time_end = chrono::high_resolution_clock::now();
    chrono::microseconds time_diff =
        chrono::duration_cast<chrono::microseconds>(time_end - time_start);
    process_mem_usage(virtual_memory, resident_memory);
    if (print_params) {
        fout << "virtual_memory[MB],resident_memory[MB],duration[us],"
                "mean_squared_error"
             << endl;
    }
    double mse = mean_squared_error(decrypted, plain);
    fout << virtual_memory << "," << resident_memory << "," << time_diff.count()
         << "," << mse << endl;
    sum_virtual_memory += virtual_memory;
    sum_resident_memory += resident_memory;
    sum_duration += time_diff;
    sum_mean_squared_error += mse;
}

int main(int argc, char *argv[]) {
    if (argc < 7) {
        cerr << "Usage:" << endl;
        cerr << "Variant 1: " << argv[0]
             << " times_to_square print_avg_header(0/1) seal N Q1 Q2 ... QL"
                "scale"
             << endl;
        cerr << "Variant 2: " << argv[0]
             << " times_to_square print_avg_header(0/1) helib m bits scale c"
             << endl;
        return 1;
    }
    if (strcmp(argv[2], "1") == 0) {
        fout_avg << "library,m or N,bits or coeff0,";
        for (int i = 0; i < MAX_SEAL_COEFFS - 1; i++) {
            fout_avg << "coeff" << i << ",";
        }
        fout_avg << "scale,batch_size,c,security,";
        fout_avg << "virtual_memory[MB],resident_memory[MB],duration[us],mean_"
                    "squared_error"
                 << endl;
    }

    times_to_square = atoi(argv[1]);
    fout << "times_to_square: " << times_to_square << endl;
    for (int i = 0; i < NUM_TEST_REPETITIONS; i++) {
        bool print_params = (i == 0);
        if (strcmp(argv[3], "seal") == 0) {
            test<SealCipherTextFactory, SealCipherText>(argc, argv,
                                                        print_params);
        } else if (strcmp(argv[3], "helib") == 0) {
            test<HELibCipherTextFactory, HELibCipherText>(argc, argv,
                                                          print_params);
        } else {
            cerr << "Unknown library: " << argv[3] << endl;
            return 1;
        }
    }
    fout_avg << ",";
    fout_avg << sum_virtual_memory / NUM_TEST_REPETITIONS << ",";
    fout_avg << sum_resident_memory / NUM_TEST_REPETITIONS << ",";
    fout_avg << sum_duration.count() / NUM_TEST_REPETITIONS << ",";
    fout_avg << sum_mean_squared_error / NUM_TEST_REPETITIONS << ",";
    fout_avg << endl;
    fout.close();
    fout_avg.close();
    return 0;
}