#ifndef TEST_SEALTEST_H_
#define TEST_SEALTEST_H_

#include "../src/architecture/HEBackend/seal/SealCipherText.h"
#include "../src/tools/IOStream.h"
#include "TestCommons.h"

void compareHELibAndSealParameters();
bool sealTestEncryptDecrypt();
bool sealTestAdd2Ciphertexts();
bool sealTestMultiply2Ciphertexts();
bool sealTestAddPlain();
bool sealTestMultiplyPlain();
bool sealTestSquare();

#endif /* TEST_SEALTEST_H_ */