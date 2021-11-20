#pragma once

#include "buf_mivector.hpp"

namespace meta {

template <typename T>
class mivector : public buf_mivector <T> {
public:
    using size_type = std::size_t;
    using buf_mivector <T>::data_;
    using buf_mivector <T>::size_;
    using buf_mivector <T>::cap_;

    mivector (std::size_t size = 0) :
        buf_mivector <T> (size)
    {
        this->construct (data_, data_ + size_);
    }

    mivector (const mivector <T>& other) :
        buf_mivector <T> (other.size_)
    {
        this->construct (data_, data_ + cap_, other.data_, other.data_ + cap_);
    }

    size_type size () const {
        return size_;
    }

    size_type capacity () const {
        return cap_;
    }

    void dump () const {
        std::cout << "size: " << size_ << ", capacity: " << cap_ << std::endl;

        for (std::size_t i = 0; i < size_; ++i) {
            std::cout << i << ") " << data_[i] << std::endl;
        }
    }

    void push (const T& value) {
        if (size_ == cap_) {
            reserve (2 * cap_ + 1);
        }

        construct (data_ + size_, value);
        ++size_;
    }

    T top () {
        if (size_ == 0) {
            throw std::invalid_argument ("");
        }
        return data_[size_ - 1]; 
    }

    void pop () {
        destroy (&data_[size_ + 1]);
        --size_;
    }

    void swap (mivector <T>& other) noexcept {
        std::swap (data_, other.data_);
        std::swap (size_, other.size_);
        std::swap (cap_, other.cap_);
    }

    void reserve (size_type new_size) {
        if (new_size > cap_) {
            buf_mivector <T> new_buf (new_size);
            mivector <T> new_vec (std::move (new_buf));

            new_vec.construct (new_vec.data_, new_vec.data_ + size_,
                                data_, data_ + size_);
            this->swap (new_vec);
        }
    }

private:
    mivector (buf_mivector <T>&& buf) noexcept :
        buf_mivector <T> (std::move (buf))
    {}
};

}