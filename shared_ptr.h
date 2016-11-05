#ifndef SHARED_PTR_H_INCLUDED
#define SHARED_PTR_H_INCLUDED

#include <algorithm>
#include <type_traits>

#include "smart_ptr.h"
#include "exception.h"

namespace tuz
{

template<typename T>
void shared_ptr<T>::wp_init_helper(enable_shared_from_this<T>* ptr) noexcept
{
    ptr->wp_helper = weak_ptr<T>(*this);
}

template<typename T>
template<typename D>
shared_ptr<T>::shared_ptr(T* ptr, const D& deleter)
{
    make_proxy(ptr, deleter);
}

template<typename T>
shared_ptr<T>::shared_ptr() noexcept
{
    make_proxy();
}

template<typename T>
shared_ptr<T>::shared_ptr(shared_ptr<T>&& sp) noexcept :
shared_ptr()
{
    SW_base<T>::swap(sp);
}

template<typename T>
void shared_ptr<T>::set_new_proxy(Proxy_base<T>* p) noexcept
{
    set_proxy(p);

    wp_init_helper(SW_base<T>::proxy->get());
}

template<typename T>
template<typename D>
void shared_ptr<T>::make_proxy(T* ptr, const D& deleter)
{
    if(ptr)
        set_new_proxy(new Proxy_deleter<T, D>(ptr, deleter));
    else
        set_proxy();
}

template<typename T>
void shared_ptr<T>::check_out() noexcept
{
    SW_base<T>::proxy->check_out(*this);
    if(SW_base<T>::proxy->need_deletion())
        delete SW_base<T>::proxy;
}

template<typename T>
void shared_ptr<T>::check_in() noexcept
{
    SW_base<T>::proxy->check_in(*this);
}

template<typename T>
void shared_ptr<T>::set_proxy(Proxy_base<T>* another_proxy) noexcept
{
    SW_base<T>::proxy = another_proxy;
    check_in();
}

template<typename T>
void shared_ptr<T>::substitute_proxy(Proxy_base<T>* another_proxy) noexcept
{
    check_out();
    set_proxy(another_proxy);
}

template<typename T>
shared_ptr<T>::shared_ptr(Proxy_base<T>& p) noexcept
{
    set_new_proxy(&p);
}

template<typename T>
shared_ptr<T>::shared_ptr(const shared_ptr& sp) noexcept
{
    set_proxy(sp.proxy);
}

template<typename T>
shared_ptr<T>::shared_ptr(const weak_ptr<T>& wp)
{
    if(wp.proxy->expired())
        throw bad_weak_ptr();
    else
        set_proxy(wp.proxy);
}

template<typename T>
template<typename D>
shared_ptr<T>::shared_ptr(unique_ptr<T, D>&& up) : shared_ptr(up.get(), up.get_deleter())
{
    up.release();
}

template<typename T>
shared_ptr<T>::~shared_ptr()
{
    check_out();
}

template<typename T>
shared_ptr<T>& shared_ptr<T>::operator=(shared_ptr&& sp) noexcept
{
    SW_base<T>::swap(sp);
    sp.reset();
    return *this;
}

template<typename T>
shared_ptr<T>& shared_ptr<T>::operator=(const shared_ptr& sp) noexcept
{
    substitute_proxy(sp.proxy);
    return *this;
}

template<typename T>
template<typename D>
shared_ptr<T>& shared_ptr<T>::operator=(unique_ptr<T, D>&& up)
{
    check_out();
    make_proxy(up.get(), up.get_deleter());

    up.release();

    return *this;
}

template<typename T>
template<typename D>
void shared_ptr<T>::reset(T* ptr, const D& deleter)
{
    check_out();
    make_proxy(ptr, deleter);
}

template<typename T>
bool shared_ptr<T>::operator==(const shared_ptr& sp) const noexcept
{
    return SW_base<T>::proxy->get() == sp.proxy->get();
}

template<typename T>
bool shared_ptr<T>::operator<=(const shared_ptr& sp) const noexcept
{
    return SW_base<T>::proxy->get() <= sp.proxy->get();
}

template<typename T>
bool shared_ptr<T>::operator>=(const shared_ptr& sp) const noexcept
{
    return SW_base<T>::proxy->get() >= sp.proxy->get();
}

template<typename T>
bool shared_ptr<T>::operator<(const shared_ptr& sp) const noexcept
{
    return SW_base<T>::proxy->get() < sp.proxy->get();
}

template<typename T>
bool shared_ptr<T>::operator>(const shared_ptr& sp) const noexcept
{
    return SW_base<T>::proxy->get() > sp.proxy->get();
}

template<typename T>
bool shared_ptr<T>::operator!=(const shared_ptr& sp) const noexcept
{
    return !(*this == sp);
}

template<typename T>
T& shared_ptr<T>::operator*() const noexcept
{
    return *SW_base<T>::proxy->get();
}

template<typename T>
T* shared_ptr<T>::operator->() const noexcept
{
    return SW_base<T>::proxy->get();
}

template<typename T>
shared_ptr<T>::operator bool() const noexcept
{
    return SW_base<T>::proxy->get() != nullptr;
}

template<typename T>
T* shared_ptr<T>::get() const noexcept
{
    return SW_base<T>::proxy->get();
}

template<typename T, typename... R>
shared_ptr<T> make_shared(R&&... args)
{
    Proxy_base<T>* pb = new Make_shared_proxy<T>(std::forward<R>(args)...);

    return shared_ptr<T>(*pb);
}

}

namespace std
{

template<typename T>
void swap(tuz::shared_ptr<T>& sp_a, tuz::shared_ptr<T>& sp_b) noexcept
{
    sp_a.swap(sp_b);
}

}

#endif // SHARED_PTR_H_INCLUDED
