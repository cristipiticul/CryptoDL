#include "performance_tests_common.h"

ofstream fout("test_results/tmp/2_multiply_plain_test.csv", ios_base::app);
ofstream fout_avg("test_results/tmp/2_multiply_plain_test_averages.csv",
                  ios_base::app);

void operationsOnPlain(vector<double> &plain) {
    for (int i = 0; i < times_to_square; i++) {
        multiply_inplace(plain, (double)max_nr);
    }
}

template <typename CiphertextT>
void operationsOnEncrypted(CiphertextT &encrypted) {
    for (int i = 0; i < times_to_square; i++) {
        encrypted *= (double)max_nr;
    }
}

int main(int argc, char *argv[]) {
    do_all_tests(argc, argv);
    return 0;
}