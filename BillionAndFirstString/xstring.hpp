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
    static const size_type size_empty = -1;

private:
    CharT* data_;
    size_type size_;    // Не считая '\0'
    size_type cap_;     // Не считая '\0'

    static const int init_size = 16 - 1;
    
public:
    xstring () :
        data_ (new CharT[init_size + 1]),
        size_ (size_empty),
        cap_ (init_size)
    {}

    xstring (const xstring& other) :
        data_ (new CharT[other.size_ + 1]),
        size_ (other.size_),
        cap_ (other.cap_)
    {
        // Вопрос про memcpy
        for (const auto& symb : other.data_) {
            data_ = symb;
        }
    }

    xstring (const CharT* str) :
        xstring (str, std::strlen (str)) // Only for char!
    {}

    xstring (const CharT* str, size_type size) :
        size_ (size),
        cap_ (size_)
    {
        data_ = new CharT[cap_ + 1];
        for (std::size_t i = 0; i <= size_; ++i) {
            data_[i] = str[i];
        }
    }

    CharT* c_str () const {
        return data_;
    }

    // Max symbol of pos is end of string character
    CharT at (size_type pos) const {
        if (pos >= size_) {
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
            CharT* new_buffer = new CharT[new_size];

            CharT* cur_new_symb = new_buffer, cur_symb = data_;
            const CharT* end_symb = data_ + size_;
            do {
                *cur_new_symb++ = *cur_symb++;
            } while (cur_symb < end_symb);

            cap_ = new_size;
        }
    }

    void
    add (const CharT* str, size_type size) {
        reserve (size_ + size);

        CharT* cur_symb = data_ + size_ + 1, end_symb = data_ + size_ + size;
        do {
            *cur_symb++ = *str++;
        } while (cur_symb < end_symb);
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
std::basic_ostream <CharT>& operator << (std::basic_ostream <CharT>& os, const meta::xstring <CharT>& xstr) {
    return os << xstr.c_str ();
}


// is - может быть неудачное название для input stream
template <typename CharT>
std::basic_istream <CharT>&
operator >> (std::basic_istream <CharT>& is, const meta::xstring <CharT>& xstr) {
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