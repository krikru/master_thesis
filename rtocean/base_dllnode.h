#ifndef BASE_DLLNODE_H
#define BASE_DLLNODE_H

class base_dllnode
{
public:
    base_dllnode(base_dllnode* next_node);

public:
    /* public member variables */
    base_dllnode* n; // Next node
    base_dllnode* p; // Previous node

private:
    /*************************
     * Disabled constructors *
     *************************/
    base_dllnode(); // Default constructor prevented from all use
    base_dllnode(base_dllnode&); // Copy constructor prevented from all use
};

#endif // BASE_DLLNODE_H
