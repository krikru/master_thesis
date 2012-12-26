#ifndef MUSTINIT_H
#define MUSTINIT_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

#include <stdexcept>
#include <iostream>

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

template<typename T>
class mustinit {
    T value;
    bool initialized;

public:
    mustinit(   ) :           initialized(false) {} // Default constructir
    mustinit(T a) : value(a), initialized(true ) {} // Copy constructor

    operator T() const { declare_reading(); return value; }

    mustinit& operator  = (const T        &a) { value = a; initialized = true; return *this;}
    mustinit& operator += (const mustinit &a) { declare_reading(); return *this = value + a; }
    mustinit& operator -= (const mustinit &a) { declare_reading(); return *this = value - a; }
    mustinit& operator *= (const mustinit &a) { declare_reading(); return *this = value * a; }
    mustinit& operator /= (const mustinit &a) { declare_reading(); return *this = value / a; }

    //template<typename T2>
    //friend std::ostream & operator << (std::ostream  &os , const mustinit &a) { a.declare_reading(); return os  << a.value; }
    //friend std::wostream& operator << (std::wostream &wos, const mustinit &a) { a.declare_reading(); return wos << a.value; }

    //friend std::istream & operator >> (std::istream  &is , mustinit &a) { return is  >> a.value; }
    //friend std::wistream& operator >> (std::wistream &wis, mustinit &a) { return wis >> a.value; }

    //const mustinit operator + () const { declare_reading(); return *this; }
    //const mustinit operator - () const { declare_reading(); return static_cast<mustinit>(-value); }

    const mustinit operator++ (int) {    // Post Increment operator
        declare_reading();
        mustinit result = *this;
        ++*this;
        return result;
    }
    const mustinit& operator++ ()    // Pre Increment operator
    { declare_reading(); return *this = value+1; }
    const mustinit operator-- (int) {    // Post Decrement operator
        declare_reading();
        mustinit result = *this;
        --*this;
        return result;
    }
    const mustinit& operator-- ()    // Pre Decrement operator
    { declare_reading(); return *this = value-1; }

private:
    void declare_reading() const;
};

/*
 * Forward declarations
 *   Must define all type specific functions in a source file
 * and declare them in the header file
 */
template<> void mustinit<float >::declare_reading() const;
template<> void mustinit<double>::declare_reading() const;

template<typename T>
void mustinit<T>::declare_reading() const
{
    if (!initialized) {
        throw std::runtime_error("Reading from an uninitialized object");
    }
}

#endif  // MUSTINIT_H
