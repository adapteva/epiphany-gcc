// { dg-do run { target *-*-freebsd* *-*-dragonfly* *-*-netbsd* *-*-linux* *-*-gnu* *-*-solaris* *-*-cygwin *-*-rtems* *-*-darwin* powerpc-ibm-aix* } }
// { dg-options " -std=gnu++11 -pthread" { target *-*-freebsd* *-*-dragonfly* *-*-netbsd* *-*-linux* *-*-gnu* powerpc-ibm-aix* } }
// { dg-options " -std=gnu++11 -pthreads" { target *-*-solaris* } }
// { dg-options " -std=gnu++11 " { target *-*-cygwin *-*-rtems* *-*-darwin* } }
// { dg-require-cstdint "" }
// { dg-require-gthreads "" }

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

#include <condition_variable>
#include <thread>
#include <mutex>

std::mutex mx;
std::condition_variable cv;
int counter = 0;

struct Inc
{
  Inc() { ++counter; }
  ~Inc() { ++counter; }
};


void func()
{
  std::unique_lock<std::mutex> lock{mx};
  std::notify_all_at_thread_exit(cv, std::move(lock));
#if _GLIBCXX_HAVE___CXA_THREAD_ATEXIT_IMPL
  // Correct order of thread_local destruction needs __cxa_thread_atexit_impl
  static thread_local Inc inc;
#else
  Inc inc;
#endif
}

int main()
{
  bool test __attribute__((unused)) = true;

  std::unique_lock<std::mutex> lock{mx};
  std::thread t{func};
  cv.wait(lock, [&]{ return counter == 2; });
  t.join();
}
