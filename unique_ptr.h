#ifndef UNIQUE_PTR_H_INCLUDED
#define UNIQUE_PTR_H_INCLUDED

#include <algorithm>

namespace tuz
{

template<class T>
class default_delete
{
public:
    void operator()(T*) const;
};

template<class T>
void default_delete<T>::operator()(T* ptr) const
{
    delete ptr;
}

template<class T, class D = default_delete<T>>
class unique_ptr
{
    T* ptr;
    D deleter;

public:
    explicit unique_ptr(T* ptr, const D& deleter = default_delete<T>());
    unique_ptr(const unique_ptr& up) = delete;
    unique_ptr(unique_ptr&& up) noexcept;
    unique_ptr() noexcept;
    ~unique_ptr();

    unique_ptr& operator=(unique_ptr&& up) noexcept;
    unique_ptr& operator=(const unique_ptr& up) = delete;

    T* release() noexcept;
    void swap(unique_ptr& up) noexcept;
    void reset(T* ptr = nullptr) noexcept;

    T* get() const noexcept;
    const D& get_deleter() const;
    operator bool() const noexcept;
    T& operator*() const noexcept;
    T* operator->() const noexcept;

    bool operator==(const unique_ptr& up) const noexcept;
    bool operator<=(const unique_ptr& up) const noexcept;
    bool operator>=(const unique_ptr& up) const noexcept;
    bool operator<(const unique_ptr& up) const noexcept;
    bool operator>(const unique_ptr& up) const noexcept;
    bool operator!=(const unique_ptr& up) const noexcept;
};

template<class T, class D>
unique_ptr<T, D>::unique_ptr() noexcept :
unique_ptr<T, D>(nullptr)
{
}

template<class T, class D>
unique_ptr<T, D>::unique_ptr(T* ptr, const D& deleter) : ptr(ptr), deleter(deleter)
{
}

template<class T, class D>
unique_ptr<T, D>::unique_ptr(unique_ptr&& up) noexcept :
unique_ptr<T, D>()
{
    swap(up);
}

template<class T, class D>
unique_ptr<T, D>& unique_ptr<T, D>::operator=(unique_ptr&& up) noexcept
{
    swap(up);
    up.reset();
    return *this;
}

template<class T, class D>
unique_ptr<T, D>::~unique_ptr()
{
    deleter(ptr);
}

template<class T, class D>
void unique_ptr<T, D>::swap(unique_ptr& up) noexcept
{
    std::swap(ptr, up.ptr);
    std::swap(deleter, up.deleter);
}

template<class T, class D>
T* unique_ptr<T, D>::release() noexcept
{
    T* tmp = ptr;
    ptr = nullptr;
    return tmp;
}

template<class T, class D>
void unique_ptr<T, D>::reset(T* ptr_) noexcept
{
    deleter(ptr);
    ptr = ptr_;
}

template<class T, class D>
T& unique_ptr<T, D>::operator*() const noexcept
{
    return *ptr;
}

template<class T, class D>
T* unique_ptr<T, D>::operator->() const noexcept
{
    return ptr;
}

template<class T, class D>
bool unique_ptr<T, D>::operator==(const unique_ptr& up) const noexcept
{
    return ptr == up.ptr;
}

template<class T, class D>
bool unique_ptr<T, D>::operator<=(const unique_ptr& up) const noexcept
{
    return ptr <= up.ptr;
}

template<class T, class D>
bool unique_ptr<T, D>::operator>=(const unique_ptr& up) const noexcept
{
    return ptr >= up.ptr;
}

template<class T, class D>
bool unique_ptr<T, D>::operator<(const unique_ptr& up) const noexcept
{
    return ptr < up.ptr;
}

template<class T, class D>
bool unique_ptr<T, D>::operator>(const unique_ptr& up) const noexcept
{
    return ptr > up.ptr;
}

template<class T, class D>
bool unique_ptr<T, D>::operator!=(const unique_ptr& up) const noexcept
{
    return !(*this == up);
}

template<class T, class D>
unique_ptr<T, D>::operator bool() const noexcept
{
    return ptr != nullptr;
}

template<class T, class D>
T* unique_ptr<T, D>::get() const noexcept
{
    return ptr;
}

template<class T, class D>
const D& unique_ptr<T, D>::get_deleter() const
{
    return deleter;
}

template<class T, class... R>
unique_ptr<T> make_unique(R&&... arg)
{
    T* ptr = new T(std::forward<R>(arg)...);
    return unique_ptr<T>(ptr);
}

}

namespace std
{

template<typename T, typename D>
void swap(tuz::unique_ptr<T, D>& up_a, tuz::unique_ptr<T, D>& up_b)
{
    up_a.swap(up_b);
}

}

#endif // UNIQUE_PTR_H_INCLUDED
