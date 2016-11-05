#ifndef ESFT_H_INCLUDED
#define ESFT_H_INCLUDED

#include "smart_ptr.h"

namespace tuz
{

template<typename T>
shared_ptr<T> enable_shared_from_this<T>::shared_from_this() const
{
    return shared_ptr<T>(wp_helper);
}

}


#endif // ESFT_H_INCLUDED
