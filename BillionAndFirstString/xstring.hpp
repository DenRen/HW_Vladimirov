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
    // todo: sso
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

        _add (str, size);
    }

    void inline
    add (const CharT* str) {
        _add (str, _strlen (str));
    }

    void inline
    add (const xstring <CharT>& other) noexcept {
        _add (other.data_, other.size_);
    }

    xstring <CharT>&
    operator += (const xstring <CharT>& other) noexcept {
        _add (other.data_, other.size_);
        return *this;
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

    CharT
    operator[] (size_type position) const {
        if (position > size_) {
            throw std::invalid_argument ("Out of range");
        }

        return data_[position];
    }

    CharT&
    operator[] (size_type position) {
        if (position > size_) {
            throw std::invalid_argument ("Out of range");
        }

        return data_[position];
    }

    size_type
    find (const CharT* str) const {
        if (str == nullptr) {
            throw std::invalid_argument ("null pointer exception");
        }

        CharT* pos = NULL;
        if ((pos = strstr (data_, str)) == NULL) {
            return npos;
        }

        return pos - data_;
    }

    // End of string character allowed
    size_type
    find (CharT symb) const noexcept {
        CharT* cur_symb = data_, *end_symb = data_ + size_;

        do {
            if (*cur_symb == symb) {
                return cur_symb - data_;
            }
        } while (cur_symb++ < end_symb);

        return npos;
    }

    void
    relpace_all (const xstring <CharT>& from, const xstring <CharT>& to) {
        const size_type len_from = from.size_;
        const size_type len_to = to.size_;

        if (len_from >= len_to) {
            // Capacity will not change
            CharT* occur = strstr (data_, from.data_);
            if (occur == nullptr) {
                return;
            }

            memcpy (occur, to.data_, to.size_);

            const size_type _delta = from.size_ - to.size_; // >= 0
            CharT* right_bound = occur + to.size_;
            CharT* finder = occur + from.size_;
            const CharT* end_data = data_ + size_;

            while (finder != end_data) {
                occur = strstr (finder, from.data_);
                if (occur == nullptr) {
                    const size_type size_shift_block = end_data - finder;
                    // Plus 1, because we shift also '\0'
                    _mem_left_shift (finder, size_shift_block + 1, finder - right_bound);
                    
                    size_ = right_bound - data_ + size_shift_block;
                    return;
                }

                const size_type size_shift_block = occur - finder;
                _mem_left_shift (finder, size_shift_block, finder - right_bound);
                memcpy (right_bound + size_shift_block, to.data_, to.size_);

                right_bound += size_shift_block + to.size_;
                finder = occur + from.size_;
            }

            *right_bound = '\0';
            size_ = right_bound - data_;
        } else {
            // Capacity will change
    }

private:
    // size without terminated zero
    void
    _add (const CharT* str, size_type size) noexcept {
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

    // witout overmapping
    void _mem_left_shift (CharT* src, size_type size, size_type shift) {
        memcpy (src - shift, src, size);
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
    throw std::runtime_error ("Need to be implement");
    return is;
}

#endif // XSTRING_HPP