#include "stdafx.h"

#define MAX_SIZE 2000	// 矩阵大小
#define MAX_NUM 1000	// 矩阵元素最大值
#define MAX_THREADS 16	// 最大线程数量

void Test(const int num_threads)
{
	// 设置线程总数
	omp_set_num_threads(num_threads);

	// 设置随机因子
	default_random_engine rand;
	rand.seed(static_cast<unsigned long>(clock()));
	const uniform_int_distribution<unsigned> dist(1, MAX_NUM);

	// 创建矩阵空间
	const auto a = new int*[MAX_SIZE], b = new int*[MAX_SIZE], c = new int*[MAX_SIZE];

	// 随机生成矩阵元素
#pragma omp parallel for shared(a) shared(b) shared(c)
	for (auto i = 0; i < MAX_SIZE; ++i)
	{
		a[i] = new int[MAX_SIZE];
		b[i] = new int[MAX_SIZE];
		c[i] = new int[MAX_SIZE] {0};
		for (auto j = 0; j < MAX_SIZE; ++j)
		{
			a[i][j] = dist(rand);
			b[i][j] = dist(rand);
		}
	}

	// 计算矩阵乘积
#pragma omp parallel for shared(a) shared(b) shared(c)
	for (auto i = 0; i < MAX_SIZE; ++i)
	{
		for (auto j = 0; j < MAX_SIZE; ++j)
		{
			for (auto k = 0; k < MAX_SIZE; ++k)
			{
				c[i][j] += a[i][k] * b[k][j];
			}
		}
	}
	
	// delete分行写才能完全释放各个变量
	for (auto i = 0; i < MAX_SIZE; ++i) {
		delete[] a[i];
		delete[] b[i];
		delete[] c[i];
	}
	delete[] a;
	delete[] b;
	delete[] c;
}

int main()
{
	// 输出可能最大线程数
	cout << "Max thread limit is " << omp_get_max_threads() << "." << endl << endl;

	// 线程数从1至MAX_THREADS执行矩阵乘法运算进行对比
	for (auto i = 1; i <= MAX_THREADS; ++i)
	{
		// 记录开始时间
		const auto start = system_clock::now();
		
		Test(i);

		// 输出总共用时
		cout << "Processed in " << i << " thread(s) uses " << duration_cast<milliseconds>(system_clock::now() - start).count()
			 << " milliseconds." << endl;
	}
	return 0;
}
