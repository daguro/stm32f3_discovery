/*
 * Copyright 2018 Daniel G. Robinson
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * @file micro_types.h
 * @brief some types used thorugh out code
 * @author Daniel G. Robinson
 * @date 2 Jul 2018
 */

#ifndef _MICRO_TYPES_H_
#define _MICRO_TYPES_H_

typedef union {
	uint64_t u64[2];
	int64_t s64[2];
	double d64[2];
	float f32[4];
	uint32_t u32[4];
	int32_t s32[4];
	uint16_t u16[8];
	int16_t s16[8];
	uint8_t u8[16];
	int8_t c8[16];
} Type128;

typedef union {
	int64_t s64;
	double d64;
	uint64_t u64;
	float f32[2];
	int32_t s32[2];
	uint32_t u32[2];
	int16_t s16[4];
	uint16_t u16[4];
	int8_t c8[8];
	uint8_t u8[8];
} Type64;

typedef union {
    float f32;
    int32_t s32;
    uint32_t u32;
    int16_t s16[2];
    uint16_t u16[2];
    int8_t c8[4];
    uint8_t u8[4];
} Type32;

#endif //  _MICRO_TYPES_H_
