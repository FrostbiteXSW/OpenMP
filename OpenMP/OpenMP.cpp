#include "stdafx.h"

#define MAX_SIZE 2000
#define MAX_NUM 1000
#define MAX_THREADS 8

void test(int num_threads) {
	omp_set_num_threads(num_threads);

	default_random_engine rand;
	rand.seed((unsigned long)clock());
	uniform_int_distribution<unsigned> dist(1, MAX_NUM);

	int **a = new int*[MAX_SIZE], **b = new int*[MAX_SIZE], **c = new int*[MAX_SIZE];

#pragma omp parallel for shared(a) shared(b) shared(c)
	for (int i = 0; i < MAX_SIZE; ++i) {
		a[i] = new int[MAX_SIZE];
		b[i] = new int[MAX_SIZE];
		c[i] = new int[MAX_SIZE] {0};
		for (int j = 0; j < MAX_SIZE; ++j) {
			a[i][j] = dist(rand);
			b[i][j] = dist(rand);
		}
	}

#pragma omp parallel for shared(a) shared(b) shared(c)
	for (int i = 0; i < MAX_SIZE; ++i) {
		for (int j = 0; j < MAX_SIZE; ++j) {
			for (int k = 0; k < MAX_SIZE; ++k) {
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
}

int main(int argc, char* argv) {
	cout << "Max thread limit is " << omp_get_max_threads() << "." << endl << endl;
	for (int i = 1; i <= MAX_THREADS; ++i) {
		auto start = system_clock::now();
		test(i);
		cout << "Processed in " << i << " thread(s) uses " << duration_cast<milliseconds>(system_clock::now() - start).count() << " milliseconds." << endl;
	}
	return 0;
}