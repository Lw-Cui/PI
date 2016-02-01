#include <cstdio>
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

const int compensation = 10;

void cal_device_Pi(unsigned long long Pi[], int thread, int start) {
    unsigned long long *tmp = new unsigned long long[thread * LEN];
    memset(tmp, 0, sizeof(unsigned long long) * thread * LEN);
    unsigned long long *sub[thread];

    for (int i = 0; i < thread; i++) {
        sub[i] = tmp + i * LEN;
        sub[i][0] += compensation;
    }

    bool flag = true;
#pragma omp parallel for firstprivate(flag) schedule(dynamic) num_threads(thread)
    for (unsigned k = start; k < (unsigned)VALID; k += 2) {
        if (!flag) continue;
        flag = cal_sub_Pi(sub[omp_get_thread_num()], k);
    }

    memset(Pi, 0, sizeof(unsigned long long) * LEN);
    int i = 0;
#pragma omp parallel for private(i)
    for (int j = 0; j < LEN; j++)
        for (i = 0; i < thread; i++)
            Pi[j] += sub[i][j];

    Pi[0] -= thread * compensation;
    delete[] tmp;
}

int main(int argc, char *argv[]) {
	double elapsed_time;
	if (argc == 2) {
		VALID = atoi(argv[1]);
		LEN = VALID / 8 + 2;
	}
	elapsed_time = -omp_get_wtime();
	unsigned long long Pi_mic[LEN], Pi_cpu[LEN];

    char signal_var;
#pragma offload target(mic:0) in(VALID, LEN) out(Pi_mic) \
    mandatory signal(&signal_var) 
    cal_device_Pi(Pi_mic, MIC, 1);
    cal_device_Pi(Pi_cpu, CPU, 0);
#pragma offload_wait target(mic:0) wait(&signal_var)

    for (int i = 0; i < LEN; i++)
        Pi_cpu[i] += Pi_mic[i];

    Pi_carry(Pi_cpu);
    //Pi_cpu[0] -= compensation * (CPU + MIC);

	elapsed_time += omp_get_wtime();
	output(Pi_cpu);
	printf("%.3fs\n", elapsed_time);
	return 0;
}
