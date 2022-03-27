#include "SealTest.h"
#include "../src/architecture/HEBackend/helib/HELIbCipherText.h"

void compareHELibAndSealParameters() {
    std::cout << "Running " << __func__ << std::endl;
    // TODO what other things do we need to compare?
    // TODO: What is the "SCALE" in HELib? Maybe r=32? Should we also choose the primes bit size close to it?
    constexpr long L = 500;   // total bit count of prime coefficients
    constexpr long m = 65536; // chosen to match batch_size of SEAL
    constexpr long r = 32;    // ??

    HELibCipherTextFactory helibFactory(L, m, r);
    helib::Context &context = *helibFactory.context;
    std::cout << "HELib primes: ";
    for (long index : context.ctxtPrimes) {
        std::cout << std::round(std::log2(context.ithPrime(index))) << " ";
    }
    std::cout << std::endl;
    std::cout << "HELib Factory batch size: " << helibFactory.batchsize()
              << std::endl;

    size_t poly_modulus_degree =
        32768; // smallest poly_modulus_degree that allows L = 500 bits of total
    int number_of_coeffs =
        9; // used to match HELib (might be useful to use helib::buildModChain instead of hard-coding)
    int coeff_size = 56; // used to match HELib
    int scale_bits = 40; // TODO: What is the configuration in HELib?
    SealCipherTextFactory sealFactory(poly_modulus_degree, number_of_coeffs,
                                      coeff_size, scale_bits);
    std::cout << "Seal primes: ";
    for (const auto &prime :
         sealFactory.getContext().key_context_data()->parms().coeff_modulus()) {
        std::cout << prime.bit_count() << " ";
    }
    std::cout << std::endl;
    std::cout << "Seal Factory batch size: " << sealFactory.batchsize()
              << std::endl;
}

SealCipherTextFactory createFactory() {
    size_t poly_modulus_degree =
        32768; // smallest poly_modulus_degree that allows L = 500 bits of total
    int number_of_coeffs =
        9; // used to match HELib (might be useful to use helib::buildModChain instead of hard-coding)
    int coeff_size = 56; // used to match HELib
    int scale_bits = 40; // TODO: What is the configuration in HELib?
                         // TODO: coeff_size and scale_bits should be similar
    return SealCipherTextFactory(poly_modulus_degree, number_of_coeffs,
                                 coeff_size, scale_bits);
}

std::vector<double> createRandomVector(std::size_t size, double min = -100.0,
                                       double max = 100.0) {
    std::vector<double> plain;
    plain.reserve(size);
    for (std::size_t i = 0; i < size; i++) {
        plain.push_back(
            static_cast<double>(std::rand()) / RAND_MAX * (max - min) + min);
    }
    return plain;
}

bool sealTestEncryptDecrypt() {
    std::cout << "Running " << __func__ << std::endl;
    SealCipherTextFactory sealFactory = createFactory();
    std::vector<double> plain = createRandomVector(sealFactory.batchsize());
    SealCipherText encrypted = sealFactory.createCipherText(plain);
    std::vector<double> decrypted = sealFactory.decryptDouble(encrypted);
    return finishTest(decrypted, plain, __func__);
}

bool sealTestAdd2Ciphertexts() {
    std::cout << "Running " << __func__ << std::endl;
    SealCipherTextFactory sealFactory = createFactory();
    std::vector<double> v1 = createRandomVector(sealFactory.batchsize());
    std::vector<double> v2 = createRandomVector(sealFactory.batchsize());
    std::vector<double> sum;
    for (std::size_t i = 0; i < v1.size(); i++) {
        sum.push_back(v1[i] + v2[i]);
    }
    SealCipherText encrypted1 = sealFactory.createCipherText(v1);
    SealCipherText encrypted2 = sealFactory.createCipherText(v2);
    encrypted1 += encrypted2;
    std::vector<double> decrypted = sealFactory.decryptDouble(encrypted1);
    return finishTest(decrypted, sum, __func__);
}

bool sealTestMultiply2Ciphertexts() {
    std::cout << "Running " << __func__ << std::endl;
    SealCipherTextFactory sealFactory = createFactory();
    std::vector<double> v1 = createRandomVector(sealFactory.batchsize());
    std::vector<double> v2 = createRandomVector(sealFactory.batchsize());
    std::vector<double> sum;
    for (std::size_t i = 0; i < v1.size(); i++) {
        sum.push_back(v1[i] * v2[i]);
    }
    SealCipherText encrypted1 = sealFactory.createCipherText(v1);
    SealCipherText encrypted2 = sealFactory.createCipherText(v2);
    encrypted1 *= encrypted2;
    std::vector<double> decrypted = sealFactory.decryptDouble(encrypted1);
    return finishTest(decrypted, sum, __func__);
}

bool sealTestAddPlain() {
    std::cout << "Running " << __func__ << std::endl;
    SealCipherTextFactory sealFactory = createFactory();
    std::vector<double> v = createRandomVector(sealFactory.batchsize());

    SealCipherText encrypted = sealFactory.createCipherText(v);
    encrypted += 1.234;
    
    for (std::size_t i = 0; i < v.size(); i++) {
        v[i] = v[i] + 1.234;
    }
    std::vector<double> decrypted = sealFactory.decryptDouble(encrypted);
    return finishTest(decrypted, v, __func__);
}

bool sealTestMultiplyPlain() {
    std::cout << "Running " << __func__ << std::endl;
    SealCipherTextFactory sealFactory = createFactory();
    std::vector<double> v = createRandomVector(sealFactory.batchsize());
    
    SealCipherText encrypted = sealFactory.createCipherText(v);
    encrypted *= 1.234;
    
    for (std::size_t i = 0; i < v.size(); i++) {
        v[i] = v[i] * 1.234;
    }
    std::vector<double> decrypted = sealFactory.decryptDouble(encrypted);
    return finishTest(decrypted, v, __func__);
}

bool sealTestSquare() {
    std::cout << "Running " << __func__ << std::endl;
    SealCipherTextFactory sealFactory = createFactory();
    std::vector<double> v = createRandomVector(sealFactory.batchsize());
    SealCipherText encrypted = sealFactory.createCipherText(v);
    encrypted.square();
    for (std::size_t i = 0; i < v.size(); i++) {
        v[i] = v[i] * v[i];
    }
    std::vector<double> decrypted = sealFactory.decryptDouble(encrypted);
    return finishTest(decrypted, v, __func__);
}