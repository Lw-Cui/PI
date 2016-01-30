#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "Operation.h"

void add(unsigned long long dest[], unsigned src[]) {
	for (int i = LEN - 1; i >= 0; i--)
		if (dest[i] >= BASE - src[i]) {
			assert(i && "Oh carry overflow!");
			dest[i] = (long long)dest[i] + src[i] - BASE;
			dest[i - 1]++;
		} else {
			dest[i] += src[i];
		}
}

void minus(unsigned long long dest[], unsigned src[]) {
	bool borrow = false;
	for (int i = LEN - 1; i >= 0; i--)
		if (dest[i] < src[i] + (borrow? 1: 0)) {
			assert(i && "Oh borrow overflow!");
			dest[i] = (unsigned long long)dest[i] + BASE - src[i] - (borrow? 1: 0);
			borrow = true;
		} else {
			dest[i] = dest[i] - src[i] - (borrow? 1: 0);
			borrow = false;
		}
}

void multiply(unsigned dest[], unsigned num) {
	unsigned carry = 0;
	for (int i = LEN - 1; i >= 0; i--) {
		unsigned long long tmp = (unsigned long long)num * dest[i] + carry;
		assert((tmp & (BASE - 1)) < BASE && "multiply truncation");
		dest[i] = (tmp & (BASE - 1));
		carry = tmp >> SHIFT;
		assert((i || !carry) && "Oh multiply overflow!");
	}
}

void divide(unsigned dest[], unsigned num) {
	unsigned long long s = 0;
	for (int i = 0; i < LEN; i++) {
		s <<= SHIFT;
		s += dest[i];
		if (s >= num) {
			assert(s / num < BASE && "divide truncation");
			dest[i] = s / num;
			s %= num;
		} else {
			dest[i] = 0;
		}
	}
}


void print(unsigned num) {
	for (int pos = SHIFT - 4; pos >= 0; pos -= 4)
		printf("%01X", (num >> pos) & 0xF);
}


void output(unsigned long long array[]) {
	printf("%llu.\n", array[0]);
	int digit = 0;
	for (int i = 1; digit < VALID; i++)
		for (int pos = SHIFT - 4; pos >= 0; pos -= 4) {
			printf("%01X", ((unsigned)array[i] >> pos) & 0xF);
			if (++digit % 64 == 0)
				printf("\n");
		}
	printf("\n");
}

void shift_right(unsigned sub[], unsigned num) {
	unsigned uint_bit = 8 * sizeof(unsigned);
	if (LEN * uint_bit <= num) {
		memset(sub, 0, sizeof(unsigned) * LEN);
		return;
	}
	if (num >= uint_bit) {
		memmove(sub + num / uint_bit, 
				sub, sizeof(unsigned) * LEN - num / uint_bit * sizeof(unsigned));
		memset(sub, 0, num / uint_bit * sizeof(unsigned));
	}
	unsigned mod = num % uint_bit;
	unsigned mask = (1 << mod) - 1;
	for (int i = LEN - 1; i > 0; i--) {
		sub[i] >>= mod;
		sub[i] |= (sub[i - 1] & mask) << (uint_bit - mod);
	}
	sub[0] >>= mod;
}

void Pi_carry(unsigned long long Pi[]) {
	for (int i = LEN - 1; i >= 0; i--)
		if (Pi[i] >= BASE) {
			assert(i && "long long Pi overflow.");
			Pi[i - 1] += (Pi[i] & ~(BASE - 1)) >> SHIFT;
			Pi[i] &= (BASE - 1);
		}
}
