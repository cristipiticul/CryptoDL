#ifndef ARCHITECTURE_SEALBACKEND_SEALTENSOR_H_
#define ARCHITECTURE_SEALBACKEND_SEALTENSOR_H_

#include "../Tensor.h"

#include <seal/seal.h>

template <class ValueType = seal::Ciphertext>
class SealTensor : public Tensor<ValueType> {

public:
    SealTensor(Shape s) : Tensor<ValueType>::Tensor(s) {
    }

    static std::shared_ptr<Tensor<ValueType>> createTensor(Shape s) {
        return std::make_shared<SealTensor<ValueType>>(s);
    }
};

#endif /* ARCHITECTURE_SEALBACKEND_SEALTENSOR_H_ */