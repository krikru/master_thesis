#include "dllnode.h"

template<>
dllnode<int>::~dllnode()
{
#if DEBUG
    cout << "Deleted node with value " << v << endl;
#endif
}
