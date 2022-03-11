#include <seal/seal.h>
#include "SealCipherText.h"

class SealCipherText {
public:
    SealCipherText(seal::Ciphertext ct, seal::SEALContext *context,
                   seal::Evaluator *evaluator, seal::CKKSEncoder *encoder)
        : mCipherText(ct), mContex(context), mEvaluator(evaluator),
          mEncoder(encoder) {
    }

    SealCipherText &operator+(const long x) {

        return *this;
    }

private:
    seal::Ciphertext mCipherText;
    seal::SEALContext *mContex;
    seal::Evaluator *mEvaluator;
    seal::CKKSEncoder *mEncoder;
};
