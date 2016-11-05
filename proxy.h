#ifndef PROXY_H_INCLUDED
#define PROXY_H_INCLUDED

#include "unique_ptr.h"

namespace tuz
{

template<typename T>
class weak_ptr;

template<typename T>
class shared_ptr;

template<typename T>
class Proxy_base
{
private:
    size_t shared_links, weak_links;
    T* ptr;

    Proxy_base(const Proxy_base&) = delete;
    Proxy_base& operator=(const Proxy_base&) = delete;

public:
explicit Proxy_base(T* ptr, size_t shared_links = 0, size_t weak_links = 0) noexcept :
    shared_links(shared_links), weak_links(weak_links), ptr(ptr) {};

    template<typename D>
    void check_in(const shared_ptr<D>& sp) noexcept;
    template<typename D>
    void check_in(const weak_ptr<D>& wp) noexcept;
    template<typename D>
    void check_out(const shared_ptr<D>& sp) noexcept;
    template<typename D>
    void check_out(const weak_ptr<D>& wp) noexcept;

    size_t links_count() const noexcept;

#ifdef DEBUG
    size_t DEBUG_weak_links_count() const noexcept
    {
        return weak_links;
    };
#endif

    bool need_deletion() const noexcept;
    bool expired() const noexcept;

    T* get() const noexcept;

    virtual void delete_(T* ptr) noexcept = 0;
    virtual ~Proxy_base() = default;
};

template<typename T>
bool Proxy_base<T>::need_deletion() const noexcept
{
    return !shared_links && !weak_links;
}

template<typename T>
bool Proxy_base<T>::expired() const noexcept
{
    return !shared_links;
}

template<typename T>
template<typename D>
void Proxy_base<T>::check_in(const shared_ptr<D>& sp) noexcept
{
    ++shared_links;
}

template<typename T>
template<typename D>
void Proxy_base<T>::check_in(const weak_ptr<D>& wp) noexcept
{
    ++weak_links;
}

template<typename T>
template<typename D>
void Proxy_base<T>::check_out(const shared_ptr<D>& sp) noexcept
{
    if(shared_links == 1)
        delete_(ptr);

    --shared_links;
}

template<typename T>
template<typename D>
void Proxy_base<T>::check_out(const weak_ptr<D>& wp) noexcept
{
    --weak_links;
}

template<typename T>
size_t Proxy_base<T>::links_count() const noexcept
{
    return shared_links;
}

template<typename T>
T* Proxy_base<T>::get() const noexcept
{
    return ptr;
}

template<typename T>
class Proxy_dummy : public Proxy_base<T>
{
private:
Proxy_dummy() noexcept :
    Proxy_base<T>(nullptr, 1) {};

public:
    static Proxy_dummy* instance_ptr() noexcept
    {
        static Proxy_dummy the_single_instance;
        return &the_single_instance;
    };
    virtual void delete_(T* ptr) noexcept override {};
};

template<typename T, typename D>
class Proxy_deleter : public Proxy_base<T>
{
private:
    D deleter;

public:
    Proxy_deleter(T* ptr, const D& deleter) : Proxy_base<T>(ptr), deleter(deleter) {};

    virtual void delete_(T* ptr) noexcept override
    {
        deleter(ptr);
    };
};


template<typename T>
class Make_shared_proxy : public Proxy_base<T>
{
private:
    char data[sizeof(T)];

    virtual void delete_(T* ptr) noexcept override
    {
        Proxy_base<T>::get()->~T();
    };

public:
    template<typename... R>
    Make_shared_proxy(R&&... args) : Proxy_base<T>(reinterpret_cast<T*>(data))
    {
        new(Proxy_base<T>::get()) T(std::forward<R>(args)...);
    };
};

}

#endif // PROXY_H_INCLUDED
