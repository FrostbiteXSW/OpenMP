#include "stdafx.h"

#define MAT_SIZE 100	// 矩阵大小
#define ITERATION 10000	// 计算迭代次数
#define MAX_NUM 1000	// 矩阵元素最大值
#define MAX_THREADS 16	// 最大线程数量

void OmpCalculation(const int num_threads, const int mat_size, const int iteration) {
	// 设置线程总数
	omp_set_num_threads(num_threads);

	// 设置随机因子
	default_random_engine rand;
	rand.seed(static_cast<unsigned long>(clock()));
	const uniform_int_distribution<unsigned> dist(1, MAX_NUM);

	// 创建矩阵空间
	const auto a = new int*[mat_size], b = new int*[mat_size], c = new int*[mat_size];
#pragma omp parallel for shared(a) shared(b) shared(c)
	for (auto i = 0; i < mat_size; ++i) {
		a[i] = new int[mat_size];
		b[i] = new int[mat_size];
		c[i] = new int[mat_size]{0};
	}
	
	// 进行多线程计算
	const auto base = mat_size * mat_size;
	for (auto iter = 0; iter < iteration; ++iter) {
		// 随机生成矩阵元素
#pragma omp parallel for shared(a) shared(b)
		for (auto i = 0; i < mat_size * mat_size; ++i) {
			a[i / mat_size][i % mat_size] = dist(rand);
			b[i / mat_size][i % mat_size] = dist(rand);
		}

		// 计算矩阵乘积
#pragma omp parallel for shared(a) shared(b) shared(c)
		for (auto n = 0; n < mat_size * mat_size * mat_size; ++n) {
			const auto i = n / base;
			const auto j = n % base / mat_size;
			const auto k = n % mat_size;
			c[i][j] += a[i][k] * b[k][j];
		}
	}

	// delete分行写才能完全释放各个变量
	delete[] a;
	delete[] b;
	delete[] c;
}

int main() {
	// 输出可能最大线程数
	cout << "Max thread limit is " << omp_get_max_threads() << "." << endl << endl;

	const auto timeList = new int[MAX_THREADS] {0};

	// 线程数从1至MAX_THREADS执行矩阵乘法运算进行对比
	for (auto i = 1; i <= MAX_THREADS; ++i) {
		// 记录开始时间
		const auto start = system_clock::now();

		// 多次实验取平均值
		for (auto j = 0; j <= 10; ++j) {
			OmpCalculation(i, MAT_SIZE, ITERATION);
			timeList[i - 1] += duration_cast<milliseconds>(system_clock::now() - start).count();
		}
		timeList[i - 1] /= 10;
		
		// 输出平均用时
		cout << "Processed in " << i << " thread(s) uses " << timeList[i - 1] << " milliseconds." << endl;
	}

	// 打印时间列表
	cout << endl;
	for (auto i = 0; i < MAX_THREADS; ++i) {
		cout << timeList[i] << (i == MAX_THREADS - 1 ? " " : ", ");
	}
	cout << endl << endl;
	system("pause");
	
	delete[] timeList;
	return 0;
}
