/******************************************************************************
 * funcs_arm.h
 *
 * All Test Functions in 64-bit ARM assembly code: they are codenamed as
 * Scan/Perm Read/Write 64/128 Ptr/Index Simple/Unroll Loop.
 *
 * Scan = consecutive scanning, Perm = walk permutation cycle.
 * Read/Write = obvious
 * 64/128 = size of access
 * Ptr = with pointer, Index = access as array[i]
 * Simple/Unroll = 1 or 16 operations per loop,
 *     Multi = ARM multi-register operations are not available under 64-bit 
 *
 ******************************************************************************
 * Copyright (C) 2013 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

// ****************************************************************************
// ----------------------------------------------------------------------------
// 64-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 64-bit writer in a simple loop (C version)
void cScanWrite64PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;
    uint64_t* end = begin + size / sizeof(uint64_t);
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    do {
        uint64_t* p = begin;
        do {
            *p++ = value;
        }
        while (p < end);
    }
    while (--repeats != 0);
}

//REGISTER(cScanWrite64PtrSimpleLoop, 8, 8, 1);

// 64-bit writer in a simple loop (Assembler version)
void ScanWrite64PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of write loop
        "str    %[value], [x16], #8 \n"  // store and advance 8
        // test write loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [value] "r" (value), [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "cc", "memory");
}

REGISTER(ScanWrite64PtrSimpleLoop, 8, 8, 1);

// 64-bit writer in an unrolled loop (Assembler version)
void ScanWrite64PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of write loop
        "str    %[value], [x16,#0*8] \n"
        "str    %[value], [x16,#1*8] \n"
        "str    %[value], [x16,#2*8] \n"
        "str    %[value], [x16,#3*8] \n"

        "str    %[value], [x16,#4*8] \n"
        "str    %[value], [x16,#5*8] \n"
        "str    %[value], [x16,#6*8] \n"
        "str    %[value], [x16,#7*8] \n"

        "str    %[value], [x16,#8*8] \n"
        "str    %[value], [x16,#9*8] \n"
        "str    %[value], [x16,#10*8] \n"
        "str    %[value], [x16,#11*8] \n"

        "str    %[value], [x16,#12*8] \n"
        "str    %[value], [x16,#13*8] \n"
        "str    %[value], [x16,#14*8] \n"
        "str    %[value], [x16,#15*8] \n"

        "add    x16, x16, #16*8 \n"       // add offset
        // test write loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [value] "r" (value), [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "cc", "memory");
}

REGISTER(ScanWrite64PtrUnrollLoop, 8, 8, 16);

// 64-bit read in a simple loop (Assembler version)
void ScanRead64PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of read loop
        "ldr    x0, [x16], #8 \n"        // retrieve and advance 8
        // test read loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x0", "cc", "memory");
}

REGISTER(ScanRead64PtrSimpleLoop, 8, 8, 1);

// 64-bit reader in an unrolled loop (Assembler version)
void ScanRead64PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of read loop
        "ldr    x0, [x16,#0*8] \n"
        "ldr    x0, [x16,#1*8] \n"
        "ldr    x0, [x16,#2*8] \n"
        "ldr    x0, [x16,#3*8] \n"

        "ldr    x0, [x16,#4*8] \n"
        "ldr    x0, [x16,#5*8] \n"
        "ldr    x0, [x16,#6*8] \n"
        "ldr    x0, [x16,#7*8] \n"

        "ldr    x0, [x16,#8*8] \n"
        "ldr    x0, [x16,#9*8] \n"
        "ldr    x0, [x16,#10*8] \n"
        "ldr    x0, [x16,#11*8] \n"

        "ldr    x0, [x16,#12*8] \n"
        "ldr    x0, [x16,#13*8] \n"
        "ldr    x0, [x16,#14*8] \n"
        "ldr    x0, [x16,#15*8] \n"

        "add    x16, x16, #16*8 \n"       // add offset
        // test read loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x0", "cc", "memory");
}

REGISTER(ScanRead64PtrUnrollLoop, 8, 8, 16);

// -----------------------------------------------------------------------------

// 64-bit writer in an indexed loop (C version)
void cScanWrite64IndexSimpleLoop(char* _memarea, size_t _size, size_t repeats)
{
    uint64_t* memarea = (uint64_t*)_memarea;
    uint64_t size = _size / sizeof(uint64_t);
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    do {
        for (size_t i = 0; i < size; ++i)
            memarea[i] = value;
    }
    while (--repeats != 0);
}

//REGISTER(cScanWrite64IndexSimpleLoop, 8, 8, 1);

// 64-bit writer in an indexed loop (Assembler version)
void ScanWrite64IndexSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, #0 \n"              // x16 = reset index
        "2: \n" // start of write loop
        "str    %[value], [%[memarea], x16] \n" // store and advance 8
        "add    x16, x16, #8 \n"
        // test write loop condition
        "cmp    x16, %[size] \n"         // compare to total size
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [value] "r" (value), [memarea] "r" (memarea), [size] "r" (size)
        : "x16", "cc", "memory");
}

REGISTER(ScanWrite64IndexSimpleLoop, 8, 8, 1);

// 64-bit reader in an indexed loop (Assembler version)
void ScanRead64IndexSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, #0 \n"              // x16 = reset index
        "2: \n" // start of read loop
        "ldr    x0, [%[memarea], x16] \n" // store and advance 8
        "add    x16, x16, #8 \n"
        // test read loop condition
        "cmp    x16, %[size] \n"         // compare to total size
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea), [size] "r" (size)
        : "x16", "x0", "cc", "memory");
}

REGISTER(ScanRead64IndexSimpleLoop, 8, 8, 1);

// ****************************************************************************
// ----------------------------------------------------------------------------
// 128-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 64-bit writer in a simple loop (C version)
void cScanWrite128PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    typedef std::pair<uint64_t,uint64_t> uint128;

    uint128* begin = (uint128*)memarea;
    uint128* end = begin + size / sizeof(uint128);
    uint64_t val64 = 0xFAEE00C0FFEEEEEE;
    uint128 value = uint128(val64,val64);

    do {
        uint128* p = begin;
        do {
            *p++ = value;
        }
        while(p < end);
    }
    while (--repeats != 0);
}

//REGISTER(cScanWrite128PtrSimpleLoop, 16, 16, 1);

// 128-bit writer in a simple loop (Assembler version)
void ScanWrite128PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    asm volatile(
        "mov    v4.d[0], %[value] \n"        // v4 = 128-bit value
        "mov    v4.d[1], %[value] \n"
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of write loop
        "str   q4, [x16], #16 \n"        // store and advance 16
        // test write loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [value] "r" (value), [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x4", "q4", "cc", "memory");
}

REGISTER(ScanWrite128PtrSimpleLoop, 16, 16, 1);


// 128-bit writer in an unrolled loop (Assembler version)
void ScanWrite128PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    asm volatile(
        "mov    v4.d[0], %[value] \n"        // v4 = 128-bit value
        "mov    v4.d[1], %[value] \n"
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of write loop
        "str   q4, [x16,#0*16] \n"
        "str   q4, [x16,#1*16] \n"
        "str   q4, [x16,#2*16] \n"
        "str   q4, [x16,#3*16] \n"

        "str   q4, [x16,#4*16] \n"
        "str   q4, [x16,#5*16] \n"
        "str   q4, [x16,#6*16] \n"
        "str   q4, [x16,#7*16] \n"

        "str   q4, [x16,#8*16] \n"
        "str   q4, [x16,#9*16] \n"
        "str   q4, [x16,#10*16] \n"
        "str   q4, [x16,#11*16] \n"

        "str   q4, [x16,#12*16] \n"
        "str   q4, [x16,#13*16] \n"
        "str   q4, [x16,#14*16] \n"
        "str   q4, [x16,#15*16] \n"

        "add    x16, x16, #16*16 \n"
        // test write loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [value] "r" (value), [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x4", "x5", "cc", "memory");
}

REGISTER(ScanWrite128PtrUnrollLoop, 16, 16, 16);


// 128-bit reader in a simple loop (Assembler version)
void ScanRead128PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of read loop
        "ldr   q4, [x16], #16 \n"        // retrieve and advance 16
        // test read loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x4", "x5", "cc", "memory");
}

REGISTER(ScanRead128PtrSimpleLoop, 16, 16, 1);

// 128-bit reader in an unrolled loop (Assembler version)
void ScanRead128PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of read loop
        "ldr   q4, [x16,#0*16] \n"
        "ldr   q4, [x16,#1*16] \n"
        "ldr   q4, [x16,#2*16] \n"
        "ldr   q4, [x16,#3*16] \n"

        "ldr   q4, [x16,#4*16] \n"
        "ldr   q4, [x16,#5*16] \n"
        "ldr   q4, [x16,#6*16] \n"
        "ldr   q4, [x16,#7*16] \n"

        "ldr   q4, [x16,#8*16] \n"
        "ldr   q4, [x16,#9*16] \n"
        "ldr   q4, [x16,#10*16] \n"
        "ldr   q4, [x16,#11*16] \n"

        "ldr   q4, [x16,#12*16] \n"
        "ldr   q4, [x16,#13*16] \n"
        "ldr   q4, [x16,#14*16] \n"
        "ldr   q4, [x16,#15*16] \n"

        "add    x16, x16, #16*16 \n"
        // test read loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x4", "x5", "cc", "memory");
}

REGISTER(ScanRead128PtrUnrollLoop, 16, 16, 16);
// ****************************************************************************
// ----------------------------------------------------------------------------
// 256-bit Operations
// ----------------------------------------------------------------------------
// ****************************************************************************

// 256-bit writer in a simple loop (Assembler version)
void ScanWrite256PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    asm volatile(
        "mov    v4.d[0], %[value] \n"        // v4 = 128-bit value
        "mov    v4.d[1], %[value] \n"
        "mov    v5.d[0], %[value] \n"        // v5 = 128-bit value
        "mov    v5.d[1], %[value] \n"
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of write loop
        "stnp   q4, q5, [x16] \n"        // store
	"add x16,x16, #32\n"		//advance 32. Not directly possible with stnp
        // test write loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [value] "r" (value), [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x4", "q4", "cc", "memory");
}

REGISTER(ScanWrite256PtrSimpleLoop, 32, 32, 1);

// 256-bit writer in an unrolled loop (Assembler version)
void ScanWrite256PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    uint64_t value = 0xFAEE00C0FFEEEEEE;

    asm volatile(
        "mov    v4.d[0], %[value] \n"        // v4 = 128-bit value
        "mov    v4.d[1], %[value] \n"
        "mov    v5.d[0], %[value] \n"        // v5 = 128-bit value
        "mov    v5.d[1], %[value] \n"
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of write loop
        "stnp   q4, q5, [x16,#0*32] \n"
        "stnp   q4, q5, [x16,#1*32] \n"
        "stnp   q4, q5, [x16,#2*32] \n"
        "stnp   q4, q5, [x16,#3*32] \n"

        "stnp   q4, q5, [x16,#4*32] \n"
        "stnp   q4, q5, [x16,#5*32] \n"
        "stnp   q4, q5, [x16,#6*32] \n"
        "stnp   q4, q5, [x16,#7*32] \n"

        "stnp   q4, q5, [x16,#8*32] \n"
        "stnp   q4, q5, [x16,#9*32] \n"
        "stnp   q4, q5, [x16,#10*32] \n"
        "stnp   q4, q5, [x16,#11*32] \n"

        "stnp   q4, q5, [x16,#12*32] \n"
        "stnp   q4, q5, [x16,#13*32] \n"
        "stnp   q4, q5, [x16,#14*32] \n"
        "stnp   q4, q5, [x16,#15*32] \n"

        "add    x16, x16, #16*32 \n"
        // test write loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [value] "r" (value), [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x4", "x5", "cc", "memory");
}

REGISTER(ScanWrite256PtrUnrollLoop, 32, 32, 16);

// 256-bit reader in a simple loop (Assembler version)
void ScanRead256PtrSimpleLoop(char* memarea, size_t size, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of read loop
        "ldnp   q4, q5, [x16] \n"        // retrieve
	"add x16,x16, #32\n"		//advance 32. Not directly possible with ldnp
        // test read loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x4", "x5", "cc", "memory");
}

REGISTER(ScanRead256PtrSimpleLoop, 32, 32, 1);

// 256-bit reader in an unrolled loop (Assembler version)
void ScanRead256PtrUnrollLoop(char* memarea, size_t size, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset loop iterator
        "2: \n" // start of read loop
        "ldnp   q4, q5, [x16,#0*32] \n"
        "ldnp   q4, q5, [x16,#1*32] \n"
        "ldnp   q4, q5, [x16,#2*32] \n"
        "ldnp   q4, q5, [x16,#3*32] \n"

        "ldnp   q4, q5, [x16,#4*32] \n"
        "ldnp   q4, q5, [x16,#5*32] \n"
        "ldnp   q4, q5, [x16,#6*32] \n"
        "ldnp   q4, q5, [x16,#7*32] \n"

        "ldnp   q4, q5, [x16,#8*32] \n"
        "ldnp   q4, q5, [x16,#9*32] \n"
        "ldnp   q4, q5, [x16,#10*32] \n"
        "ldnp   q4, q5, [x16,#11*32] \n"

        "ldnp   q4, q5, [x16,#12*32] \n"
        "ldnp   q4, q5, [x16,#13*32] \n"
        "ldnp   q4, q5, [x16,#14*32] \n"
        "ldnp   q4, q5, [x16,#15*32] \n"

        "add    x16, x16, #16*32 \n"
        // test read loop condition
        "cmp    x16, %[end] \n"          // compare to end iterator
        "blo    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea), [end] "r" (memarea+size)
        : "x16", "x4", "x5", "cc", "memory");
}

REGISTER(ScanRead256PtrUnrollLoop, 32, 32, 16);

// ****************************************************************************
// ----------------------------------------------------------------------------
// Permutation Walking
// ----------------------------------------------------------------------------
// ****************************************************************************

// follow 64-bit permutation in a simple loop (C version)
void cPermRead64SimpleLoop(char* memarea, size_t, size_t repeats)
{
    uint64_t* begin = (uint64_t*)memarea;

    do {
        uint64_t* p = begin;
        do {
            p = (uint64_t*)*p;
        }
        while (p != begin);
    }
    while (--repeats != 0);
}

//REGISTER_PERM(cPermRead64SimpleLoop, 8);

// follow 64-bit permutation in a simple loop (Assembler version)
void PermRead64SimpleLoop(char* memarea, size_t, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset iterator
        "2: \n" // start of loop
        "ldr    x16, [x16] \n"
        // test loop condition
        "cmp    x16, %[memarea] \n"      // compare to end iterator
        "bne    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea)
        : "x16", "cc", "memory");
}

REGISTER_PERM(PermRead64SimpleLoop, 8);

// follow 64-bit permutation in an unrolled loop (Assembler version)
void PermRead64UnrollLoop(char* memarea, size_t, size_t repeats)
{
    asm volatile(
        "1: \n" // start of repeat loop
        "mov    x16, %[memarea] \n"      // x16 = reset iterator
        "2: \n" // start of loop
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"

        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"

        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"

        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        "ldr    x16, [x16] \n"
        // test loop condition
        "cmp    x16, %[memarea] \n"      // compare to end iterator
        "bne    2b \n"
        // test repeat loop condition
        "subs   %[repeats], %[repeats], #1 \n" // until repeats = 0
        "bne    1b \n"
        : [repeats] "+r" (repeats)
        : [memarea] "r" (memarea)
        : "x16", "cc", "memory");
}

REGISTER_PERM(PermRead64UnrollLoop, 8);

// -----------------------------------------------------------------------------
