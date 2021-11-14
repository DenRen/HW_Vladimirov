#ifndef XSTRING_HPP
#define XSTRING_HPP

#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "char_traits.hpp"

namespace meta {

template <typename CharT,
          typename Traits = char_traits <CharT>>
class basic_xstring {
public:
    using size_type = std::size_t;

    static const size_type npos = -1;
    static const size_type size_empty = 0;

private:
    // todo: sso
    CharT* data_;
    size_type size_;    // Не считая '\0'
    size_type cap_;     // Не считая '\0'

    static const size_type init_cap = 16 - 1;

    size_type
    _strlen (const char* str) const noexcept {
        return std::strlen (str);
    }

    template <typename CharU>
    size_type
    _strlen (const CharU* str) const noexcept {
        const CharU* cur = str;
        for (;
             Traits::is_equal (*cur, Traits::eof ()) == false;
             ++cur)
        {}

        return cur - str;
    }

    // Used for calc data_[] size from number of symbols
    constexpr size_type
    _size_buf_from_num_symb (size_type number_symbols) noexcept {
        return number_symbols + 1;
    }

public:
    // Constructos ------------------------------------------------------------
    basic_xstring () :
        data_ (new CharT[_size_buf_from_num_symb (init_cap)]),
        size_ (size_empty),
        cap_ (init_cap)
    {
        data_[0] = Traits::eof ();
    }

    basic_xstring (basic_xstring&& other) :
        size_ (other.size_),
        cap_ (other.cap_),
        data_ (other.data_)
    {
        other.data_ = nullptr;
    }

    basic_xstring (const basic_xstring& other) :
        data_ (new CharT[_size_buf_from_num_symb (other.size_)]),
        size_ (other.size_),
        cap_ (other.cap_)
    {
        std::copy (other.data_, other.data_ + size_ + 1, data_);
    }

    basic_xstring (const CharT* str) :
        basic_xstring (str, _strlen (str)) // Only for char!
    {}

    basic_xstring (const CharT* str, size_type size) :
        size_ (size),
        cap_ (size_)
    {
        data_ = new CharT[_size_buf_from_num_symb (cap_)];

        std::copy (str, str + size_ + 1, data_);
    }

    ~basic_xstring () {
        delete[] data_;
    }

    // Getters string params  -------------------------------------------------
    CharT*
    c_str () const noexcept {
        return data_;
    }

    size_type inline
    capacity () const noexcept {
        return cap_;
    }

    size_type inline
    length () const noexcept {
        return size_;
    }

    bool inline
    is_empty () const noexcept {
        return size_ == size_empty;
    }

    // Basic string methods ---------------------------------------------------

    // Max symbol of pos is end of string character
    CharT
    at (size_type pos) const {
        if (pos > size_) {
            throw std::invalid_argument ("Out of range");
        }

        return data_[pos];
    } // at (size_type pos)

    void
    reserve (size_type new_size) {
        if (new_size > cap_) {
            CharT* const new_buffer = new CharT[_size_buf_from_num_symb (new_size)];

            std::copy (data_, data_ + size_ + 1, new_buffer);
            delete[] data_;

            cap_ = new_size;
            data_ = new_buffer;
        }
    } // reserve (size_type new_size)

    // size without terminated zero
    void
    add (const CharT* str, size_type size) {
        if (str == nullptr) {
            throw std::invalid_argument ("Nullpointer exception");
        }

        _add (str, size);
    }

    void inline
    add (const CharT* str) {
        _add (str, _strlen (str));
    }

    void inline
    add (const basic_xstring <CharT>& other) noexcept {
        _add (other.data_, other.size_);
    }

    basic_xstring <CharT>&
    operator += (const basic_xstring <CharT>& other) noexcept {
        _add (other.data_, other.size_);
        return *this;
    } // operator += (const xstring <CharT>& other)

    CharT
    operator[] (size_type position) const {
        if (position > size_) {
            throw std::invalid_argument ("Out of range");
        }

        return data_[position];
    } // operator[] (size_type position)

    CharT&
    operator[] (size_type position) {
        if (position > size_) {
            throw std::invalid_argument ("Out of range");
        }

        return data_[position];
    } // operator[] (size_type position)

    size_type
    find (const CharT* str) const {
        if (str == nullptr) {
            throw std::invalid_argument ("Out of range");
        }

        const size_type len = _strlen (str);
        if (len > size_) {
            return npos;
        }

        const CharT* begin = data_, *end = data_ + size_;
        const CharT* pos = std::search (begin, end, str, str + len, Traits::is_equal);
        if (pos == end) {
            return npos;
        }

        return pos - begin;
    } // find (const CharT* str)

    // End of string character allowed
    size_type
    find (CharT symb) const noexcept {
        const CharT* cbegin = data_, *cend = data_ + size_ + 1;

        auto res = std::find (cbegin, cend, symb);
        return res == cend ? npos : res - cbegin;
    } // find (CharT symb)

    void
    relpace_all (const basic_xstring <CharT>& from, const basic_xstring <CharT>& to) {
        if (from.size_ >= to.size_) {
            _replace_all_from_greater_to (from, to);
        } else {
            _replace_all_from_less_to (from, to);
        }
    } // relpace_all (const xstring <CharT>& from, const xstring <CharT>& to)

    bool
    is_equal (const basic_xstring <CharT>& other) const {
        return size_ == other.size_ &&
               std::equal (data_, data_ + size_, other.data_);
    } // is_equal (const xstring <CharT>& other)

private:
    // size without terminated zero
    void
    _add (const CharT* str, size_type size) noexcept {
        const size_type new_size = size_ + size;
        reserve (new_size);

        std::copy (str, str + size, data_ + size_);

        *(data_ + new_size) = Traits::eof ();

        size_ = new_size;
    } // _add (const CharT* str, size_type size)

    // witout overmapping
    void
    _mem_left_shift (CharT* src, size_type size, size_type shift) {
        std::copy (src, src + size, src - shift);
    }

    void
    _replace_all_from_greater_to (const basic_xstring <CharT>& from, const basic_xstring <CharT>& to) {
        // Capacity will not change
        CharT* occur = std::search (data_, data_ + size_,
                                    from.data_, from.data_ + from.size_,
                                    Traits::is_equal);
        if (occur == data_ + size_) {
            return;
        }

        std::copy (to.data_, to.data_ + to.size_, occur);

        const size_type _delta = from.size_ - to.size_; // >= 0
        CharT* right_bound = occur + to.size_;
        CharT* finder = occur + from.size_;
        const CharT* end_data = data_ + size_;

        while (finder != end_data) {
            occur = std::search (finder, const_cast <CharT*> (end_data),
                                 from.data_, from.data_ + from.size_,
                                Traits::is_equal);
            if (occur == end_data) {
                const size_type size_shift_block = end_data - finder;
                // Plus 1, because we shift also '\0'
                _mem_left_shift (finder, size_shift_block + 1, finder - right_bound);

                size_ = right_bound - data_ + size_shift_block;
                return;
            }

            const size_type size_shift_block = occur - finder;
            _mem_left_shift (finder, size_shift_block, finder - right_bound);
            std::copy (to.data_, to.data_ + to.size_, right_bound + size_shift_block);

            right_bound += size_shift_block + to.size_;
            finder = occur + from.size_;
        }

        *right_bound = Traits::eof ();
        size_ = right_bound - data_;
    } // _replace_all_from_greater_to (const basic_xstring <CharT>& from, const basic_xstring <CharT>& to)

    void
    _replace_all_from_less_to (const basic_xstring <CharT>& from, const basic_xstring <CharT>& to) {
        // Capacity will change

        // Calculate new data size
        size_type extra_size = 0;

        CharT* finder = data_;
        const CharT* end_data = data_ + size_;
        while (finder != end_data) {
            CharT* occur = std::search (finder, const_cast <CharT*> (end_data),
                                        from.data_, from.data_ + from.size_,
                                        Traits::is_equal);
            if (occur == end_data) {
                break;
            }

            extra_size += to.size_ - from.size_;
            finder = occur + from.size_;
        }

        if (extra_size == 0) {
            return;
        }

        // Fill new buffer and swap data pointers
        const size_type new_size = size_ + extra_size;
        const size_type new_cap = _size_buf_from_num_symb (new_size);
        CharT* const new_data_ = new CharT[new_cap];

        CharT* new_right_bound = new_data_;

        finder = data_;
        while (finder != end_data) {
            CharT* occur = std::search (finder, const_cast <CharT*> (end_data),
                                        from.data_, from.data_ + from.size_,
                                        Traits::is_equal);
            if (occur == end_data) {
                const size_type size_block = end_data - finder;
                // Plus 1, because we shift also '\0'
                std::copy (finder, finder + size_block + 1, new_right_bound);

                delete[] data_;
                data_ = new_data_;
                size_ = new_size;
                cap_ = new_cap;

                return;
            }

            const size_type size_block = occur - finder;
            std::copy (finder, finder + size_block, new_right_bound);
            std::copy (to.data_, to.data_ + to.size_, new_right_bound + size_block);

            new_right_bound += size_block + to.size_;
            finder = occur + from.size_;
        }

        *new_right_bound = Traits::eof ();

        delete[] data_;
        data_ = new_data_;
        size_ = new_size;
        cap_ = new_cap;
    } // _replace_all_from_less_to (const basic_xstring <CharT>& from, const basic_xstring <CharT>& to)

}; // basic_xstring <CharT>

using cstring = basic_xstring <char>;
using wstring = basic_xstring <wchar_t>;

} // namespace meta

template <typename CharT>
bool
operator == (const ::meta::basic_xstring <CharT>& lhs, const ::meta::basic_xstring <CharT>& rhs) {
    return lhs.is_equal (rhs);
}

template <typename CharT>
bool
operator != (const ::meta::basic_xstring <CharT>& lhs, const ::meta::basic_xstring <CharT>& rhs) {
    return (lhs == rhs) == false;
}

// os - может быть неудачное название для output stream
template <typename CharT>
std::basic_ostream <CharT>&
operator << (std::basic_ostream <CharT>& os, const ::meta::basic_xstring <CharT>& xstr) {
    return os << xstr.c_str ();
}

// is - может быть неудачное название для input stream
template <typename CharT>
std::basic_istream <CharT>&
operator >> (std::basic_istream <CharT>& is, ::meta::basic_xstring <CharT>& xstr) {
    throw std::runtime_error ("Need to be implement");
    return is;
}

#endif // XSTRING_HPP