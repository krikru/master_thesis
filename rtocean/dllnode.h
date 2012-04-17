#ifndef DLLNODE_H
#define DLLNODE_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <iostream>
using std::cout;
using std::endl;

// Own includes
#include "definitions.h"
#include "base_float_vec3.h"

////////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////////

typedef base_float_vec3<pftype>  pfvec3;

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

template<typename T>
class dllnode
{
public:
    // Constructors and destructor
    dllnode<T>(dllnode<T>* next_node);
    dllnode<T>(T value, dllnode<T>* next_node);
    ~dllnode<T>();

public:
    T           v; // Value;
    dllnode<T>* n; // Next node
    dllnode<T>* p; // Previous node

private:
    /*************************
     * Disabled constructors *
     *************************/
    dllnode<T>(); // Default constructor prevented from all use
    dllnode<T>(dllnode<T>&); // Copy constructor prevented from all use
};

/*
 * Forward declarations
 *   Must define all type specific functions in a source file
 * and declare them in the header file
 */
template<> dllnode<int>::~dllnode();

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTUR
////////////////////////////////////////////////////////////////

/* Constructor using the default constructor of the type */
template<typename T>
dllnode<T>::dllnode(dllnode<T>* next_node)
{
    n = next_node;
    p = 0;
}

/* Constructor using the copy constructor of the type */
template<typename T>
dllnode<T>::dllnode(T value, dllnode<T>* next_node) :
    v(value)
{
    n = next_node;
    p = 0;
}

/* Destructor */
template<typename T>
dllnode<T>::~dllnode()
{
}

#endif // DLLNODE_H
