#include "performance_test_multiply.h"

ofstream fout("test_results/tmp/1_power_raising_test.csv", ios_base::app);
ofstream fout_avg("test_results/tmp/1_power_raising_test_averages.csv",
                  ios_base::app);

void operationsOnPlain(vector<double> &plain) {
    for (int i = 0; i < times_to_square; i++) {
        multiply_inplace(plain, plain);
    }
}

template <typename CiphertextT>
void operationsOnEncrypted(CiphertextT &encrypted) {
    for (int i = 0; i < times_to_square; i++) {
        encrypted *= encrypted;
    }
}

int main(int argc, char *argv[]) {
    do_all_tests(argc, argv);
    return 0;
}