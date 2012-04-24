#ifndef  CALLBACK_H
#define  CALLBACK_H

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

template<typename T>
class callback
{
public:
    /* Constructors */
    callback();
    callback(T function, void* parameter);

public:
    /* Public member variables */
    T     func;
    void* param;

public:
    /* Public methods */
    bool is_defined();
    bool has_parameter();
};

////////////////////////////////////////////////////////////////
// CONSTRUCTORS
////////////////////////////////////////////////////////////////

// Constructor
template<typename T>
inline
callback<T>::callback()
{
    func  = 0;
    param = 0;
}

template<typename T>
inline
callback<T>::callback(T function, void* parameter)
{
    func  = function ;
    param = parameter;
}

////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////

template<typename T>
inline
bool callback<T>::is_defined()
{
    return func;
}

template<typename T>
inline
bool callback<T>::has_parameter()
{
    return param;
}

#endif  /* CALLBACK_H */
