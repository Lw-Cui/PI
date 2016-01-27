#include <cstdio>
#include <cstring>
#include <cassert>
const int SHIFT = 32;
const unsigned long long BASE = (unsigned long long)1 << SHIFT;
const int LEN = 125002;
const int VALID = LEN - 1;

void add(unsigned dest[], unsigned src[]) {
	for (int i = LEN - 1; i >= 0; i--)
		if (dest[i] >= BASE - src[i]) {
			assert(i && "Oh carry overflow!");
			dest[i] = (long long)dest[i] + src[i] - BASE;
			dest[i - 1]++;
		} else {
			dest[i] += src[i];
		}
}

void minus(unsigned dest[], unsigned src[]) {
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

void test_multiply_divide() {
	unsigned *ptr = new unsigned[LEN];
	for (int i = 0; i < LEN; i++)
		ptr[i] = i;
	unsigned num = 123454321;
	multiply(ptr, num);
	divide(ptr, num);
	for (size_t i = 0; i < LEN; i++)
		assert(ptr[i] == i && "multiply or divide error.");
	delete[] ptr;
}

void test_add_minus() {
	unsigned *ptr = new unsigned[LEN], *ptr2 = new unsigned[LEN];
	for (size_t i = 0; i < LEN; i++) {
		ptr[i] = LEN - i;
		ptr2[i] = ptr[i] + 1;
	}
	ptr[0] = 1, ptr2[0] = 0;

	minus(ptr, ptr2);
	assert(ptr[0] == 0 && "ptr[0] minus error.");
	assert(ptr[LEN - 1] == BASE - 1 && "ptr[LEN - 1] minus error.");
	for (int i = 1; i < LEN - 1; i++)
		assert(ptr[i] == BASE - 2 && "substraction error.");

	add(ptr, ptr2);
	assert(ptr[0] == 1 && "ptr[0] add error.");
	for (size_t i = 1; i < LEN; i++)
		assert(ptr[i] == LEN - i && "addition error.");
	delete[] ptr;
	delete[] ptr2;
}


inline void print(unsigned num) {
	for (int pos = SHIFT - 4; pos >= 0; pos -= 4)
		printf("%01X", (num >> pos) & 0xF);
}

void test_print() {
	unsigned tmp = 180150013;
	print(tmp);
}

inline void output(unsigned array[]) {
	printf("%d.\n", array[0]);
	unsigned digit = 0;
	for (int i = 1; i < VALID; i++)
		for (int pos = SHIFT - 4; pos >= 0; pos -= 4) {
			printf("%01X", (array[i] >> pos) & 0xF);
			if (++digit == 64) {
				printf("\n");
				digit = 0;
			}
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

void test_shift_right() {
	unsigned *sub = new unsigned[LEN];
	for (int i = 0; i < LEN; i++)
		sub[i] = 0xFFFFFFFF;
	shift_right(sub, sizeof(unsigned) * 8 * (LEN - 1) + 6);
	assert(sub[LEN - 1] == 0x3FFFFFF && "shift error.");
	for (int i = 0; i < LEN - 1; i++)
		assert(sub[i] == 0 && "shift error.");

	for (int i = 0; i < LEN; i++)
		sub[i] = 0xABCDDCBE;
	shift_right(sub, sizeof(unsigned) * 8 * (LEN - 2) + 4);
	assert(sub[LEN - 2] == 0xABCDDCB && "shift error.");
	assert(sub[LEN - 1] == 0xEABCDDCB  && "shift error.");
	for (int i = 0; i < LEN - 2; i++)
		assert(sub[i] == 0 && "shift error.");
	delete[] sub;

	sub = new unsigned[3];
	sub[0] = 0;
	sub[1] = 0x19999999;
	sub[2] = 0x99999999;
	shift_right(sub, 8);
	assert(sub[1] == 0x00199999 && "shift error!");
	assert(sub[2] == 0x99999999 && "shift error!");
	delete[] sub;
}

bool cal_sub_Pi(unsigned Pi[], unsigned k) {
	unsigned *sub = new unsigned[LEN];
	assert((unsigned long long)k * 8 + 6 < BASE && "sub Pi overflow.");

	memset(sub, 0, sizeof(unsigned) * LEN);
	sub[0] = 4;
	divide(sub, 8 * k + 1);
	shift_right(sub, 4 * k);
	add(Pi,	sub);

	bool zero = true;
	for (int i = 0; i < LEN; i++)
		if (sub[i] != 0)
			zero = false;
	if (zero)
		return false;

	memset(sub, 0, sizeof(unsigned) * LEN);
	sub[0] = 2;
	divide(sub, 8 * k + 4);
	shift_right(sub, 4 * k);
	minus(Pi, sub);

	memset(sub, 0, sizeof(unsigned) * LEN);
	sub[0] = 1;
	divide(sub, 8 * k + 5);
	shift_right(sub, 4 * k);
	minus(Pi, sub);

	memset(sub, 0, sizeof(unsigned) * LEN);
	sub[0] = 1;
	divide(sub, 8 * k + 6);
	shift_right(sub, 4 * k);
	minus(Pi, sub);

	delete[] sub;
	return true;
}


int main(int argc, char *argv[]) {
	/*
	test_add_minus();
	test_multiply_divide();
	test_print();
	test_shift_right();
	*/
	unsigned *Pi = new unsigned[LEN];
	for (unsigned k = 0; cal_sub_Pi(Pi, k); k++);
	//output(Pi);
	delete[] Pi;
	return 0;
}
