#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "Operation.h"
int VALID = 1000000;
int LEN = VALID / 8 + 2;

void test_carray() {
	unsigned long long *ptr = new unsigned long long[LEN];
	ptr[0] = 0;
	ptr[1] = 0xFFFFFFFFFF;
	Pi_carry(ptr);
	assert(ptr[0] == 0xFF && "carry error.");
	assert(ptr[1] == 0xFFFFFFFF && "carry error.");
	delete[] ptr;
}

void test_multiply_divide() {
	unsigned *ptr = new unsigned[LEN];
	for (int i = 0; i < LEN; i++)
		ptr[i] = i;
	unsigned num = 123454321;
	multiply(ptr, num);
	divide(ptr, num);
	for (int i = 0; i < LEN; i++)
		assert(ptr[i] == (unsigned)i && "multiply or divide error.");
	delete[] ptr;
}

void test_add_minus() {
	unsigned long long *ptr = new unsigned long long[LEN];
	unsigned *ptr2 = new unsigned[LEN];
	for (int i = 0; i < LEN; i++) {
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
	for (int i = 1; i < LEN; i++)
		assert(ptr[i] == (unsigned)LEN - i && "addition error.");
	delete[] ptr;
	delete[] ptr2;
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

	sub[0] = 0;
	sub[1] = 0x19999999;
	sub[2] = 0x99999999;
	shift_right(sub, 8);
	assert(sub[1] == 0x00199999 && "shift error!");
	assert(sub[2] == 0x99999999 && "shift error!");
	delete[] sub;
}

void test_print() {
	unsigned tmp = 180150013;
	print(tmp);
}

int main(int argc, char *argv[]) {
	test_add_minus();
	test_multiply_divide();
	test_print();
	test_shift_right();
	test_carray();
	return 0;
}
