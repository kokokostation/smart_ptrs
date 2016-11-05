#ifndef EXCEPTION_H_INCLUDED
#define EXCEPTION_H_INCLUDED

#include <stdexcept>

namespace tuz
{

class bad_weak_ptr : std::exception
{
};

}

#endif // EXCEPTION_H_INCLUDED
