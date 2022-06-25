#include "TestCommons.h"

std::vector<double> createRandomVector(std::size_t size, double min,
                                       double max) {
    std::vector<double> plain;
    plain.reserve(size);
    for (std::size_t i = 0; i < size; i++) {
        plain.push_back(
            static_cast<double>(std::rand()) / RAND_MAX * (max - min) + min);
    }
    return plain;
}

template <>
bool compareOuput(std::vector<double> one, std::vector<double> two) {
    if (one.size() != two.size())
        return false;
    double max = one[0];
    for (unsigned int i = 0; i < one.size(); ++i) {
        if (std::abs(one[i]) > max) {
            max = std::abs(one[i]);
        }
        if (std::abs(two[i]) > max) {
            max = std::abs(two[i]);
        }
    }
    double epsilon = max * 0.001;
    for (unsigned int i = 0; i < one.size(); ++i)
        if (std::abs(one[i] - two[i]) > epsilon)
            return false;
    return true;
};