#include "dllnode.h"

template<>
dllnode<int>::~dllnode()
{
    cout << "Deleted node with value " << v << endl;
}
