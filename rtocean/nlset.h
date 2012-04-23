#ifndef NLSET_H
#define NLSET_H

////////////////////////////////////////////////////////////////
// INCLUDE FILES
////////////////////////////////////////////////////////////////

// Standard includes
#include <stdexcept>
//using std::exception;
using std::logic_error;
using std::out_of_range;

// Own includes
#include "octneighbor.h"

////////////////////////////////////////////////////////////////
// ENUMS
////////////////////////////////////////////////////////////////

enum NEIGHBOR_LISTS_ENUM {
    NL_LOWER_LEVEL_OF_DETAIL_LEAF    ,
    NL_LOWER_LEVEL_OF_DETAIL_NON_LEAF,
    NL_SAME_LEVEL_OF_DETAIL_LEAF     ,
    NL_SAME_LEVEL_OF_DETAIL_NON_LEAF ,
    NL_HIGHER_LEVEL_OF_DETAIL        ,
    NUM_NEIGHBOR_LISTS
};

//#define  NUM_NEIGHBOR_LISTS  0

////////////////////////////////////////////////////////////////
// CLASS DEFINITION
////////////////////////////////////////////////////////////////

class nlset
{
public:
    nlset();

public:
    /* Public methods */
    void add_neighbor_list(nlist* list);
    nlnode* get_first_node();
    nlnode* get_next_node();

private:
    /* Private member variables */
    int num_unread_lists; /* The number of lists left to take elements from */
    nlnode* next_node; /* The next node to return */
    nlist* unread_lists[NUM_NEIGHBOR_LISTS]; /* The lists yet to read (from back to front in this array) */

private:
    /* Private methods */
    nlist* get_next_unread_list();
    void find_next_node();
};

////////////////////////////////////////////////////////////////
// CONSTRUCTORS AND DESTRUCTOR
////////////////////////////////////////////////////////////////

inline
nlset::nlset()
{
    num_unread_lists  = 0;
#if  DEBUG
    next_node         = 0;
#endif
}

////////////////////////////////////////////////////////////////
// PUBLIC METHODS
////////////////////////////////////////////////////////////////

inline
void nlset::add_neighbor_list(nlist* list)
{
#if  DEBUG
    if (num_unread_lists >= NUM_NEIGHBOR_LISTS) {
        throw out_of_range("Have already added maximum number of neighbor lists to set");
    }
    for (int i = 0; i < num_unread_lists; i++) {
        if (unread_lists[i] == list) {
            throw logic_error("List is already in set");
        }
    }
    if (next_node) {
        throw logic_error("Have already started to loop through list elements");
    }
#endif
    unread_lists[num_unread_lists] = list;
    num_unread_lists++;
}

inline
nlnode* nlset::get_first_node()
{
#if  DEBUG
    if (next_node) {
        throw logic_error("Have already started to loop through list elements");
    }
#endif
    next_node = 0;
    if (num_unread_lists) {
        find_next_node();
        nlnode* ret = next_node;
        find_next_node();
        return ret;
    }
    return 0;
}

inline
nlnode* nlset::get_next_node()
{
    if (next_node) {
        nlnode* ret = next_node;
        find_next_node();
        return ret;
    }
    return 0;
}

inline
nlist* nlset::get_next_unread_list()
{
    if (!num_unread_lists) {
        return 0;
    }
    num_unread_lists--;
    return unread_lists[num_unread_lists];
}

inline
void nlset::find_next_node()
{
    if (next_node) {
        next_node = next_node->get_next_node();
    }
    while (!next_node) {
        nlist* next_list = get_next_unread_list();
        if (!next_list) {
            /* No more list to read from */
            return;
        }
        next_node = next_list->get_first_node();
    }
}

#endif // NLSET_H
