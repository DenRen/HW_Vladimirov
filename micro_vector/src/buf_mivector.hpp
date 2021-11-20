#pragma once

#include <cstdlib>

namespace meta {

template <typename T, typename... Args>
void construct (T* ptr, Args... args) {
    new (ptr) T (args...);
}

template <typename T>
void destroy (T* ptr) {
    ptr->~T ();
}
 
template <typename T>
class buf_mivector {
protected:
    using size_type = std::size_t;

    T* data_;
    size_type size_;
    size_type cap_;

public:
    buf_mivector (size_type size = 0) :
        data_ (size == 0 ?
               nullptr :
               static_cast <T*> (::operator new (sizeof (T) * size))),
        size_ (0),
        cap_ (size)
    {}

    buf_mivector (buf_mivector <T>&& buf) :
        data_ (buf.data_),
        size_ (buf.size_),
        cap_ (buf.cap_)
    {
        buf.data_ = nullptr;
        buf.size_ = 0;
    }

    ~buf_mivector () {
        destroy (data_, data_ + size_);
        ::operator delete (data_);
    }
    
    template <typename ForwardIter>
    void construct (ForwardIter first, ForwardIter last) {
        while (first != last) {
            ::meta::construct (first++);
        }
    }

    template <typename ForwardIter>
    void construct (ForwardIter first, ForwardIter last,
                         ForwardIter input_first, ForwardIter input_last)
    {
        while (first != last) {
            ::meta::construct <T> (first++, *input_first++);
            ++size_;
        }
    }

    template <typename ForwardIter>
    void destroy (ForwardIter first, ForwardIter last) {
        while (first != last) {
            ::meta::destroy <T> (first++);
        }
    }
};

}
