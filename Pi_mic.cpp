
#include <cstdlib>
#include <omp.h>
#include <cstring>
#include <cassert>
#include "Operation.h"

int VALID = 1000000;
int LEN = VALID / 8 + 2;
const int MIC = 55, CPU = 20;

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
	if (argc == 2) {
		VALID = atoi(argv[1]);
		LEN = VALID / 8 + 2;
	}
	elapsed_time = -omp_get_wtime();

	unsigned long long compensation = 10;
	unsigned long long Pi[MIC][LEN], tmp[CPU][LEN];
	memset(Pi, 0, sizeof(Pi));
	memset(tmp, 0, sizeof(tmp));

	for (int i = 0; i < MIC; i++)
		Pi[i][0] += compensation;
	for (int i = 0; i < CPU; i++)
		tmp[i][0] += compensation;

#pragma omp parallel sections
{
#pragma omp section
	{
#pragma offload target(mic) inout(Pi)
		{
			bool flag = true;
#pragma omp parallel for firstprivate(flag) schedule(dynamic) num_threads(MIC)
			for (unsigned k = 0; k < (unsigned)VALID; k += 2) {
				if (!flag) continue;
				flag = cal_sub_Pi(Pi[omp_get_thread_num()], k);
			}

			int i = 1;
#pragma omp parallel for firstprivate(i)
			for (int j = 0; j < LEN; j++)
				for (i = 1; i < MIC; i++)
					Pi[0][j] += Pi[i][j];
		}
	}
#pragma omp section
	{
		bool flag = true;
#pragma omp parallel for firstprivate(flag) schedule(dynamic) num_threads(CPU)
		for (unsigned k = 1; k < (unsigned)VALID; k += 2) {
			if (!flag) continue;
			flag = cal_sub_Pi(tmp[omp_get_thread_num()], k);
		}

		int i = 1;
#pragma omp parallel for firstprivate(i)
		for (int j = 0; j < LEN; j++)
			for (i = 1; i < CPU; i++)
				tmp[0][j] += tmp[i][j];
		}
}

	Pi[0][0] -= compensation * MIC;
	Pi_carry(Pi[0]);
	tmp[0][0] -= compensation * CPU;
	Pi_carry(tmp[0]);
	add(Pi[0], tmp[0]);

	elapsed_time += omp_get_wtime();
	output(Pi[0]);
	printf("%.3fs\n", elapsed_time);
	return 0;
}
