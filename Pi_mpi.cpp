#include <cstdio>
#include <cstdlib>
#include <mpi.h>
#include <cstring>
#include <cassert>
#include "Operation.h"
int VALID = 1000000;
int LEN = VALID / 8 + 2;

void cal_fraction(unsigned sub[], unsigned numerator, unsigned denominator,
		unsigned bit_num) {
	memset(sub, 0, sizeof(unsigned) * LEN);
	sub[0] = numerator;
	divide(sub, denominator);
	shift_right(sub, bit_num);
}

bool cal_sub_Pi(unsigned long long Pi[], unsigned k) {
	unsigned *sub = new unsigned[LEN];
	unsigned bit_num = 10 * k + 6;
	assert((unsigned long long)k * 10 + 9 < BASE && "sub Pi overflow.");

	cal_fraction(sub, 256, 10 * k + 1, bit_num);
	if (k & 1) minus(Pi, sub);
	else add(Pi, sub);

	bool zero = true;
	for (int i = 0; i < LEN; i++)
		if (sub[i] != 0) zero = false;
	if (zero) {
		delete[] sub;
		return false;
	}

	cal_fraction(sub, 32, 4 * k + 1, bit_num);
	if (k & 1) add(Pi,	sub);
	else minus(Pi, sub);

	cal_fraction(sub, 1, 4 * k + 3, bit_num);
	if (k & 1) add(Pi,	sub);
	else minus(Pi, sub);

	cal_fraction(sub, 64, 10 * k + 3, bit_num);
	if (k & 1) add(Pi,	sub);
	else minus(Pi, sub);

	cal_fraction(sub, 4, 10 * k + 5, bit_num);
	if (k & 1) add(Pi,	sub);
	else minus(Pi, sub);

	cal_fraction(sub, 4, 10 * k + 7, bit_num);
	if (k & 1) add(Pi,	sub);
	else minus(Pi, sub);

	cal_fraction(sub, 1, 10 * k + 9, bit_num);
	if (k & 1) minus(Pi, sub);
	else add(Pi, sub);

	delete[] sub;
	return true;

}

void Pi_carry(unsigned long long Pi[]) {
	for (int i = LEN - 1; i >= 0; i--)
		if (Pi[i] >= BASE) {
			assert(i && "long long Pi overflow.");
			Pi[i - 1] += (Pi[i] & ~(BASE - 1)) >> SHIFT;
			Pi[i] &= (BASE - 1);
		}
}

/*
   BBP formula
bool cal_sub_Pi(unsigned long long Pi[], unsigned k) {
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
	if (zero) {
		delete[] sub;
		return false;
	}

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
*/


#ifdef DEBUG
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
#endif

int main(int argc, char *argv[]) {
#ifdef DEBUG
	test_add_minus();
	test_multiply_divide();
	test_print();
	test_shift_right();
	test_carray();
#else
	double elapsed_time;
	MPI_Init(&argc, &argv);
	if (argc == 2) {
		VALID = atoi(argv[1]);
		LEN = VALID / 8 + 2;
	}
	unsigned long long *sub_Pi = new unsigned long long[LEN];
	unsigned long long *Pi = new unsigned long long[LEN];
	int compensation = 10;

	sub_Pi[0] = compensation;
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time = -MPI_Wtime();
	int size, id;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	for (unsigned k = id; cal_sub_Pi(sub_Pi, k); k += size);
	MPI_Reduce(sub_Pi, Pi, LEN, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	elapsed_time += MPI_Wtime();
	if (!id) {
		Pi[0] -= compensation * size;
		Pi_carry(Pi);
		output(Pi);
		printf("%.3fs\n", elapsed_time);
	}
	delete[] sub_Pi;
	delete[] Pi;
	MPI_Finalize();
#endif
	return 0;
}
