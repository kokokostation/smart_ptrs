#ifndef SW_BASE_H_INCLUDED
#define SW_BASE_H_INCLUDED

#include "proxy.h"

namespace tuz
{

template<typename T, typename P>
class SW_base
{
protected:
    Proxy_base<T>* proxy;

    void swap(SW_base<T, P>& swb) noexcept;
    void check_out() noexcept;
    void check_in() noexcept;
    void set_proxy(Proxy_base<T>* another_proxy = Proxy_dummy<T>::instance_ptr()) noexcept;

public:
    size_t use_count() const noexcept;

#ifdef DEBUG
    const Proxy_base<T>* DEBUG_get_proxy_addr() const noexcept
    {
        return proxy;
    };
#endif
};

template<typename T, typename P>
void SW_base<T, P>::set_proxy(Proxy_base<T>* another_proxy) noexcept
{
    proxy = another_proxy;
    check_in();
}

template<typename T, typename P>
void SW_base<T, P>::check_out() noexcept
{
    proxy->check_out(static_cast<const P&>(*this));
    if(proxy->need_deletion())
        delete proxy;
}

template<typename T, typename P>
void SW_base<T, P>::check_in() noexcept
{
    proxy->check_in(static_cast<const P&>(*this));
}

template<typename T, typename P>
size_t SW_base<T, P>::use_count() const noexcept
{
    return proxy->links_count();
}

template<typename T, typename P>
void SW_base<T, P>::swap(SW_base<T, P>& swb) noexcept
{
    std::swap(proxy, swb.proxy);
}

}

#endif // SW_BASE_H_INCLUDED
