#ifndef SW_BASE_H_INCLUDED
#define SW_BASE_H_INCLUDED

#include "proxy.h"

namespace tuz
{

template<typename T>
class SW_base
{
protected:
    Proxy_base<T>* proxy;

    void swap(SW_base<T>& swb) noexcept;

public:
    size_t use_count() const noexcept;

#ifdef DEBUG
    const Proxy_base<T>* DEBUG_get_proxy_addr() const noexcept
    {
        return proxy;
    };
#endif
};

template<typename T>
size_t SW_base<T>::use_count() const noexcept
{
    return proxy->links_count();
}

template<typename T>
void SW_base<T>::swap(SW_base<T>& swb) noexcept
{
    std::swap(proxy, swb.proxy);
}

}

#endif // SW_BASE_H_INCLUDED
