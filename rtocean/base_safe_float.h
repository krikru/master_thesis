#ifndef BASE_SAFE_FLOAT_H
#define BASE_SAFE_FLOAT_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

/* Standard includes */
#if  1 || DEBUG
#include <math.h>
#endif


////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

template<typename T>
class base_safe_float
{
public:
    /* Constructors */
    base_safe_float(); // Default constructor
    template<typename T2> base_safe_float<T>(T2 s); // Copy conversion constructor

    /* Conversion operators */
    operator T() const;

#if 0
    /* Unary operators */
    base_safe_float<T> operator-() const;

    /* Binary operators */
    template<typename T2> base_safe_float<T> operator+(const T2&) const;
    template<typename T2> base_safe_float<T> operator-(const T2&) const;
    template<typename T2> base_safe_float<T> operator*(const T2&) const;
    template<typename T2> base_safe_float<T> operator/(const T2&) const;

    template<typename T2> bool               operator==(const T2&) const;
    template<typename T2> bool               operator!=(const T2&) const;
    template<typename T2> bool               operator< (const T2&) const;
    template<typename T2> bool               operator> (const T2&) const;
    template<typename T2> bool               operator<=(const T2&) const;
    template<typename T2> bool               operator>=(const T2&) const;
#endif

    /* Assignment operators */
    template<typename T2> base_safe_float<T>& operator =(const T2&);
    template<typename T2> base_safe_float<T>& operator+=(const T2&);
    template<typename T2> base_safe_float<T>& operator-=(const T2&);
    template<typename T2> base_safe_float<T>& operator*=(const T2&);
    template<typename T2> base_safe_float<T>& operator/=(const T2&);

private:
    T v; /* Value */
};

/****************************************************************
 * Class related functions declarations
 ****************************************************************/

#if 0
template<typename T, typename T2>
base_safe_float<T> operator+(const T2&, const base_safe_float<T>&);


template<typename T, typename T2>
base_safe_float<T> operator-(const T2&, const base_safe_float<T>&);


template<typename T, typename T2>
base_safe_float<T> operator*(const T2&, const base_safe_float<T>&);


template<typename T, typename T2>
base_safe_float<T> operator/(const T2&, const base_safe_float<T>&);
#endif

/****************************************************************
 * Public functions
 ****************************************************************/

/* Constructors */
// Default constructor
template<typename T>
inline
base_safe_float<T>::base_safe_float()
{
#if  DEBUG
#ifdef  NAN
    v = NAN;
#endif
#else
    /* Don't do anything */
#endif
}

/* Conversion operators */

template<typename T>
inline
base_safe_float<T>::operator T() const
{
    return v;
}

#if 0
/* Unary operators */
template<typename T>
inline
base_safe_float<T> base_safe_float<T>::operator-() const
{
    return -v;
}

/* Binary operators */
template<typename T>
template<typename T2>
inline
base_safe_float<T> base_safe_float<T>::operator+(const T2& ro) const
{
    return v + ro;
}

template<typename T>
template<typename T2>
inline
base_safe_float<T> base_safe_float<T>::operator-(const T2& ro) const
{
    return v - ro;
}

template<typename T>
template<typename T2>
inline
base_safe_float<T> base_safe_float<T>::operator*(const T2& ro) const
{
    return v * ro;
}

template<typename T>
template<typename T2>
inline
base_safe_float<T> base_safe_float<T>::operator/(const T2& ro) const
{
    return v / ro;
}

template<typename T>
template<typename T2>
inline
bool base_safe_float<T>::operator==(const T2& ro) const
{
    return v == ro;
}

template<typename T>
template<typename T2>
inline
bool base_safe_float<T>::operator!=(const T2& ro) const
{
    return v != ro;
}

template<typename T>
template<typename T2>
inline
bool base_safe_float<T>::operator< (const T2& ro) const
{
    return v < ro;
}

template<typename T>
template<typename T2>
inline
bool base_safe_float<T>::operator> (const T2& ro) const
{
    return v > ro;
}

template<typename T>
template<typename T2>
inline
bool base_safe_float<T>::operator<=(const T2& ro) const
{
    return v <= ro;
}

template<typename T>
template<typename T2>
inline
bool base_safe_float<T>::operator>=(const T2& ro) const
{
    return v >= ro;
}
#endif

/* Assignment operators */
template<typename T>
template<typename T2>
inline
base_safe_float<T>& base_safe_float<T>::operator =(const T2& rhs)
{
    v  = rhs;
    return *this;
}

template<typename T>
template<typename T2>
inline
base_safe_float<T>& base_safe_float<T>::operator+=(const T2& rhs)
{
    v += rhs;
    return *this;
}

template<typename T>
template<typename T2>
inline
base_safe_float<T>& base_safe_float<T>::operator-=(const T2& rhs)
{
    v -= rhs;
    return *this;
}

template<typename T>
template<typename T2>
inline
base_safe_float<T>& base_safe_float<T>::operator*=(const T2& rhs)
{
    v *= rhs;
    return *this;
}

template<typename T>
template<typename T2>
inline
base_safe_float<T>& base_safe_float<T>::operator/=(const T2& rhs)
{
    v /= rhs;
    return *this;
}

/****************************************************************
 * Class related functions definitions
 ****************************************************************/

#if 0
template<typename T, typename T2>
base_safe_float<T> operator+(const T2& lo, const base_safe_float<T>& ro)
{
    return lo + ro;
}

template<typename T, typename T2>
base_safe_float<T> operator-(const T2& lo, const base_safe_float<T>& ro)
{
    return lo - ro;
}

template<typename T, typename T2>
base_safe_float<T> operator*(const T2& lo, const base_safe_float<T>& ro)
{
    return lo * ro;
}

template<typename T, typename T2>
base_safe_float<T> operator/(const T2& lo, const base_safe_float<T>& ro)
{
    return lo / ro;
}
#endif

#endif // BASE_SAFE_FLOAT_H
