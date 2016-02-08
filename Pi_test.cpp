#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Operation.h"
#include "gtest/gtest.h"
int VALID = 1000000;
int LEN = VALID / 8 + 2;

TEST(CarrayTest, simpleCarry) {
	unsigned long long *ptr = new unsigned long long[LEN];
	ptr[0] = 0;
	ptr[1] = 0xFFFFFFFFFF;
	Pi_carry(ptr);
	EXPECT_EQ(0xFF, ptr[0]);
	EXPECT_EQ(0xFFFFFFFF, ptr[1]);
	delete[] ptr;
}

TEST(DivideTest, equalTest) {
	unsigned *ptr = new unsigned[LEN];
	for (int i = 0; i < LEN; i++)
		ptr[i] = i;
	unsigned num = 123454321;
	multiply(ptr, num);
	divide(ptr, num);
	for (int i = 0; i < LEN; i++)
		EXPECT_EQ(i, ptr[i]);
	delete[] ptr;
}

TEST(AddTest, minus) {
	unsigned long long *ptr = new unsigned long long[LEN];
	unsigned *ptr2 = new unsigned[LEN];
	for (int i = 0; i < LEN; i++) {
		ptr[i] = LEN - i;
		ptr2[i] = ptr[i] + 1;
	}
	ptr[0] = 1, ptr2[0] = 0;

	minus(ptr, ptr2);
	EXPECT_EQ(0, ptr[0]);
	EXPECT_EQ(BASE - 1, ptr[LEN - 1]);
	for (int i = 1; i < LEN - 1; i++)
		EXPECT_EQ(BASE - 2, ptr[i]);

	add(ptr, ptr2);
	EXPECT_EQ(1, ptr[0]);
	for (int i = 1; i < LEN; i++)
		EXPECT_EQ(LEN - i, ptr[i]);
	delete[] ptr;
	delete[] ptr2;
}

TEST(ShiftTest, naturalNumSimple) {
	unsigned *sub = new unsigned[LEN];
	for (int i = 0; i < LEN; i++)
		sub[i] = 0xFFFFFFFF;
	shift_right(sub, sizeof(unsigned) * 8 * (LEN - 1) + 6);
	EXPECT_EQ(0x3FFFFFF, sub[LEN - 1]);
	for (int i = 0; i < LEN - 1; i++)
		EXPECT_EQ(0, sub[i]);
	delete[] sub;
}

TEST(ShiftTest, EightTimes) {
	unsigned *sub = new unsigned[LEN];
	sub[0] = 0;
	sub[1] = 0x19999999;
	sub[2] = 0x99999999;
	shift_right(sub, 8);
	EXPECT_EQ(0x00199999, sub[1]);
	EXPECT_EQ(0x99999999, sub[2]);
	delete[] sub;
}

TEST(ShiftTest, naturalNum) {
	unsigned *sub = new unsigned[LEN];
	for (int i = 0; i < LEN; i++)
		sub[i] = 0xABCDDCBE;
	shift_right(sub, sizeof(unsigned) * 8 * (LEN - 2) + 4);
	EXPECT_EQ(0xABCDDCB, sub[LEN - 2]);
	EXPECT_EQ(0xEABCDDCB, sub[LEN - 1]);
	for (int i = 0; i < LEN - 2; i++)
		EXPECT_EQ(0, sub[i]);
	delete[] sub;
}

/*
void test_print() {
	unsigned tmp = 180150013;
	print(tmp);
}
*/
