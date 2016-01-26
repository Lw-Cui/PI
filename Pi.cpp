#include <cstdio>
#include <cstring>
#include <cassert>
const int shift = 32;
const unsigned long long BASE = (unsigned long long)1 << shift;
const int LEN = 10000000;

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
		carry = tmp >> shift;
		assert((i || !carry) && "Oh multiply overflow!");
	}
}

void divide(unsigned dest[], unsigned num) {
	unsigned long long s = 0;
	for (int i = 0; i < LEN; i++) {
		s <<= shift;
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
	for (int pos = 28; pos >= 0; pos -= 4)
		printf("%01X", (num >> pos) & 0xF);
}

void test_print() {
	unsigned tmp = 180150013;
	print(tmp);
}

inline void output(unsigned array[]) {
	printf("3.\n");
	for (int i = 1; i < LEN; i++)
		print(array[i]);
	printf("\n");
}

void shift_right(unsigned sub[], int num) {
	for (int i = LEN - 1; i >= 0; i++) {
	}
}

void cal_sub_Pi(unsigned Pi[], unsigned k) {
	unsigned *sub = new unsigned[LEN];
	memset(sub, 0, sizeof(unsigned) * LEN);

	sub[0] = 4;
	delete[] sub;
}

int main(int argc, char *argv[]) {
	test_add_minus();
	test_multiply_divide();
	test_print();
	return 0;
}
