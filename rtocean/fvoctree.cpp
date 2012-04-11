#include "fvoctree.h"

fvoctree::fvoctree()
{
    root = 0;
}

fvoctree::~fvoctree()
{
    if (root) {
        delete root;
    }
}

