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
#if    NUM_DIMENSIONS == 3
#include "base_float_vec3.h"
#elif  NUM_DIMENSIONS == 2
#include "base_float_vec2.h"
#endif

#include "base_dllnode.h"


////////////////////////////////////////////////////////////////
// TYPEDEFS
////////////////////////////////////////////////////////////////

// TODO: Move to some more appropriate header?
#if    NUM_DIMENSIONS == 3
typedef base_int_vec3   <int  >  ivec ;
typedef base_float_vec3<pftype>  pfvec;
#elif  NUM_DIMENSIONS == 2
typedef base_int_vec2  <int   >  ivec ;
typedef base_float_vec2<pftype>  pfvec;
#endif

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

template<typename T>
class dllnode : public base_dllnode
{
public:
    // Constructors and destructor
    dllnode<T>(dllnode<T>* next_node);
    dllnode<T>(T value, dllnode<T>* next_node);
    ~dllnode<T>();

public:
    /* Public member variables */
    T v; // Value;

public:
    /* Public methods */
    dllnode<T>*   get_next_node              ();
    base_dllnode* get_previous_node          ();
    dllnode<T>*   remove_from_list_and_keep  ();
    void          remove_from_list_and_delete();

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
dllnode<T>::dllnode(dllnode<T>* next_node) :
    base_dllnode(next_node),
    v()
{
}

/* Constructor using the copy constructor of the type */
template<typename T>
dllnode<T>::dllnode(T value, dllnode<T>* next_node) :
    base_dllnode(next_node),
    v(value)
{
}

/* Destructor */
template<typename T>
dllnode<T>::~dllnode()
{
}

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

template<typename T>
inline
dllnode<T>* dllnode<T>::get_next_node()
{
    return static_cast<dllnode<T>*>(n);
}

template<typename T>
inline
base_dllnode* dllnode<T>::get_previous_node()
{
    return p;
}

template<typename T>
inline
dllnode<T>* dllnode<T>::remove_from_list_and_keep()
{
    // Update previous node (there is always one)
    p->n = n;

    // Update next node if there is one
    if (n) {
        n->p = p;
    }

    // Delete the node
    return this;
}

template<typename T>
inline
void dllnode<T>::remove_from_list_and_delete()
{
    delete remove_from_list_and_keep();
}

#endif // DLLNODE_H
