#ifndef XSTRING_HPP
#define XSTRING_HPP

#include <string>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "char_traits.hpp"
#include "basic_xstring.hpp"

namespace meta {

using cstring = basic_xstring <char>;
using wstring = basic_xstring <wchar_t>;

} // namespace meta

#endif // XSTRING_HPP