#include "SealCipherText.h"
#include <seal/seal.h>

SealCipherTextFactory *SealCipherText::defaultFactory = nullptr;

SealCipherText::SealCipherText()
    : mFactory(defaultFactory), mCiphertext(mFactory->createRawEmpty()) {
}

SealCipherText &SealCipherText::operator+=(SealCipherText &other) {
    mFactory->bringToSameLevel(*this, other);
    mFactory->evaluator->add_inplace(*mCiphertext, *other.mCiphertext);
    return *this;
}

SealCipherText &SealCipherText::operator*=(SealCipherText &other) {
    mFactory->bringToSameLevel(*this, other);
    mFactory->evaluator->multiply_inplace(*mCiphertext, *other.mCiphertext);
    mFactory->evaluator->relinearize_inplace(*mCiphertext,
                                             *mFactory->relin_keys);
    mFactory->evaluator->rescale_to_next_inplace(*mCiphertext);
    // mFactory->fixScale(*this);
    return *this;
}

SealCipherText &SealCipherText::operator+=(SealCipherText *other) {
    return this->operator+=(*other);
}

SealCipherText &SealCipherText::operator*=(SealCipherText *other) {
    return this->operator*=(*other);
}

SealCipherText &SealCipherText::operator=(const SealCipherText &other) {
    *this->mCiphertext = other.ctxt();
    return *this;
}

SealCipherText &SealCipherText::operator+=(double plain) {
    seal::Plaintext xPlain =
        mFactory->createPlainText(plain, mCiphertext->scale());
    return this->operator+=(xPlain);
}

SealCipherText &SealCipherText::operator+=(const std::vector<double> &plain) {
    seal::Plaintext xPlain =
        mFactory->createPlainText(plain, mCiphertext->scale());
    return this->operator+=(xPlain);
}

SealCipherText &SealCipherText::operator+=(seal::Plaintext &plain) {
    mFactory->evaluator->mod_switch_to_inplace(plain, mCiphertext->parms_id());
    mFactory->evaluator->add_plain_inplace(*mCiphertext, plain);
    return *this;
}

SealCipherText &SealCipherText::operator*=(double x) {
    seal::Plaintext xPlain = mFactory->createPlainText(x, mCiphertext->scale());
    return this->operator*=(xPlain);
}
SealCipherText &SealCipherText::operator*=(const std::vector<double> &plain) {
    seal::Plaintext xPlain =
        mFactory->createPlainText(plain, mCiphertext->scale());
    return this->operator*=(xPlain);
}
SealCipherText &SealCipherText::operator*=(seal::Plaintext &plain) {
    mFactory->evaluator->mod_switch_to_inplace(plain, mCiphertext->parms_id());
    mFactory->evaluator->multiply_plain_inplace(*mCiphertext, plain);
    // When multiplying with plaintext, we don't need to relinearize
    mFactory->evaluator->rescale_to_next_inplace(*mCiphertext);
    // mFactory->fixScale(*this);
    return *this;
}

void SealCipherText::square() {
    mFactory->evaluator->square_inplace(*mCiphertext);
    mFactory->evaluator->relinearize_inplace(*mCiphertext,
                                             *mFactory->relin_keys);
    mFactory->evaluator->rescale_to_next_inplace(*mCiphertext);
    // mFactory->fixScale(*this);
}

void SealCipherText::power(uint p) {
    throw std::runtime_error("Not supported!");
}

bool SealCipherText::noiseNearOverflow() {
    // TODO find out a way to check if noise is near overflow
    return false;
}

void SealCipherText::writeToFile(const std::string &fileName) {
    throw std::runtime_error("Not supported!");
}

void SealCipherText::writeToFile(std::ostream &str) {
    throw std::runtime_error("Not supported!");
}

void SealCipherTextFactory::setAsDefaultFactory() {
    SealCipherText::defaultFactory = this;
}

SealCipherText SealCipherTextFactory::empty() {
    return SealCipherText(createRawEmpty(), this);
}

std::shared_ptr<seal::Ciphertext> SealCipherTextFactory::createRawEmpty() {
    seal::Ciphertext cipher;
    encryptor->encrypt(createPlainText(0.0, scale), cipher);
    return std::make_shared<seal::Ciphertext>(cipher);
}

SealCipherText
SealCipherTextFactory::createCipherText(const std::vector<double> &in) {
    seal::Plaintext plain;
    std::shared_ptr<seal::Ciphertext> cipher =
        std::make_shared<seal::Ciphertext>();
    encoder->encode(in, scale, plain);
    encryptor->encrypt(plain, *cipher);
    return SealCipherText(cipher, this);
}

SealCipherText SealCipherTextFactory::createCipherText(long in) {
    seal::Plaintext plain;
    std::shared_ptr<seal::Ciphertext> cipher =
        std::make_shared<seal::Ciphertext>();
    encoder->encode(static_cast<int64_t>(in), scale, plain);
    encryptor->encrypt(plain, *cipher);
    return SealCipherText(cipher, this);
}

SealCipherText
SealCipherTextFactory::createCipherText(const std::vector<long> &in) {
    std::vector<double> input;
    input.reserve(in.size());
    for (long x : in) {
        input.push_back(static_cast<double>(x));
    }
    return createCipherText(input);
}

SealCipherText
SealCipherTextFactory::createCipherText(const std::vector<float> &in) {
    std::vector<double> input;
    input.reserve(in.size());
    for (long x : in) {
        input.push_back(static_cast<double>(x));
    }
    return createCipherText(input);
}

seal::Plaintext SealCipherTextFactory::createPlainText(double x,
                                                       double ciphertextScale) {
    seal::Plaintext plain;
    encoder->encode(x, ciphertextScale, plain);
    return plain;
}

seal::Plaintext
SealCipherTextFactory::createPlainText(const std::vector<double> &x,
                                       double ciphertextScale) {
    seal::Plaintext plain;
    encoder->encode(x, ciphertextScale, plain);
    return plain;
}

std::vector<double>
SealCipherTextFactory::decryptDouble(const SealCipherText &ctx) {
    std::vector<double> result;
    seal::Plaintext plain;
    decryptor->decrypt(ctx.ctxt(), plain);
    encoder->decode(plain, result);
    return result;
}

std::vector<long>
SealCipherTextFactory::decryptLong(const SealCipherText &ctx) {
    throw std::runtime_error("Not supported!");
}

uint SealCipherTextFactory::batchsize() {
    return encoder->slot_count();
}

TensorP<SealCipherText> SealCipherTextFactory::createCipherTensor(
    const std::vector<double> &in, const Shape &shape,
    HETensorFactory<SealCipherText> *hetf) {
    throw std::runtime_error("Not supported!");
}

TensorP<SealCipherText> SealCipherTextFactory::createCipherTensor(
    const std::vector<float> &in, const Shape &shape,
    HETensorFactory<SealCipherText> *hetf) {
    throw std::runtime_error("Not supported!");
}

void SealCipherTextFactory::feedCipherTensor(const std::vector<double> &in,
                                             TensorP<SealCipherText> tensor,
                                             int batchSize) {
    throw std::runtime_error("Not supported!");
}
void SealCipherTextFactory::feedCipherTensor(const std::vector<float> &in,
                                             TensorP<SealCipherText> tensor,
                                             int batchSize) {
    throw std::runtime_error("Not supported!");
}

void SealCipherTextFactory::feedCipherTensor(const TensorP<double> in,
                                             TensorP<SealCipherText> tensor) {
    throw std::runtime_error("Not supported!");
}

void SealCipherTextFactory::feedCipherTensor(const TensorP<double> in,
                                             Tensor<SealCipherText> &tensor) {
    throw std::runtime_error("Not supported!");
}

void SealCipherTextFactory::bringToSameLevel(SealCipherText &c1,
                                             SealCipherText &c2) {
    size_t c1Level =
        context->get_context_data(c1.ctxt().parms_id())->chain_index();
    size_t c2Level =
        context->get_context_data(c2.ctxt().parms_id())->chain_index();
    if (c1Level > c2Level) {
        // Bring to same level & scale by multiplying with 1 (includes rescale)
        // Better than mod_switch_to_inplace from accuracy perspective
        // because it also refreshes scale (without we need fixScale method below).
        // Worse than mod_switch_to_inplace from performance perspective
        // because it doesn't need to multiply.
        for (int i = c1Level; i > c2Level; i--) {
            c1 *= 1.0;
        }
    } else if (c1Level < c2Level) {
        for (int i = c2Level; i > c1Level; i--) {
            c2 *= 1.0;
        }
    }
}

// REPLACED WITH SOLUTION IN bringToSameLevel
// Problem: when multiplying A * B, if both have scale 2^40 (for example),
// the result has scale 2^80. Then we do rescale, this divides scale by the last
// prime coeff modulus, which should be close to 2^40, but not exactly 2^40.
// In SEAL/native/examples/5_ckks_basics.cpp, they suggest 2 solutions:
// 1. Approximation: adjust the scale as if the prime number was exactly 2^40 (what we do here)
// 2. More exact: multiply by 1 to get to the same scale

// void SealCipherTextFactory::fixScale(SealCipherText &c) {
//     double ratio = c.ctxt().scale() / scale;
//     if (std::fabs(ratio - 1.0) >= 0.05) {
//         std::wcout << "The scale changed with more than 5%. Initial: " << scale
//                    << ", got: " << c.ctxt().scale() << std::endl;
//     }
//     c.ctxt().scale() = scale;
// }