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
#if    NUM_DIRECTIONS == 3
#include "base_float_vec3.h"
#elif  NUM_DIRECTIONS == 2
#include "base_float_vec2.h"
#endif


////////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////////

#if    NUM_DIRECTIONS == 3
typedef base_float_vec3<pftype>  pfvec;
#elif  NUM_DIRECTIONS == 2
typedef base_float_vec2<pftype>  pfvec;
#endif

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
    /* Public member variables */
    T           v; // Value;
    dllnode<T>* n; // Next node
    dllnode<T>* p; // Previous node

public:
    /* Public methods */
    void remove();

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

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTUR
////////////////////////////////////////////////////////////////

template<typename T>
void dllnode<T>::remove()
{
    // Update previous node (there is always one)
    p->n = n;

    // Update next node if there is one
    if (n) {
        n->p = p;
    }

    // Delete the node
    delete this;
}

#endif // DLLNODE_H
