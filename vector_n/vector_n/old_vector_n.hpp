#pragma once
#include <vector>
#include <type_traits>
#include <cstdlib>
#include <array>
#include <numeric>
#include <cassert> 
/*
!! OLD VERSION N-VERCTOR.

Need to test:
1. optimal allocation of large memory sizes (example, 10-30 gigabyte)
2. calculation time

Need to compare with vector and deque.
*/
template <size_t N>
using vec_n_size = std::array<size_t, N>;

namespace impl_
{
	template<typename ...Args>
	void ignore(Args ...) { ; }

	template<typename Arg>
	inline static Arg product(Arg arg)
	{
		return arg;
	}

	template<typename FirstArg, typename ... Args>
	inline FirstArg product(FirstArg first, Args ... args)
	{
		return product(args...) * first;
	}

	template<typename FirstIndex, typename ... Indexes>
	inline static FirstIndex index(const size_t *sizes, const size_t *dims, FirstIndex first)
	{
		return *dims * first;
	}

	template<typename FirstIndex, typename ... Indexes>
	inline static FirstIndex index(const size_t *sizes, const size_t *dims, FirstIndex first, Indexes ... indexes)
	{
		return *dims * first + index(sizes, dims + 1, indexes...);
	}

	template<typename FirstIndex>
	inline static bool checkIndex(const size_t *sizes, FirstIndex first)
	{
		return (size_t(first) < *sizes);
	}

	template<typename FirstIndex, typename ... Indexes>
	inline static bool checkIndex(const size_t *sizes, FirstIndex first, Indexes ... indexes)
	{
		return checkIndex(sizes, first) && checkIndex(sizes + 1, indexes...);
	}

	template<typename FirstArg>
	inline void calcCoefficients(size_t *arr, FirstArg first)
	{
		*arr = 1;
	}

	template<typename FirstArg, typename SecondArg, typename ... Args>
	inline void calcCoefficients(size_t *arr, FirstArg first, SecondArg second, Args ... args)
	{
		calcCoefficients(arr + 1, second, args...);
		*arr = *(arr + 1) * second;
	}

	inline void calcCoefficients(size_t *arr, const size_t *args)
	{
		if (args == nullptr)
		{
			*arr = 1;
			return;
		}

		calcCoefficients(arr + 1, args + 1);
		*arr = *(arr + 1) * *args;
	}

	template <size_t N>
	inline void calcCoefficients(size_t *arr, const size_t *args)
	{
		*(arr + N - 1) = 1;

		for (int n = N - 2; n >= 0; --n)
		{
			*(arr + n) = *(arr + n + 1) * *(args + n + 1);
		}
	}

	template<class First, class ...Args> struct AllNumeric
	{
		const static auto value = std::is_integral<First>::value && AllNumeric<Args...>::value;

	};

	template<class Arg> struct AllNumeric<Arg>
	{
		const static auto value = std::is_integral<Arg>::value;
	};

	template<typename ...Args>
	bool isPositive(Args ... args)
	{
		bool positive(true);

		ignore(positive = positive && args >= 0 ...);
		return positive;
	};
}

template<typename ElementType, size_t numDims>
class old_vector_n
{
public:
	old_vector_n() {};

	template<typename ... Sizes>
	old_vector_n(Sizes ... sizesDims)
	{
		static_assert(sizeof...(sizesDims) == numDims, "Parameters count is invalid");

		assert(impl::isPositive(sizesDims ...));

		sizes = { size_t(sizesDims)... };
		data.resize(impl::product(sizesDims ...));
		impl::calcCoefficients(dims.data(), sizesDims ...);
	}

	template<typename ... Sizes>
	inline void resize(Sizes ... sizesDims)
	{
		static_assert(sizeof...(sizesDims) == numDims, "Parameters count is invalid");

		sizes = { size_t(sizesDims)... };
		data.resize(impl_::product(sizesDims ...));
		impl_::calcCoefficients(dims.data(), sizesDims ...);
	}

	inline void resize(const vec_n_size<numDims> &sizesDims)
	{
		data.resize(std::accumulate(sizesDims.begin(), sizesDims.end(), (size_t)1, std::multiplies<size_t>()));
		sizes = sizesDims;

		impl_::calcCoefficients<numDims>(dims.data(), sizesDims.data());
	}

	template<typename ... Indexes>
	inline ElementType& operator()(Indexes ... indexes)
	{
		static_assert(impl_::AllNumeric<Indexes...>::value, "Parameters type is invalid");
		static_assert(sizeof...(indexes) == numDims, "Parameters count is invalid");

		return data[getIndex(indexes ...)];
	}

	template<typename ... Indexes>
	inline const ElementType& operator()(Indexes ... indexes) const
	{
		static_assert(impl_::AllNumeric<Indexes...>::value, "Parameters type is invalid");
		static_assert(sizeof...(indexes) == numDims, "Parameters count is invalid");

		return data[getIndex(indexes ...)];
	}

	template<typename ... Indexes>
	inline ElementType& at(Indexes ... indexes)
	{
		static_assert(impl_::AllNumeric<Indexes...>::value, "Parameters type is invalid");
		static_assert(sizeof...(indexes) == numDims, "Parameters count is invalid");

		return data[getIndex(indexes ...)];
	}

	template<typename ... Indexes>
	inline const ElementType& at(Indexes ... indexes) const
	{
		static_assert(impl_::AllNumeric<Indexes...>::value, "Parameters type is invalid");
		static_assert(sizeof...(indexes) == numDims, "Parameters count is invalid");
		
		return data[getIndex(indexes ...)];
	}

	inline const size_t& size(const size_t numberDims) const
	{
		assert((numberDims - 1) <= numDims && (numberDims - 1) >= 0 && "Parameters count is invalid");

		return sizes[numberDims - 1];
	}

	inline const vec_n_size<numDims>& size() const
	{
		return sizes;
	}

	inline void clear()
	{
		std::vector<ElementType>().swap(data);
	}

	typename std::vector<ElementType>::iterator begin()
	{
		return data.begin();
	}

	typename std::vector<ElementType>::iterator end()
	{
		return data.end();
	}

	typename std::vector<ElementType>::const_iterator begin() const
	{
		return data.begin();
	}

	typename std::vector<ElementType>::const_iterator end() const
	{
		return data.end();
	}

	template<typename ... Indexes>
	bool existData(Indexes ... indexes) const 
	{
		return impl_::checkIndex(sizes.data(), indexes...);
	}

private:

	template<typename ... Indexes>
	size_t getIndex(Indexes ... indexes) const
	{
		assert(impl_::checkIndex(sizes.data(), indexes...) && "Indexes is invalid.");

		auto index = impl_::index(sizes.data(), dims.data(), indexes...);
		
		assert(size_t(index) < data.size() && "Parameters count dimensional is invalid");

		return index;
	}

	std::vector<ElementType> data;
	std::array<size_t, numDims> dims;
	std::array<size_t, numDims> sizes;
};
