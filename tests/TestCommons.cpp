#include "TestCommons.h"

// TODO: find good EPSILON value
constexpr double EPSILON = 0.00001;

template <>
bool compareOuput(std::vector<double> one, std::vector<double> two) {
    if (one.size() != two.size())
        return false;
    for (unsigned int i = 0; i < one.size(); ++i)
        if (std::abs(one[i] - two[i]) > EPSILON)
            return false;
    return true;
};