#ifndef  BASE_FLOAT_VEC3_H
#define  BASE_FLOAT_VEC3_H

/****************************************************************
 * Include files
 ****************************************************************/

#include <stdexcept>
//using std::exception;
using std::domain_error;
//using std::invalid_argument;

/* Own includes */
#include "math_functions.h"
#include "base_int_vec3.h"

/****************************************************************
 * Class definition
 ****************************************************************/

template<typename T> 
struct base_float_vec3
{
public:
    T e[3];

    /* Constructors */
    base_float_vec3<T>();                    // Default constructor
    base_float_vec3<T>(const ivec3& source); // Conversion constructor
    base_float_vec3<T>(T e0, T e1, T e2);    // Other constructor

    base_float_vec3<T>& operator =(const ivec3&); // Conversion
    base_float_vec3<T>& operator+=(const base_float_vec3<T>&);
    base_float_vec3<T>& operator-=(const base_float_vec3<T>&);
    //base_float_vec3<T>& operator&=(const base_float_vec3<T>&); // Yet to define
    base_float_vec3<T>& operator*=(const T                  );
    base_float_vec3<T>& operator/=(const T                  );
    T&                  operator[](const int)      ;
    const T&            operator[](const int) const;

    base_float_vec3<T> operator-() const;

    base_float_vec3<T> operator+(const base_float_vec3<T>&) const;
    base_float_vec3<T> operator-(const base_float_vec3<T>&) const;
    T                  operator*(const base_float_vec3<T>&) const; // Scalar product
    base_float_vec3<T> operator&(const base_float_vec3<T>&) const; // Crossproduct
    base_float_vec3<T> operator*(const T                  ) const;
    base_float_vec3<T> operator/(const T                  ) const;

    bool              operator==(const base_float_vec3<T>&) const;

    T                  length                              () const;
    T                  sqr_length                          () const;
    void               normalize                           ()      ;
    base_float_vec3<T> normalized                          () const;
    base_float_vec3<T> random_equal_lenth_orthogonal_vector() const;
    base_float_vec3<T> random_normalized_orthogonal_vector () const;
};

typedef  base_float_vec3<float >  fvec3;
typedef  base_float_vec3<double>  dvec3;

/****************************************************************
 * Class related functions
 ****************************************************************/

template<typename T, typename T2>
base_float_vec3<T> operator* (const T2, const base_float_vec3<T>&);

/****************************************************************
 * Public functions
 ****************************************************************/

// Default constructor
template<typename T>
inline
base_float_vec3<T>::base_float_vec3()
{
    e[0] = e[1] = e[2] = 0.0;
}

template<typename T>
inline
base_float_vec3<T>::base_float_vec3(const ivec3& source)
{
    e[0] = T(source.e[0]);
    e[1] = T(source.e[1]);
    e[2] = T(source.e[2]);
}

template<typename T>
inline
base_float_vec3<T>::base_float_vec3(T e0, T e1, T e2)
{
    e[0] = e0;
    e[1] = e1;
    e[2] = e2;
}

template<typename T>
inline
base_float_vec3<T>& base_float_vec3<T>::operator=(const ivec3& rhs)
{
    for (int i = 0; i < 3; i++) e[i] = T(rhs.e[i]);
    return *this;
}

template<typename T>
inline
base_float_vec3<T>& base_float_vec3<T>::operator+=(const base_float_vec3<T>& rhs)
{
    for (int i = 0; i < 3; i++) e[i] += rhs.e[i];
    return *this;
}

template<typename T>
inline
base_float_vec3<T>& base_float_vec3<T>::operator-=(const base_float_vec3<T>& rhs)
{
    for (int i = 0; i < 3; i++) e[i] -= rhs.e[i];
    return *this;
}

template<typename T>
inline
base_float_vec3<T>& base_float_vec3<T>::operator*=(const T k)
{
    for (int i = 0; i < 3; i++) e[i] *= k;
    return *this;
}

template<typename T>
inline
base_float_vec3<T>& base_float_vec3<T>::operator/=(const T den)
{
#if  DEBUG
    if (!den) throw domain_error("Trying to divide a vector by zero");
#endif
    *this *= 1/den;
    return *this;
}

template<typename T>
inline
T& base_float_vec3<T>::operator[](const int i)
{
    return e[i];
}

template<typename T>
inline
const T& base_float_vec3<T>::operator[](const int i) const
{
    return e[i];
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::operator-() const
{
    return base_float_vec3<T>(-e[0],
                              -e[1],
                              -e[2]);
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::operator+(const base_float_vec3<T>& rhs) const
{
    return base_float_vec3<T>(e[0] + rhs.e[0],
                              e[1] + rhs.e[1],
                              e[2] + rhs.e[2]);
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::operator-(const base_float_vec3<T>& rhs) const
{
    return base_float_vec3<T>(e[0] - rhs.e[0],
                              e[1] - rhs.e[1],
                              e[2] - rhs.e[2]);
}

template<typename T>
inline
T base_float_vec3<T>::operator*(const base_float_vec3<T>& rhs) const
{
    return e[0]*rhs.e[0] + e[1]*rhs.e[1] + e[2]*rhs.e[2];
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::operator&(const base_float_vec3<T>& rhs) const
{
    return base_float_vec3<T>(e[1]*rhs.e[2] - e[2]*rhs.e[1],
                              e[2]*rhs.e[0] - e[0]*rhs.e[2],
                              e[0]*rhs.e[1] - e[1]*rhs.e[0]);
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::operator*(const T k) const
{
    return base_float_vec3<T>(e[0]*k, e[1]*k, e[2]*k);
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::operator/(const T den) const
{
#if  DEBUG
    if(!den) throw domain_error("Not defined to divide with zero");
#endif
    T k = 1/den;
    return base_float_vec3<T>(e[0]*k, e[1]*k, e[2]*k);
}

template<typename T>
inline
bool base_float_vec3<T>::operator==(const base_float_vec3<T>& rhs) const
{
    return e[0] == rhs[0] && e[1] == rhs[1] && e[2] == rhs[2];
}

template<typename T>
inline
T base_float_vec3<T>::length() const
{
    return sqrt(sqr_length());
}

template<typename T>
inline
T base_float_vec3<T>::sqr_length() const
{
    return (e[0]*e[0] + e[1]*e[1] + e[2]*e[2]);
}

template<typename T>
inline
void base_float_vec3<T>::normalize()
{
    T len = length();
#if  DEBUG
    if (!len) throw domain_error("Trying to normalize a zero-length base_float_vec3<T>");
#endif
    T k = 1/len;
    for (int i = 0; i < 3; i++) e[i] *= k;
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::normalized() const
{
    T len = length();
#if  DEBUG
    if (!len) throw domain_error("Trying to normalize a zero-length base_float_vec3<T>");
#endif
    T k = 1/len;
    return base_float_vec3<T>(e[0]*k, e[1]*k, e[2]*k);
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::random_equal_lenth_orthogonal_vector() const
{
#if  DEBUG
    if (!e[0] && !e[1] && !e[2]) throw domain_error("Trying to get an orthogonal vector to a zero-length base_float_vec3<T>");
#endif
    base_float_vec3<T> n = this->normalized();
    base_float_vec3<T> vec1 = n.e[0] ? base_float_vec3<T>(0, 1, 0) : base_float_vec3<T>(1, 0, 0);
    vec1 -= (n*vec1)*n;
    vec1 *= sqrt(this->sqr_length() / vec1.sqr_length());
    base_float_vec3<T> vec2 = n & vec1;
    double angle = uniform(0, M_2PI);
    return sin(angle)*vec1 + cos(angle)*vec2;
}

template<typename T>
inline
base_float_vec3<T> base_float_vec3<T>::random_normalized_orthogonal_vector() const
{
#if  DEBUG
    if (!e[0] && !e[1] && !e[2]) throw domain_error("Trying to get an orthogonal vector to a zero-length base_float_vec3<T>");
#endif
    base_float_vec3<T> n = this->normalized();
    base_float_vec3<T> vec1 = n.e[0] ? base_float_vec3<T>(0, 1, 0) : base_float_vec3<T>(1, 0, 0);
    vec1 -= (n*vec1)*n;
    vec1.normalize();
    base_float_vec3<T> vec2 = n & vec1;
    double angle = uniform(0, M_2PI);
    return sin(angle)*vec1 + cos(angle)*vec2;
}

/****************************************************************
 * Class related functions
 ****************************************************************/

template<typename T, typename T2>
inline
base_float_vec3<T> operator*(const T2 lhs, const base_float_vec3<T>& rhs)
{
    return base_float_vec3<T>(T(lhs)*rhs.e[0], T(lhs)*rhs.e[1], T(lhs)*rhs.e[2]);
}

#endif  /* BASE_FLOAT_VEC3_H */
