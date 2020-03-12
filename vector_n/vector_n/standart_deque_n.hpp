#pragma once
#include <deque>
#include <type_traits>
#include <cstdlib>
#include <array>
#include <numeric>
#include <cassert> 
#include "old_vector_n.hpp"

/*
* Copy TestVector.h but it is for Deque
*/
namespace Helperdeque
{
	template <typename T, size_t N>
	struct MakeDeque
	{
		template <typename... Args>
		static auto make_deque(std::size_t first, Args... sizes)
			-> std::deque<decltype(MakeDeque<T, N - 1>::make_deque(sizes...))>
		{
			auto inner = MakeDeque<T, N - 1>::make_deque(sizes...);
			return std::deque<decltype(inner)>(first, inner);
		}

		static auto get_n_deque(size_t size)
			-> std::deque<decltype(MakeDeque<T, N - 1>::get_n_deque(size - 1))>
		{
			return std::vector<MakeDeque<T, N - 1>::get_n_deque(size - 1)>();
		}
	};

	template <typename T>
	struct MakeDeque<T, 1>
	{
		static std::deque<T> make_deque(std::size_t size) {
			return std::deque<T>(size);
		}

		static std::deque<T> get_n_deque(size_t N) {
			return std::deque<T>();
		}
	};

	template <typename T, typename ... Args>
	auto make_deque(Args... args)
		-> decltype(MakeDeque<T, sizeof...(Args)>::make_deque(args...))
	{
		return MakeDeque<T, sizeof...(Args)>::make_deque(args...);
	};

	template <typename T, size_t N>
	auto get_n_deque(size_t size)
		-> decltype(MakeDeque<T, N>::get_n_deque(size))
	{
		return MakeDeque<T, size>::get_n_deque(size);
	};

};

template <typename T, size_t N>
using deq_n_type = decltype(Helperdeque::get_n_deque<T, N>(N));

template<typename ElementType, size_t numDims>
class standart_deque_n
{
public:
	standart_deque_n() {}

	template<typename ... Sizes>
	standart_deque_n(Sizes ... sizesDims)
	{
		sizes = { size_t(sizesDims)... };
		data = Helperdeque::make_deque<ElementType>(sizesDims ...);
	}

	template<typename ... Sizes>
	inline void resize(Sizes ... sizesDims)
	{
		sizes = { size_t(sizesDims)... };
		data = Helperdeque::make_deque<ElementType>(sizesDims ...);
	}

	template<typename ... Sizes>
	inline void resize(const vec_n_size<numDims> &sizesDims)
	{
		sizes = sizesDims;
		data = Helperdeque::make_deque<ElementType>(std::get<Sizes>(std::forward<std::array>(sizesDims))...);
	}

	template<typename ... Indexes>
	inline ElementType& operator()(Indexes ... indexes)
	{
		return getData(data, indexes...);
	}

	template<typename ... Indexes>
	inline const ElementType& operator()(Indexes ... indexes) const
	{
		return getData(data, indexes...);
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

	template<typename ... Indexes>
	bool existData(Indexes ... indexes) const
	{
		return impl::checkIndex(sizes.data(), indexes...);
	}

private:

	deq_n_type<ElementType, numDims> data;
	std::array<size_t, numDims> sizes;

	template<typename Container, typename FirstIndex, typename ... Indexes>
	auto& getData(Container& c_data, FirstIndex first, Indexes ... indexes)
	{
		return getData(c_data[first], indexes...);
	}

	template<typename Container, typename FirstIndex>
	auto& getData(Container& c_data, FirstIndex first)
	{
		return c_data[first];
	}

	template<typename Container, typename FirstIndex, typename ... Indexes>
	auto& getData(Container& c_data, FirstIndex first, Indexes ... indexes) const
	{
		return getData(c_data[first], indexes...);
	}

	template<typename Container, typename FirstIndex>
	auto& getData(Container& c_data, FirstIndex first) const
	{
		return c_data[first];
	}
};