#ifndef ARCHITECTURE_SEALBACKEND_SEAL_SEALCIPHERTEXT_H_
#define ARCHITECTURE_SEALBACKEND_SEAL_SEALCIPHERTEXT_H_

#include "../CipherTextWrapper.h"
#include "../HETensor.h"
#include <seal/seal.h>

class SealCipherTextFactory;

class SealCipherText {
public:
    static SealCipherTextFactory *defaultFactory; // declaration in cpp

    friend SealCipherTextFactory;

    SealCipherText();

    SealCipherText(std::shared_ptr<seal::Ciphertext> cipherText,
                   SealCipherTextFactory *factory)
        : mFactory(factory), mCiphertext(cipherText) {
    }

    seal::Ciphertext &ctxt() const {
        return *mCiphertext;
    }

    virtual SealCipherText &operator+=(SealCipherText &other);
    virtual SealCipherText &operator*=(SealCipherText &other);
    virtual SealCipherText &operator+=(const std::vector<double> &plain);
    virtual SealCipherText &operator*=(const std::vector<double> &plain);
    virtual SealCipherText &operator+=(SealCipherText *other);
    virtual SealCipherText &operator*=(SealCipherText *other);
    virtual SealCipherText &operator=(const SealCipherText &other);

    virtual SealCipherText &operator+=(double x);
    virtual SealCipherText &operator*=(double x);

    void square();
    void power(uint p);

    bool noiseNearOverflow();
    void writeToFile(const std::string &fileName);
    void writeToFile(std::ostream &str);

private:
    virtual SealCipherText &operator+=(seal::Plaintext &ptxt);
    virtual SealCipherText &operator*=(seal::Plaintext &ptxt);

    SealCipherTextFactory *mFactory;
    std::shared_ptr<seal::Ciphertext> mCiphertext;
};

class SealCipherTextFactory : public CipherTextWrapperFactory<SealCipherText> {
public:
    friend SealCipherText;

    SealCipherTextFactory(size_t poly_modulus_degree, int number_of_coeffs,
                          int coeff_size, int scale_bits) {
        std::vector<int> coeffSizes;
        for (int i = 0; i < number_of_coeffs; i++) {
            coeffSizes.push_back(coeff_size);
        }
        init(poly_modulus_degree, coeffSizes, scale_bits);
    }

    SealCipherTextFactory(size_t poly_modulus_degree,
                          std::vector<int> coeffSizes, int scale_bits) {
        init(poly_modulus_degree, coeffSizes, scale_bits);
    }

    void init(size_t poly_modulus_degree, std::vector<int> coeffSizes,
              int scale_bits) {
        seal::EncryptionParameters parms(seal::scheme_type::ckks);
        parms.set_poly_modulus_degree(poly_modulus_degree);

        parms.set_coeff_modulus(
            seal::CoeffModulus::Create(poly_modulus_degree, coeffSizes));
        scale = pow(2.0, scale_bits);
        context = std::make_shared<seal::SEALContext>(parms);
        seal::KeyGenerator keygen(*context);
        auto secret_key = keygen.secret_key();
        seal::PublicKey public_key;
        keygen.create_public_key(public_key);
        seal::RelinKeys relin_keys_temp;
        keygen.create_relin_keys(relin_keys_temp);
        relin_keys = std::make_shared<seal::RelinKeys>(relin_keys_temp);
        // seal::GaloisKeys gal_keys;
        // keygen.create_galois_keys(gal_keys);
        encryptor = std::make_shared<seal::Encryptor>(*context, public_key);
        evaluator = std::make_shared<seal::Evaluator>(*context);
        decryptor = std::make_shared<seal::Decryptor>(*context, secret_key);
        encoder = std::make_shared<seal::CKKSEncoder>(*context);
    }

    seal::SEALContext &getContext() {
        return *context;
    }

    virtual void setAsDefaultFactory() override;

    virtual std::shared_ptr<seal::Ciphertext> createRawEmpty();
    virtual SealCipherText empty() override;

    virtual SealCipherText createCipherText(long x);
    virtual SealCipherText createCipherText(const std::vector<long> &in);
    virtual SealCipherText createCipherText(const std::vector<double> &in);
    virtual SealCipherText createCipherText(const std::vector<float> &in);
    virtual std::vector<long> decryptLong(const SealCipherText &ctx);
    virtual std::vector<double> decryptDouble(const SealCipherText &ctx);
    virtual uint batchsize();

    /**
	 * @brief Take a 1D vector transform into the given shape and encrypt it. The 1st dimension of the same needs to line up with the batchSize
	 * supported by the encryption scheme.
	 */
    virtual TensorP<SealCipherText>
    createCipherTensor(const std::vector<double> &in, const Shape &shape,
                       HETensorFactory<SealCipherText> *hetf);

    virtual TensorP<SealCipherText>
    createCipherTensor(const std::vector<float> &in, const Shape &shape,
                       HETensorFactory<SealCipherText> *hetf);

    /**
	 * Encrypts the data and sticks it into the ciphertensor.
	 * If batchSize == -1 the batch size is infered from the crypto parameters otherwise it
	 * must not be larger than the batch size infered from the crypto parameters. 
	 */
    virtual void feedCipherTensor(const std::vector<double> &in,
                                  TensorP<SealCipherText> tensor,
                                  int batchSize = -1);

    /**
	 * Encrypts the data and sticks it into the ciphertensor.
	 * If batchSize == -1 the batch size is infered from the crypto parameters otherwise it
	 * must not be larger than the batch size infered from the crypto parameters. 
	 */
    virtual void feedCipherTensor(const std::vector<float> &in,
                                  TensorP<SealCipherText> tensor,
                                  int batchSize = -1);

    virtual void feedCipherTensor(const TensorP<double> in,
                                  TensorP<SealCipherText> tensor);

    virtual void feedCipherTensor(const TensorP<double> in,
                                  Tensor<SealCipherText> &tensor);

    virtual void bringToSameLevel(SealCipherText &c1, SealCipherText &c2);

    // virtual void fixScale(SealCipherText &c);

private:
    virtual seal::Plaintext createPlainText(double x, double ciphertextScale);
    virtual seal::Plaintext createPlainText(const std::vector<double> &x,
                                            double ciphertextScale);

    double scale;
    std::shared_ptr<seal::SEALContext> context;
    std::shared_ptr<seal::Encryptor> encryptor;
    std::shared_ptr<seal::Evaluator> evaluator;
    std::shared_ptr<seal::Decryptor> decryptor;
    std::shared_ptr<seal::CKKSEncoder> encoder;
    std::shared_ptr<seal::RelinKeys> relin_keys;
};

#endif /* ARCHITECTURE_SEALBACKEND_SEAL_SEALCIPHERTEXT_H_ */
