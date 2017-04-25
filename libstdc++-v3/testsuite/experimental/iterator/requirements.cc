// Copyright (C) 2015-2016 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING3.  If not see
// <http://www.gnu.org/licenses/>.

// { dg-options "-std=gnu++14" }
// { dg-do compile }

// This is a compile-only test with minimal includes
#include <experimental/iterator>
#include <iosfwd>

using namespace std::experimental;

template<typename Delim, typename Char>
struct tester
{
  using joiner_type = ostream_joiner<Delim, Char>;
  using ostream_type = std::basic_ostream<Char>;
  using test_type = decltype(make_ostream_joiner(std::declval<ostream_type&>(),
                                                 std::declval<Delim>()));

  static_assert(is_same_v<test_type, joiner_type>, "");

  static_assert(is_same_v<typename test_type::char_type, Char>, "");

  static_assert(is_same_v<typename test_type::traits_type,
                          std::char_traits<Char>>, "");

  static_assert(is_same_v<typename test_type::ostream_type, ostream_type>, "");

  static_assert(is_same_v<typename test_type::iterator_category,
                          std::output_iterator_tag>, "");

  static_assert(is_same_v<typename test_type::value_type,        void>, "");
  static_assert(is_same_v<typename test_type::difference_type,   void>, "");
  static_assert(is_same_v<typename test_type::pointer,           void>, "");
  static_assert(is_same_v<typename test_type::reference,         void>, "");
};

tester<char, char> cc;
tester<int, char> ic;
#if _GLIBCXX_USE_WCHAR_T
tester<wchar_t, wchar_t> ww;
tester<int, wchar_t> iw;
#endif
