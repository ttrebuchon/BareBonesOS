/* Definition of `errno' variable.  Canonical version.
   Copyright (C) 2002-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */



__thread int kerrno = 0;
extern __thread int __libc_kerrno __attribute__ ((alias ("kerrno")));

volatile int* __kerrno()
{
	return &kerrno;
}


void set_kerrno(int n)
{
	kerrno = n;
}

#ifndef TESTING

#include <errno.h>
#undef errno



__thread int errno;
extern __thread int __libc_errno __attribute__ ((alias ("errno")));

volatile int* __errno()
{
	return &errno;
}


void set_errno(int n)
{
	errno = n;
}

#endif