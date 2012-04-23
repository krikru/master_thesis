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
    base_dllnode h; // Head

public:
    // Public methods
    dllnode<T>* add_new_element();
#if 0
    dllnode<T>* add_existing_element(T element);
#endif
    dllnode<T>* add_existing_node(dllnode<T>* node);
    dllnode<T>* get_first_node();
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
    for (current_node = get_first_node(); current_node; current_node = next_node) {
        next_node = current_node->get_next_node();
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
    /* Create new empty node and assign correct pointer values */
    dllnode<T>* node = new dllnode<T>(get_first_node());
    node->p = &h;
    /* Update pointers in nodes already existing in list */
    if (h.n) {
        h.n->p = node;
    }
    h.n = node;
    return node;
}

#if 0
template<typename T>
dllnode<T>* dllist<T>::add_existing_element(T element)
{
    /* Create new node and assign correct pointer values */
    dllnode<T>* node = new dllnode<T>(element, get_first_element());
    node->p = &h;
    /* Update pointers in nodes already existing in list */
    if (h.n) {
        h.n->p = node;
    }
    h.n = node;
    return node;
}
#endif

template<typename T>
dllnode<T>* dllist<T>::add_existing_node(dllnode<T>* node)
{
    /* Update pointers in node */
    node->n = get_first_node();
    node->p = &h;
    /* Update pointers in nodes already existing in list */
    if (h.n) {
        h.n->p = node;
    }
    h.n = node;
    return node;
}

template<typename T>
inline
dllnode<T>* dllist<T>::get_first_node()
{
    return static_cast<dllnode<T>*>(h.n);
}

#endif // DLLIST_H
