#ifndef NANINIT_H
#define NANINIT_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

#include <math.h>
#include <iostream>

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

template<typename T>
class naninit {
    T value;

public:
#ifdef  NAN
    naninit() : value(NAN) {} // The NAN macro is defined in math.h on machines that support IEEE floating point.
#else
    naninit() : value(sqrt(T(-1))) {} // I would optimise out the sqrt call in practice.
#endif
    naninit(T inVal) : value(inVal) {}

    operator T() const { return value; }

    naninit& operator += (const naninit &a) { return *this = value + a; }
    naninit& operator -= (const naninit &a) { return *this = value - a; }
    naninit& operator *= (const naninit &a) { return *this = value * a; }
    naninit& operator /= (const naninit &a) { return *this = value / a; }

    //template<typename T2>
    friend std::ostream & operator << (std::ostream  &os , const naninit &a) { return os  << a.value; }
    friend std::wostream& operator << (std::wostream &wos, const naninit &a) { return wos << a.value; }

    friend std::istream & operator >> (std::istream  &is , naninit &a) { return is  >> a.value; }
    friend std::wistream& operator >> (std::wistream &wis, naninit &a) { return wis >> a.value; }

    const naninit operator + () const { return *this; }
    const naninit operator - () const { return static_cast<naninit>(-value); }

    const naninit operator++ (int) {    // Post Increment operator
        naninit result = *this;
        ++*this;
        return result;
    }
    const naninit& operator++ ()    // Pre Increment operator
    { return *this = value+1; }
    const naninit operator-- (int) {    // Post Decrement operator
        naninit result = *this;
        --*this;
        return result;
    }
    const naninit& operator-- ()    // Pre Decrement operator
    { return *this = value-1; }
};

#endif  // NANINIT_H
