#ifndef SMART_PTR_H_INCLUDED
#define SMART_PTR_H_INCLUDED

#include "proxy.h"
#include "unique_ptr.h"
#include "SW_base.h"

namespace tuz
{

template<typename T>
class shared_ptr;

template<typename T>
class weak_ptr;

}

namespace std
{

template<typename T>
void swap(tuz::shared_ptr<T>& sp_a, tuz::shared_ptr<T>& sp_b) noexcept;

template<typename T>
void swap(tuz::weak_ptr<T>& wp_a, tuz::weak_ptr<T>& wp_b) noexcept;

}

namespace tuz
{

template<typename T, typename... R>
shared_ptr<T> make_shared(R&&... args);

template<typename T>
class enable_shared_from_this;

template<typename T>
class shared_ptr : public SW_base<T>
{
    template<typename U, typename... R>
    friend shared_ptr<U> make_shared(R&&... args);

    friend void std::swap<T>(shared_ptr& sp_a, shared_ptr& sp_b) noexcept;
    friend weak_ptr<T>;

private:
    void wp_init_helper(enable_shared_from_this<T>* ptr) noexcept;
    void wp_init_helper(...) noexcept {};

    template<typename D = default_delete<T>>
    void make_proxy(T* ptr = nullptr, const D& deleter = default_delete<T>());
    void set_new_proxy(Proxy_base<T>* p) noexcept;
    shared_ptr(Proxy_base<T>& p) noexcept;

    void check_out() noexcept;
    void check_in() noexcept;
    void substitute_proxy(Proxy_base<T>* another_proxy = Proxy_dummy<T>::instance_ptr()) noexcept;
    void set_proxy(Proxy_base<T>* another_proxy = Proxy_dummy<T>::instance_ptr()) noexcept;

public:
    shared_ptr() noexcept;
    template<typename D = default_delete<T>>
    explicit shared_ptr(T* ptr, const D& deleter = default_delete<T>());
    shared_ptr(const shared_ptr& sp) noexcept;
    shared_ptr(shared_ptr&& sp) noexcept;
    explicit shared_ptr(const weak_ptr<T>& wp);
    template<typename D>
    shared_ptr(unique_ptr<T, D>&& up);
    ~shared_ptr();

    shared_ptr& operator=(shared_ptr&& sp) noexcept;
    shared_ptr& operator=(const shared_ptr& sp) noexcept;
    template<typename D>
    shared_ptr& operator=(unique_ptr<T, D>&& up);

    template<typename D = default_delete<T>>
    void reset(T* ptr = nullptr, const D& deleter = default_delete<T>());

    T& operator*() const noexcept;
    T* operator->() const noexcept;
    operator bool() const noexcept;
    T* get() const noexcept;

    bool operator==(const shared_ptr& sp) const noexcept;
    bool operator<=(const shared_ptr& sp) const noexcept;
    bool operator>=(const shared_ptr& sp) const noexcept;
    bool operator<(const shared_ptr& sp) const noexcept;
    bool operator>(const shared_ptr& sp) const noexcept;
    bool operator!=(const shared_ptr& sp) const noexcept;
};


template<typename T>
class weak_ptr : public SW_base<T>
{
    friend void std::swap<T>(weak_ptr& sp_a, weak_ptr& sp_b) noexcept;
    friend shared_ptr<T>;

private:
    void check_out() noexcept;
    void check_in() noexcept;
    void substitute_proxy(Proxy_base<T>* another_proxy = Proxy_dummy<T>::instance_ptr()) noexcept;
    void set_proxy(Proxy_base<T>* another_proxy = Proxy_dummy<T>::instance_ptr()) noexcept;

public:
    weak_ptr() noexcept;
    weak_ptr(const weak_ptr& wp) noexcept;
    weak_ptr(const shared_ptr<T>& sp) noexcept;
    ~weak_ptr();

    weak_ptr& operator=(const weak_ptr& wp) noexcept;
    weak_ptr& operator=(const shared_ptr<T>& sp) noexcept;

    void reset() noexcept;

#ifdef DEBUG
    size_t DEBUG_weak_links_count() const noexcept
    {
        return SW_base<T>::proxy->DEBUG_weak_links_count();
    };
#endif

    shared_ptr<T> lock() const;
};


template<typename T>
class enable_shared_from_this
{
    friend shared_ptr<T>;

private:
    weak_ptr<T> wp_helper;

public:
    shared_ptr<T> shared_from_this() const;
};

}

#include "weak_ptr.h"
#include "shared_ptr.h"
#include "esft.h"

#endif // SMART_PTR_H_INCLUDED
