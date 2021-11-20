#ifndef BUG_BASIC_XSTRING_HPP
#define BUG_BASIC_XSTRING_HPP

#include "basic_xstring.hpp"

namespace meta {

template <typename CharT>
class buf_basic_string {
    using typename basic_xstring <CharT>::size_type;

protected:
    CharT* data_;
    size_type size_;
    size_type cap_;

public:
    buf_basic_string (size_type size) :
        data_ ()
    {

    }
};

}

#endif // BUG_BASIC_XSTRING_HPP