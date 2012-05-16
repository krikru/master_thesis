#ifndef  BASE_INT_VEC2_H
#define  BASE_INT_VEC2_H

/****************************************************************
 * Include files
 ****************************************************************/

#include <stdexcept>
//using std::exception;
using std::domain_error;
using std::out_of_range;
//using std::invalid_argument;

/****************************************************************
 * Class definition
 ****************************************************************/

template<typename T>
struct base_int_vec2
{
public:
    T e[2];

    /* Constructors */
    base_int_vec2<T>();                 // Default constructor
    base_int_vec2<T>(T e0, T e1); // Other constructor

    base_int_vec2<T>& operator+=(const base_int_vec2<T>&);
    base_int_vec2<T>& operator-=(const base_int_vec2<T>&);
    base_int_vec2<T>& operator*=(const T                );
    base_int_vec2<T>& operator/=(const T                );
    T&                operator[](const int)      ;
    const T&          operator[](const int) const;

    base_int_vec2<T> operator-() const;

    base_int_vec2<T> operator+(const base_int_vec2<T>&) const;
    base_int_vec2<T> operator-(const base_int_vec2<T>&) const;
    T                operator*(const base_int_vec2<T>&) const; // Scalar product
    base_int_vec2<T> operator*(const T                ) const;
    base_int_vec2<T> operator/(const T                ) const;

    T sqr_length() const;
};

typedef  base_int_vec2<int>  ivec2;

/****************************************************************
 * Class related functions
 ****************************************************************/

template<typename T>
base_int_vec2<T> operator* (const T&, const base_int_vec2<T>&);

/****************************************************************
 * Public functions
 ****************************************************************/

// Default constructor
template<typename T>
inline
base_int_vec2<T>::base_int_vec2()
{
    e[0] = e[1] = 0;
}

template<typename T>
inline
base_int_vec2<T>::base_int_vec2(T e0, T e1)
{
    e[0] = e0;
    e[1] = e1;
}

template<typename T>
inline
base_int_vec2<T>& base_int_vec2<T>::operator+=(const base_int_vec2<T>& rhs)
{
    for (T i = 0; i < 2; i++) e[i] += rhs.e[i];
    return *this;
}

template<typename T>
inline
base_int_vec2<T>& base_int_vec2<T>::operator-=(const base_int_vec2<T>& rhs)
{
    for (T i = 0; i < 2; i++) e[i] -= rhs.e[i];
    return *this;
}

template<typename T>
inline
base_int_vec2<T>& base_int_vec2<T>::operator*=(const T k)
{
    for (T i = 0; i < 2; i++) e[i] *= k;
    return *this;
}

template<typename T>
inline
base_int_vec2<T>& base_int_vec2<T>::operator/=(const T den)
{
#if  DEBUG
    if (!den) throw domain_error("Trying to divide a vector by zero");
#endif
    for (T i = 0; i < 2; i++) e[i] /= den;
    return *this;
}

template<typename T>
inline
T& base_int_vec2<T>::operator[](const int i)
{
#if  DEBUG
    if (i < 0) {
        throw out_of_range("Trying to access base_int_vec2 element with negative index");
    }
    else if (i >= 2) {
        throw out_of_range("Trying to access base_int_vec2 element with to high index");
    }
#endif
    return e[i];
}

template<typename T>
inline
const T& base_int_vec2<T>::operator[](const int i) const
{
#if  DEBUG
    if (i < 0) {
        throw out_of_range("Trying to access base_int_vec2 element with negative index");
    }
    else if (i >= 2) {
        throw out_of_range("Trying to access base_int_vec2 element with to high index");
    }
#endif
    return e[i];
}

template<typename T>
inline
base_int_vec2<T> base_int_vec2<T>::operator-() const
{
    return base_int_vec2<T>(-e[0],
                            -e[1]);
}

template<typename T>
inline
base_int_vec2<T> base_int_vec2<T>::operator+(const base_int_vec2<T>& rhs) const
{
    return base_int_vec2<T>(e[0] + rhs.e[0],
                            e[1] + rhs.e[1]);
}

template<typename T>
inline
base_int_vec2<T> base_int_vec2<T>::operator-(const base_int_vec2<T>& rhs) const
{
    return base_int_vec2<T>(e[0] - rhs.e[0],
                            e[1] - rhs.e[1]);
}

template<typename T>
inline
T base_int_vec2<T>::operator*(const base_int_vec2<T>& rhs) const
{
    return e[0]*rhs.e[0] + e[1]*rhs.e[1];
}

template<typename T>
inline
base_int_vec2<T> base_int_vec2<T>::operator*(const T k) const
{
    return base_int_vec2<T>(e[0]*k, e[1]*k);
}

template<typename T>
inline
base_int_vec2<T> base_int_vec2<T>::operator/(const T den) const
{
#if  DEBUG
    if(!den) throw domain_error("Not defined to divide with zero");
#endif
    return base_int_vec2<T>(e[0]/den, e[1]/den);
}

template<typename T>
inline
T base_int_vec2<T>::sqr_length() const
{
    return e[0]*e[0] + e[1]*e[1];
}

/****************************************************************
 * Class related functions
 ****************************************************************/

template<typename T>
inline
base_int_vec2<T> operator*(const T& lhs, const base_int_vec2<T>& rhs)
{
    return base_int_vec2<T>(lhs*rhs.e[0], lhs*rhs.e[1]);
}

#endif  /* BASE_INT_VEC2_H */
