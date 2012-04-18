#ifndef DLLIST_H
#define DLLIST_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Own include files
#include "dllnode.h"

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

template<typename T>
class dllist
{
public:
    dllist<T>();
    ~dllist<T>();

private:
    /* Private member variables */
    dllnode<T> h; // Head

public:
    // Public methods
    dllnode<T>* add_new_element();
    dllnode<T>* add_existing_element(T element);
    dllnode<T>* get_first_element();
    //void remove_node(dllnode<T>* node);

private:
    /*************************
     * Disabled constructors *
     *************************/
    dllist<T>(dllist<T>&); // Copy constructor prevented from all use
};

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTUR
////////////////////////////////////////////////////////////////

/* Default constructor */
template<typename T>
dllist<T>::dllist() :
    h(0)
{
}

/* Destructor */
template<typename T>
dllist<T>::~dllist()
{
    dllnode<T>* current_node;
    dllnode<T>* next_node;
    // TODO: Optimize this loop
    for (current_node = h.n; current_node; current_node = next_node) {
        next_node = current_node->n;
        delete current_node;
    }
}

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

/* Adds element to the beginig of the list*/
template<typename T>
dllnode<T>* dllist<T>::add_new_element()
{
    dllnode<T>* node = new dllnode<T>(h.n);
    if (h.n) {
        h.n->p = node;
    }
    h.n = node;
    node->p = &h;
    return node;
}

template<typename T>
dllnode<T>* dllist<T>::add_existing_element(T element)
{
    dllnode<T>* node = new dllnode<T>(element, h.n);
    if (h.n) {
        h.n->p = node;
    }
    h.n = node;
    node->p = &h;
    return node;
}

template<typename T>
inline
dllnode<T>* dllist<T>::get_first_element()
{
    return h.n;
}

#endif // DLLIST_H
