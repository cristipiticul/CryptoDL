#include "SealCipherText.h"
#include <seal/seal.h>

SealCipherTextFactory* SealCipherText::defaultFactory = nullptr;

void SealCipherTextFactory::setAsDefaultFactory() {
    SealCipherText::defaultFactory = this;
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
    throw std::runtime_error("Not supported!");
    // std::vector<double> input;
    // input.reserve(in.size());
    // for (long x : in) {
    //     input.push_back(static_cast<double>(x));
    // }
    // return createCipherText(input);
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