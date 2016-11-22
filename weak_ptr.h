#ifndef WEAK_PTR_H_INCLUDED
#define WEAK_PTR_H_INCLUDED

#include "smart_ptr.h"

namespace tuz
{

template<typename T>
void weak_ptr<T>::substitute_proxy(Proxy_base<T>* another_proxy) noexcept
{
    if(another_proxy != SW_base<T>::proxy)
    {
        check_out();
        set_proxy(another_proxy);
    }
}

template<typename T>
void weak_ptr<T>::set_proxy(Proxy_base<T>* another_proxy) noexcept
{
    SW_base<T>::proxy = another_proxy;
    check_in();
}

template<typename T>
void weak_ptr<T>::check_out() noexcept
{
    SW_base<T>::proxy->check_out(*this);
    if(SW_base<T>::proxy->need_deletion())
        delete SW_base<T>::proxy;
}

template<typename T>
void weak_ptr<T>::check_in() noexcept
{
    SW_base<T>::proxy->check_in(*this);
}

template<typename T>
weak_ptr<T>::weak_ptr() noexcept
{
    set_proxy();
}

template<typename T>
weak_ptr<T>::weak_ptr(const weak_ptr<T>& wp) noexcept
{
    set_proxy(wp.proxy);
}

template<typename T>
weak_ptr<T>::weak_ptr(const shared_ptr<T>& sp) noexcept
{
    set_proxy(sp.proxy);
}

template<typename T>
weak_ptr<T>& weak_ptr<T>::operator=(const weak_ptr& wp) noexcept
{
    substitute_proxy(wp.proxy);
    return *this;
}

template<typename T>
weak_ptr<T>& weak_ptr<T>::operator=(const shared_ptr<T>& sp) noexcept
{
    substitute_proxy(sp.proxy);
    return *this;
}

template<typename T>
weak_ptr<T>::~weak_ptr()
{
    check_out();
}

template<typename T>
void weak_ptr<T>::reset() noexcept
{
    substitute_proxy();
}

template<typename T>
shared_ptr<T> weak_ptr<T>::lock() const
{
    if(SW_base<T>::proxy->expired())
        return shared_ptr<T>();
    else
        return shared_ptr<T>(*this);
    //здесь исключение никогда не будет кинуто, потому что proxy 100% не протухшая
}

}

#include <algorithm>

namespace std
{

template<typename T>
void swap(tuz::weak_ptr<T>& wp_a, tuz::weak_ptr<T>& wp_b) noexcept
{
    wp_a.swap(wp_b);
}

}

#endif // WEAK_PTR_H_INCLUDED
