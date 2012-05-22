#include "mustinit.h"

template<>
void mustinit<float>::declare_reading() const
{
    if (!initialized) {
        throw std::runtime_error("Reading from an uninitialized object");
    }
}

template<>
void mustinit<double>::declare_reading() const
{
    if (!initialized) {
        throw std::runtime_error("Reading from an uninitialized object");
    }
}
