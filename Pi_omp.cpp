#include <cstdio>
#include <cstdlib>
#include <omp.h>
#include <cstring>
#include <ctime>
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
	int elapsed_time;
	if (argc == 2) {
		VALID = atoi(argv[1]);
		LEN = VALID / 8 + 2;
	}
	elapsed_time = -clock();

	int core = omp_get_num_procs();
	omp_set_num_threads(core);

	unsigned long long compensation = 10;
	unsigned long long Pi[core][LEN];
	memset(Pi, 0, sizeof(Pi));
	for (int i = 0; i < core; i++)
		Pi[i][0] += compensation;

	bool flag = true;
#pragma omp parallel for firstprivate(flag) schedule(dynamic)
	for (unsigned k = 0; k < VALID; k++) {
		if (!flag) continue;
		flag = cal_sub_Pi(Pi[omp_get_thread_num()], k);
	}

#pragma omp parallel for private(i)
	for (int j = 0; j < LEN; j++)
		for (int i = 1; i < core; i++)
			Pi[0][j] += Pi[i][j];

	Pi[0][0] -= compensation * core;
	Pi_carry(Pi[0]);

	elapsed_time += clock();

	output(Pi[0]);
	printf("%.3fs\n", (double)elapsed_time / CLOCKS_PER_SEC);
	return 0;
}
