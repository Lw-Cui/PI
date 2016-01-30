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


int main(int argc, char *argv[]) {
	double elapsed_time;
	MPI_Init(&argc, &argv);
	if (argc == 2) {
		VALID = atoi(argv[1]);
		LEN = VALID / 8 + 2;
	}
	unsigned long long *sub_Pi = new unsigned long long[LEN];
	unsigned long long *Pi = new unsigned long long[LEN];
    memset(sub_Pi, 0, sizeof(unsigned long long) * LEN);
    memset(Pi, 0, sizeof(unsigned long long) * LEN);
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
	return 0;
}
