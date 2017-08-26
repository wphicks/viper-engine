/*
** Viper Engine - Copyright (C) 2016 Velan Studios - All Rights Reserved
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to
** deal in the Software without restriction, including without limitation the
** rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
** sell copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
** IN THE SOFTWARE.
*/

#include "thread/vatomic.h"

int32_t vatomic32_compare_exchange(int32_t* store, int32_t comp, int32_t value)
{
	return __sync_val_compare_and_swap((volatile long*)store, comp, value);
}

int32_t vatomic32_exchange(int32_t* store, int32_t value)
{
	return __sync_swap((volatile long*)store, value);
}

int32_t vatomic32_exchange_add(int32_t* store, int32_t value)
{
	return __sync_fetch_and_add((volatile long*)store, value);
}

int32_t vatomic32_increment(int32_t* store)
{
	return __sync_fetch_and_add((volatile long*)store, 1);
}

int32_t vatomic32_decrement(int32_t* store)
{
	return __sync_fetch_and_add((volatile long*)store, -1);
}

int64_t vatomic64_compare_exchange(int64_t* store, int64_t comp, int64_t value)
{
	return __sync_val_compare_and_swap((volatile long*)store, comp, value);
}

int64_t vatomic64_exchange(int64_t* store, int64_t value)
{
	return __sync_swap((volatile long*)store, value);
}

int64_t vatomic64_exchange_add(int64_t* store, int64_t value)
{
	return __sync_fetch_and_add((volatile long*)store, value);
}

int64_t vatomic64_increment(int64_t* store)
{
	return __sync_fetch_and_add((volatile long*)store, 1);
}

int64_t vatomic64_decrement(int64_t* store)
{
	return __sync_fetch_and_add((volatile long*)store, -1);
}

void vatomic_barrier()
{
    /* __sync_synchronize? */
    __sync_synchronize();
	/*int32_t unused = 0;
	vatomic32_exchange(&unused, 1);*/
}
