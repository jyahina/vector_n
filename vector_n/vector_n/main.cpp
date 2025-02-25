// vector_n.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "vector_n.h"
#include <omp.h>
#include <ctime>
#include <cassert>


typedef std::vector<int> vec_int;
typedef std::vector<vec_int> vec2_int;
typedef std::vector<vec2_int> vec3_int;

void testVector4d()
{
	int nx = 200, ny = 60, nz = 70, nt = 200;

	vector_n<int, 4> a(nx, ny, nz, nt);
	std::vector<std::vector<std::vector<vec_int>>> vec4;
	vec4.resize(nx, vec3_int(ny, vec2_int(nz, vec_int(nt))));
	std::vector<int> expected_data;

	size_t start_time_new = clock(); // начальное время

//#pragma omp parallel for
	for (int i = 0, n = 0; i < nx; ++i)
	{
		for (int j = 0; j < ny; ++j)
		{
			for (int k = 0; k < nz; ++k)
			{
				for (int g = 0; g < nt; ++g)
				{
					//expected_data.push_back(n);
					a(i, j, k, g) = i + j + k + g;// n;
					//++n;
				}
			}
		}
	}

	size_t search_time = clock() - start_time_new; // искомое в

	std::cout << "TIME 1-DIM VECTOR = " << search_time << std::endl;

	size_t start_time_vec = clock(); // начальное время
//#pragma omp parallel for
	for (int i = 0, n = 0; i < nx; ++i)
	{
		for (int j = 0; j < ny; ++j)
		{
			for (int k = 0; k < nz; ++k)
			{
				for (int g = 0; g < nt; ++g)
				{
					vec4[i][j][k][g] = i + j + k + g;// ++n;
				}
			}
		}
	}

	size_t time_vec = clock() - start_time_vec; // искомое в
	std::cout << "TIME 4-DIM VECTOR = " << time_vec << std::endl;
	std::cout << "TOTAL AMOUNT = " << nx * ny * nz * nt << std::endl;
	std::cout << std::endl;
}

void testVector2d()
{
	int nx = 40000, ny = 4200;

	vector_n<int, 2> a(nx, ny);
	std::vector<vec_int> vec4;
	vec4.resize(nx, vec_int(ny));

	size_t start_time_new = clock(); // начальное время
	for (int i = 0, n = 0; i < nx; ++i)
	{
		for (int j = 0; j < ny; ++j)
		{
			a(i, j) = n;
			++n;
		}
	}

	size_t search_time = clock() - start_time_new; // искомое в

	std::cout << "TIME 1-DIM VECTOR = " << search_time << std::endl;

	size_t start_time_vec = clock(); // начальное время
	for (int i = 0, n = 0; i < nx; ++i)
	{
		for (int j = 0; j < ny; ++j)
		{
			vec4[i][j] = ++n;
		}
	}

	size_t time_vec = clock() - start_time_vec; // искомое в
	std::cout << "TIME 2-DIM VECTOR = " << time_vec << std::endl;
	std::cout << "TOTAL AMOUNT = " << nx * ny << std::endl;
	std::cout << std::endl;

}

void testVector3d()
{
	int nx = 1000, ny = 1000, nz = 160;

	vector_n<int, 3> a(nx, ny, nz);
	std::vector<std::vector<vec_int>> vec3;
	vec3.resize(nx, vec2_int(ny, vec_int(nz)));

	size_t start_time_new = clock(); // начальное время
	for (int i = 0, n = 0; i < nx; ++i)
	{
		for (int j = 0; j < ny; ++j)
		{
			for (int k = 0; k < nz; ++k)
			{
				a(i, j, k) = n;
				++n;
			}
		}
	}

	size_t search_time = clock() - start_time_new;
	std::cout << "TIME 1-DIM VECTOR = " << search_time << std::endl;

	size_t start_time_vec = clock(); // начальное время
	for (int i = 0, n = 0; i < nx; ++i)
	{
		for (int j = 0; j < ny; ++j)
		{
			for (int k = 0; k < nz; ++k)
			{
				vec3[i][j][k] = ++n;
			}
		}
	}

	size_t time_vec = clock() - start_time_vec;
	std::cout << "TIME 3-DIM VECTOR = " << time_vec << std::endl;
	std::cout << "TOTAL AMOUNT = " << nx * ny * nz << std::endl;
	std::cout << std::endl;
}

bool test_index_full_1()
{
	vector_n<int, 3> a(3, 4, 5);
	int val = 0;
	for (int i1 = 0; i1 < 3; ++i1)
		for (int i2 = 0; i2 < 4; ++i2)
			for (int i3 = 0; i3 < 5; ++i3) a(i1, i2, i3) = val++;

	int val_old = val;
	val = 0;

	for (auto x : a.get_indexer<0, 1, 2>())
	{
		if (x.value != val++) return false;
		if (val_old < val) return false;
	}
	return true;
}

bool test_index_full_2()
{
	vector_n<int, 3> a(3, 4, 5);
	int val = 0;
	for (int i3 = 0; i3 < 5; ++i3)
		for (int i1 = 0; i1 < 3; ++i1)
			for (int i2 = 0; i2 < 4; ++i2) a(i1, i2, i3) = val++;

	int val_old = val;
	val = 0;

	for (auto x : a.get_indexer<2, 0, 1>())
	{
		if (x.value != val++) return false;
		if (val_old < val) return false;
	}
	return true;
}

bool test_index_partial1()
{
	vector_n<int, 4> a(3, 4, 5, 6);
	{
		int val = 0;
		for (int i1 = 0; i1 < 3; ++i1)
			for (int i2 = 0; i2 < 4; ++i2)
				for (int i3 = 0; i3 < 5; ++i3)
					for (int i4 = 0; i4 < 6; ++i4) a(i1, i2, i3, i4) = val++;
	}

	for (auto x : a.get_indexer<2, 0>())
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 6; ++j)
			{
				if (x.value(i, j) != a(x.index[1], i, x.index[0], j))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool test_index_partial2()
{
	vector_n<int, 5> a(3, 4, 5, 6, 7);
	{
		auto tmp = &a(0, 0, 0, 0, 0);
		for (int i = 0; i < 3 * 4 * 5 * 6 * 7; ++i)
		{
			tmp[i] = i;
		}
	}

	for (auto x : a.get_indexer<0, 3>())  // 0 and 3
	{
		for (auto y : x.value.get_indexer<2, 0>()) // 4 and 1
		{
			for (auto z : y.value.get_indexer<0>()) // 2
			{
				if (a(x.index[0], y.index[1], z.index[0], x.index[1], y.index[0]) != z.value)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool test_fix1()
{
	vector_n<int, 2> a(3, 4);

	{
		int tmp = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				a(i, j) = tmp++;
			}
		}
	}

	auto f = a.fix<0>(2);
	for (int i = 0; i < 4; ++i)
	{
		if (a(2, i) != f(i))
		{
			return false;
		}
	}

	return true;
}

bool test_fix2()
{
	vector_n<int, 4> a(3, 4, 5, 6);

	{
		int tmp = 0;
		for (int i1 = 0; i1 < 3; ++i1)
		{
			for (int i2 = 0; i2 < 4; ++i2)
			{
				for (int i3 = 0; i3 < 5; ++i3)
				{
					for (int i4 = 0; i4 < 6; ++i4)
					{
						a(i1, i2, i3, i4) = tmp++;
					}
				}
			}
		}
	}

	auto f = a.fix<1, 2>(3, 4);
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			if (a(i, 3, 4, j) != f(i, j))
			{
				return false;
			}
		}
	}

	return true;
}

bool test_fix_full()
{
	vector_n<int, 3> a(3, 4, 5);
	{
		int val = 0;
		for (int i1 = 0; i1 < 3; ++i1)
			for (int i2 = 0; i2 < 4; ++i2)
				for (int i3 = 0; i3 < 5; ++i3) a(i1, i2, i3) = val++;
	}
	for (int i1 = 0; i1 < 3; ++i1)
		for (int i2 = 0; i2 < 4; ++i2)
			for (int i3 = 0; i3 < 5; ++i3)
			{
				auto f = a.fix<1, 0, 2>(i2, i1, i3);
				if (a(i1, i2, i3) != f()) return false;
			}
	return true;
}

bool test_copy_constructor()
{
	vector_n<int, 3> a(3, 4, 5);

	int val = 0;
	for (int i1 = 0; i1 < 3; ++i1)
		for (int i2 = 0; i2 < 4; ++i2)
			for (int i3 = 0; i3 < 5; ++i3) a(i1, i2, i3) = val++;
	
	// Copy constructor
	vector_n<int, 3> b = a;
	for (int i1 = 0; i1 < 3; ++i1)
		for (int i2 = 0; i2 < 4; ++i2)
			for (int i3 = 0; i3 < 5; ++i3) b(i1, i2, i3) += 42;
	
	for (int i1 = 0; i1 < 3; ++i1)
		for (int i2 = 0; i2 < 4; ++i2)
			for (int i3 = 0; i3 < 5; ++i3)
			{
				if (a(i1, i2, i3) != b(i1, i2, i3) - 42)
				{
					return false;
				}
			}

	return true;
}

bool test_copy_assignment()
{
	vector_n<int, 3> a;
	{
		vector_n<int, 3> b(3, 4, 5);
		b(0, 0, 0) = 21;
		a = b;
		b(0, 0, 0) = 12;
		// Must be 12, 21
		if(b(0, 0, 0) != 12 || a(0, 0, 0) != 21) return false;
	}
	// Try to get value
	return a(0, 0, 0) == 21;
}

int main()
{
	auto tests = {test_index_full_1, test_index_full_2,
		test_index_partial1, test_index_partial2,
		test_fix1, test_fix2, test_fix_full, 
		test_copy_constructor, test_copy_assignment};
	for (auto test : tests)
	{
		if (!test())
		{
			std::cout << "Error\n";
			return -1;
		}
	}
	std::cout << "Ok\n";
	// TODO: write simple tests
	/*testVector4d();
	testVector2d();
	testVector3d();*/

	return 0;
}