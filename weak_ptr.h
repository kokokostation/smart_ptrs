#ifndef WEAK_PTR_H_INCLUDED
#define WEAK_PTR_H_INCLUDED

#include "smart_ptr.h"

namespace tuz
{

template<typename T>
weak_ptr<T>::weak_ptr() noexcept
{
    SW_base<T, weak_ptr>::set_proxy();
}

template<typename T>
weak_ptr<T>::weak_ptr(const weak_ptr<T>& wp) noexcept
{
    SW_base<T, weak_ptr>::set_proxy(wp.proxy);
}

template<typename T>
weak_ptr<T>::weak_ptr(const shared_ptr<T>& sp) noexcept
{
    SW_base<T, weak_ptr>::set_proxy(sp.proxy);
}

template<typename T>
weak_ptr<T>& weak_ptr<T>::operator=(const weak_ptr& wp) noexcept
{
    weak_ptr<T> tmp(wp);

    std::swap(*this, tmp);

    return *this;
}

template<typename T>
weak_ptr<T>& weak_ptr<T>::operator=(const shared_ptr<T>& sp) noexcept
{
    return *this = weak_ptr(sp);
}

template<typename T>
weak_ptr<T>::~weak_ptr()
{
    SW_base<T, weak_ptr>::check_out();
}

template<typename T>
void weak_ptr<T>::reset() noexcept
{
    SW_base<T, weak_ptr>::check_out();
    SW_base<T, weak_ptr>::set_proxy();
}

template<typename T>
shared_ptr<T> weak_ptr<T>::lock() const
{
    if(SW_base<T, weak_ptr>::proxy->expired())
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
