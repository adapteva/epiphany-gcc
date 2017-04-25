// { dg-do run }
// { dg-options "-std=gnu++11 -g -O0" }

// Copyright (C) 2014-2016 Free Software Foundation, Inc.
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

#include <memory>

struct x_struct
{
  int y;
};

int
main ()
{
  int *i = new int;
  *i = 10;
  std::unique_ptr<int> p(i);

  x_struct *x = new x_struct;
  x->y = 23;
  std::unique_ptr<x_struct> q(x);

// { dg-final { note-test *p 10 } }
// { dg-final { regexp-test p.get() 0x.* } }

// { dg-final { whatis-test *p int } }
// { dg-final { whatis-test p.get() "int \*" } }

// { dg-final { note-test *q {\{y = 23\}} } }
// { dg-final { regexp-test q.get() 0x.* } }
// { dg-final { note-test q->y 23 } }

// { dg-final { whatis-test *q x_struct } }
// { dg-final { whatis-test q.get() "x_struct \*" } }
// { dg-final { whatis-test q->y int } }

  return 0;  // Mark SPOT
}

// { dg-final { gdb-test SPOT {} 1 } }
