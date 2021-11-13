#ifndef XSTRING_HPP
#define XSTRING_HPP

#include <string>
#include <cstring>
#include <iostream>

namespace meta {

template <typename CharT>
class xstring {
public:
    using size_type = std::size_t;

    static const size_type npos = -2;
    static const size_type size_empty = 0;

private:
    CharT* data_;
    size_type size_;    // Не считая '\0'
    size_type cap_;     // Не считая '\0'

    static const size_type init_cap = 16 - 1;

    size_type 
    _strlen (const CharT* str) const noexcept {
        // TODO: implement for not only char type
        return std::strlen (str);
    }

    // Used for calc data_[] size from number of symbols
    constexpr size_type
    _size_buf_from_num_symb (size_type number_symbols) noexcept {
        return number_symbols + 1;
    }

public:
    xstring () :
        data_ (new CharT[_size_buf_from_num_symb (init_cap)]),
        size_ (size_empty),
        cap_ (init_cap)
    {
        data_[0] = '\0';
    }

    xstring (xstring&&) = default;

    xstring (const xstring& other) :
        data_ (new CharT[_size_buf_from_num_symb (other.size_)]),
        size_ (other.size_),
        cap_ (other.cap_)
    {
        // Вопрос про memcpy
        for (const auto& symb : other.data_) {
            data_ = symb;
        }
    }

    xstring (const CharT* str) :
        xstring (str, _strlen (str)) // Only for char!
    {}

    xstring (const CharT* str, size_type size) :
        size_ (size),
        cap_ (size_)
    {
        data_ = new CharT[_size_buf_from_num_symb (cap_)];
        for (std::size_t i = 0; i <= size_; ++i) {
            data_[i] = str[i];
        }
    }

    ~xstring () {
        delete[] data_;
    }

    CharT* c_str () const noexcept {
        return data_;
    }

    // Max symbol of pos is end of string character
    CharT at (size_type pos) const {
        if (pos > size_) {
            throw std::invalid_argument ("overbound!");
        }

        return data_[pos];
    }

    // End of string character allowed
    size_type find (CharT symb) const {
        CharT* cur_symb = data_, *end_symb = data_ + size_;

        do {
            if (*cur_symb == symb) {
                return cur_symb - data_;
            }
        } while (cur_symb++ < end_symb);

        return npos;
    }

    void reserve (size_type new_size) {
        if (new_size > cap_) {
            CharT* const new_buffer = new CharT[_size_buf_from_num_symb (new_size)];

            CharT* cur_new_symb = new_buffer, *cur_symb = data_;
            const CharT* end_symb = data_ + size_;
            do {
                *cur_new_symb++ = *cur_symb++;
            } while (cur_symb < end_symb);

            cap_ = new_size;

            delete[] data_;
            data_ = new_buffer;
        }
    }

    // size without terminated zero
    void
    add (const CharT* str, size_type size) {
        if (str == nullptr && size == 0) {
            throw std::invalid_argument ("Empty string!");
        }

        const size_type new_size = size_ + size;

        reserve (new_size);

        CharT* cur_symb = data_ + size_;
        const CharT* end_symb = cur_symb + size;
        while (cur_symb < end_symb) {
            *cur_symb++ = *str++;
        }

        *cur_symb = '\0';

        size_ = new_size;
    }

    void
    add (const CharT* str) {
        add (str, _strlen (str));
    }

    void
    add (const xstring <CharT>& other) {
        add (other.data_, other.size_);
    }

    xstring <CharT>&
    operator += (const xstring <CharT>& other) {
        add (other);
        return *this;
    }

    size_type capacity () const {
        return cap_;
    }

    size_type length () const {
        return size_;
    }

    bool is_empty () const {
        return size_ == size_empty;
    }
};

} // namespace meta

// os - может быть неудачное название для output stream
template <typename CharT>
std::basic_ostream <CharT>&
operator << (std::basic_ostream <CharT>& os, const ::meta::xstring <CharT>& xstr) {
    return os << xstr.c_str ();
}


// is - может быть неудачное название для input stream
template <typename CharT>
std::basic_istream <CharT>&
operator >> (std::basic_istream <CharT>& is, ::meta::xstring <CharT>& xstr) {
    const unsigned BUF_SIZE = 128;
    CharT buffer[BUF_SIZE] = {0};
    /*
    CharT cur_symb;
    do {
        for (unsigned counter = 0; counter < BUF_SIZE - 1; ++counter) {
            is >> cur_symb;

            buffer[counter] = cur_symb;
            if (cur_symb == '\0' || cur_symb == '\n') {
                buffer[counter] = '\0';
                break;
            }
        }

        xstr += xstr;

    } while ();*/
}

#endif // XSTRING_HPP