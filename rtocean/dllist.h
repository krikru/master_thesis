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

public:
    // TODO: Optimization: make h a real ghost node instead of the pointer to a real node (?)
    dllnode<T>* h; // Head

public:
    // Public methods
    dllnode<T>* add_new_element();
    dllnode<T>* add_existing_element(T element);
    void remove_node(dllnode<T>* node);

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
dllist<T>::dllist()
{
    h = 0;
}

/* Destructor */
template<typename T>
dllist<T>::~dllist()
{
    dllnode<T>* current_node;
    dllnode<T>* next_node;
    for (current_node = h; current_node; current_node = next_node) {
        next_node = current_node->n;
        delete current_node;
    }
}

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

template<typename T>
dllnode<T>* dllist<T>::add_new_element()
{
    dllnode<T>* node = new dllnode<T>(h);
    if (h) {
        h->p = node;
    }
    h = node;
    return node;
}

template<typename T>
dllnode<T>* dllist<T>::add_existing_element(T element)
{
    dllnode<T>* node = new dllnode<T>(element, h);
    if (h) {
        h->p = node;
    }
    h = node;
    return node;
}

template<typename T>
void dllist<T>::remove_node(dllnode<T>* node)
{
    // Update previous node or change list head
    if (node->p) {
        // There is at least one node before this node in the list
        node->p->n = node->n;
    }
    else {
        // The node is the head of the list
        h = node->n;
    }

    // Update next node if there is one
    if (node->n) {
        node->n->p = node->p;
    }

    // Delete the node
    delete node;
}

#endif // DLLIST_H
