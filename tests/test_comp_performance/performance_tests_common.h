#include <chrono>
#include <ios>
#include <unistd.h>

#include "../TestCommons.h"
#include "architecture/HEBackend/helib/HELIbCipherText.h"
#include "architecture/HEBackend/seal/SealCipherText.h"
#include "tools/IOStream.h"

using namespace std;

extern ofstream fout;
extern ofstream fout_avg;

struct SealParams {
    size_t poly_modulus_degree;
    int log_big_primes;
    int num_small_primes;
    int log_small_primes;
    int scale_bits;
    uint batch_size;
};

struct HELibParams {
    int m;
    int bits;
    int scale;
    int c;
    uint batch_size;
};

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

template <typename FactoryT, typename ParamsT>
FactoryT initializeFactory(ParamsT params);

int getLogQ(int log_big_primes, int num_small_primes, int log_small_primes) {
    return 2 * log_big_primes + num_small_primes * log_small_primes;
}

template <typename ParamsT>
ParamsT getParams(int argc, char *argv[], int startingArgIndex);
template <typename ParamsT> void printParams(ofstream &o, ParamsT params);

template <>
SealParams getParams<SealParams>(int argc, char *argv[], int startingArgIndex) {
    assert(argc == startingArgIndex + 5);
    SealParams params;
    params.poly_modulus_degree = atoi(argv[startingArgIndex]);
    params.log_big_primes = atoi(argv[startingArgIndex + 1]);
    params.num_small_primes = atoi(argv[startingArgIndex + 2]);
    params.log_small_primes = atoi(argv[startingArgIndex + 3]);
    params.scale_bits = atoi(argv[startingArgIndex + 4]);
    params.batch_size = params.poly_modulus_degree / 2;
    return params;
}

template <>
HELibParams getParams<HELibParams>(int argc, char *argv[],
                                   int startingArgIndex) {
    HELibParams params;
    assert(argc == startingArgIndex + 4);
    params.m = atoi(argv[startingArgIndex]);
    params.bits = atoi(argv[startingArgIndex + 1]);
    params.scale = atoi(argv[startingArgIndex + 2]);
    params.c = atoi(argv[startingArgIndex + 3]);
    params.batch_size = params.m / 4;
    return params;
}

void printGenericParamsHeader(ostream &out) {
    out << "library,";
    out << "batch_size,";
    out << "m or N,";
    out << "bits or logQ,";
    out << "log_big_primes,";
    out << "nr_small_primes,";
    out << "log_small_primes,";
    out << "scale,";
    out << "c";
}

template <> void printParams(ofstream &o, SealParams params) {
    o << "SEAL,";
    o << params.batch_size << ",";
    o << params.poly_modulus_degree << ",";
    o << getLogQ(params.log_big_primes, params.num_small_primes,
                 params.log_small_primes)
      << ",";
    o << params.log_big_primes << ",";
    o << params.num_small_primes << ",";
    o << params.log_small_primes << ",";
    o << params.scale_bits;
    o << ","; // the empty column is for c from HELib
}

template <> void printParams(ofstream &o, HELibParams params) {
    o << "HELib,";
    o << params.batch_size << ",";
    o << params.m << ",";
    o << params.bits << ",";
    o << ",,,"; // empty columns for log_big_primes, num_small_primes, log_small_primes
    o << params.scale << ",";
    o << params.c;
    // TODO: maybe also print the security estimate by creating a dummy factory.
}

template <> SealCipherTextFactory initializeFactory(SealParams params) {
    vector<int> coeff_sizes;
    // SEAL recommendation:
    // Start with biggest prime for best accuracy
    coeff_sizes.push_back(params.log_big_primes);
    // Then put small primes
    for (int i = 0; i < params.num_small_primes; i++) {
        coeff_sizes.push_back(params.log_small_primes);
    }
    // Then put special prime (equal to biggest prime)
    coeff_sizes.push_back(params.log_big_primes);

    SealCipherTextFactory factory(params.poly_modulus_degree, coeff_sizes,
                                  params.scale_bits);
    return factory;
}

template <> HELibCipherTextFactory initializeFactory(HELibParams params) {
    HELibCipherTextFactory factory(params.bits, params.m, params.scale,
                                   params.c);
    return factory;
}
