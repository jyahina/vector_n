#pragma once
#include "old_vector_n.hpp"

namespace HelperVector
{
	template <typename T, size_t N>
	struct MakeVector
	{
		template <typename... Args>
		static auto make_vector(std::size_t first, Args... sizes)
			-> std::vector<decltype(MakeVector<T, N - 1>::make_vector(sizes...))>
		{
			auto inner = MakeVector<T, N - 1>::make_vector(sizes...);
			return std::vector<decltype(inner)>(first, inner);
		}

		static auto get_n_vector(size_t size)
			-> std::vector<decltype(MakeVector<T, N - 1>::get_n_vector(size - 1))>
		{
			return std::vector<MakeVector<T, N - 1>::get_n_vector(size - 1)>();
		}
	};

	template <typename T>
	struct MakeVector<T, 1>
	{
		static std::vector<T> make_vector(std::size_t size) {
			return std::vector<T>(size);
		}

		static std::vector<T> get_n_vector(size_t N) {
			return std::vector<T>();
		}
	};

	template <typename T, typename ... Args>
	auto make_vector(Args... args)
		-> decltype(MakeVector<T, sizeof...(Args)>::make_vector(args...))
	{
		return MakeVector<T, sizeof...(Args)>::make_vector(args...);
	};

	template <typename T, size_t N>
	auto get_n_vector(size_t size)
		-> decltype(MakeVector<T, N>::get_n_vector(size))
	{
		return MakeVector<T, size>::get_n_vector(size);
	};

};

template <typename T, size_t N>
using vec_n_type = decltype(HelperVector::get_n_vector<T, N>(N));

template<typename ElementType, size_t numDims>
class TestVector
{
public:
	TestVector() {}

	template<typename ... Sizes>
	TestVector(Sizes ... sizesDims)
	{
		sizes = { size_t(sizesDims)... };
		data = HelperVector::make_vector<ElementType>(sizesDims ...);
	}

	template<typename ... Sizes>
	inline void resize(Sizes ... sizesDims)
	{
		sizes = { size_t(sizesDims)... };
		data = HelperVector::make_vector<ElementType>(sizesDims ...);
	}

	inline void resize(const vec_n_size<numDims> &sizesDims)
	{
		sizes = sizesDims;
		data = HelperVector::make_vector(std::get<numDims>(std::forward<std::array>(sizesDims))...);
	}

	template<typename ... Indexes>
	inline ElementType& operator()(Indexes ... indexes)
	{
		return getData(indexes...);
	}

	template<typename ... Indexes>
	inline const ElementType& operator()(Indexes ... indexes) const
	{
		return getData(indexes...);
	}

	template<typename ... Indexes>
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
		data.clear();
	}
	// 
	// 	typename std::vector<ElementType>::iterator begin()
	// 	{
	// 		return data.begin();
	// 	}
	// 
	// 	typename std::vector<ElementType>::iterator end()
	// 	{
	// 		return data.end();
	// 	}
	// 
	// 	typename std::vector<ElementType>::const_iterator begin() const
	// 	{
	// 		return data.begin();
	// 	}
	// 
	// 	typename std::vector<ElementType>::const_iterator end() const
	// 	{
	// 		return data.end();
	// 	}

private:

	vec_n_type<ElementType, numDims> data;
	std::array<size_t, numDims> sizes;

	template<typename LastIndex, typename ... Indexes>
	auto& getData(Indexes ... indexes, LastIndex last)
	{
		return getData(indexes...)[last];
	}

	template<typename FisrtIndex, typename LastIndex>
	auto& getData(FisrtIndex first, LastIndex last)
	{
		return data[first][last];
	}

	template<typename LastIndex>
	auto& getData(LastIndex last)
	{
		return data[last];
	}
};