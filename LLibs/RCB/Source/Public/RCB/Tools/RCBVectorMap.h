#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace rcb
{
	template<typename Iter, typename TKey>
	concept IteratorOf = requires(Iter it) 
	{
		typename std::iterator_traits<Iter>::value_type;
		requires std::same_as<typename std::iterator_traits<Iter>::value_type, TKey>;
	};

	//have a std::vector of TKey, it is effectively an efficient map ids->TKey, maitain it with vector of value and use it to update key index
	template <typename TKey, typename TValue, bool b_ALLOW_REORDERING_VALUES = true>
	struct RCBVectorMap
	{
	public:
		struct RCBValues
		{
		private:
			RCBVectorMap* m_map;
		public:
			inline std::vector<TValue>::iterator begin()
			{
				return m_map->m_values.begin();
			}
			inline std::vector<TValue>::iterator end()
			{
				return m_map->m_values.end();
			}
			inline std::vector<TValue>::const_iterator begin() const
			{
				return m_map->m_values.cbegin();
			}
			inline std::vector<TValue>::const_iterator end() const
			{
				return m_map->m_values.cend();
			}
			TValue& operator[](const size_t InIndex) 
			{
				return m_map->m_values[InIndex];
			}
			const TValue& operator[](const size_t InIndex) const
			{
				return m_map->m_values[InIndex];
			}


			inline size_t size() const { return m_map->m_values.size(); }
			RCBValues(RCBVectorMap* InMap) : m_map(InMap) { }

			std::vector<TValue>& GetUnderlyingVector() { return m_map->m_values; }
			const std::vector<TValue>& GetUnderlyingVector() const { return m_map->m_values; }
		};
	protected:
		std::vector<TValue> m_values;
		std::vector<TKey> m_keys;
		std::unordered_map<TKey, size_t> m_IDXs;

		std::unordered_set<TKey> m_keysMarkedForDeletion;
	public:
		RCBValues Values;

#pragma region Constructors and affectation
	public:
		RCBVectorMap() : Values(this) {}
		/// <summary>
		/// Copy constructor
		/// </summary>
		/// <param name="InOther">The other map</param>
		RCBVectorMap(const RCBVectorMap& InOther)
			: m_values(InOther.m_values), m_IDXs(InOther.m_IDXs), m_keys(InOther.m_keys), m_keysMarkedForDeletion(InOther.m_keysMarkedForDeletion),  Values(this)
		{
		}
		/// <summary>
		/// Copy assignment
		/// </summary>
		/// <param name="InOther">The other map</param>
		/// <returns>this</returns>
		RCBVectorMap& operator=(const RCBVectorMap& InOther) 
		{
			if (this != &InOther) 
			{
				m_values = InOther.m_values;
				m_IDXs = InOther.m_IDXs;
				m_keys = InOther.m_keys;
				m_keysMarkedForDeletion = InOther.m_keysMarkedForDeletion;
				Values = RCBValues(this);
			}
			return *this;
		}
		/// <summary>
		/// Move constructor
		/// </summary>
		/// <param name="InMoveOther">The other map to move</param>
		RCBVectorMap(RCBVectorMap&& InMoveOther) noexcept
			: m_values(std::move(InMoveOther.m_values)), m_IDXs(std::move(InMoveOther.m_IDXs)), m_keys(std::move(InMoveOther.m_keys)), m_keysMarkedForDeletion(std::move(InMoveOther.m_keysMarkedForDeletion)), Values(this)
		{
		}

		/// <summary>
		/// Move assignment
		/// </summary>
		/// <param name="InMoveOther">The other map to move</param>
		/// <returns>this</returns>
		RCBVectorMap& operator=(RCBVectorMap&& InMoveOther) noexcept 
		{
			if (this != &InMoveOther) 
			{
				m_values = std::move(InMoveOther.m_values);
				m_IDXs = std::move(InMoveOther.m_IDXs);
				m_keys = std::move(InMoveOther.m_keys);
				m_keysMarkedForDeletion = std::move(InMoveOther.m_keysMarkedForDeletion);
				Values = RCBValues(this);
			}
			return *this;
		}
#pragma endregion

#pragma region Iterator

		class iterator
		{
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = std::pair<const TKey, TValue>;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using reference = value_type&;

			iterator(typename std::unordered_map<TKey, size_t>::iterator mapIt, RCBVectorMap& parent)
				: m_mapIt(mapIt), m_parent(parent) 
			{
			}

			value_type operator*() const 
			{
				TKey key = m_mapIt->first;
				TValue& value = m_parent.m_values[m_mapIt->second];
				return { key, value };
			}

			iterator& operator++() 
			{
				++m_mapIt;
				return *this;
			}

			bool operator!=(const iterator& other) const 
			{
				return m_mapIt != other.m_mapIt;
			}

		private:
			typename std::unordered_map<TKey, size_t>::iterator m_mapIt;
			RCBVectorMap& m_parent;
		};

		// Methods to obtain iterators
		iterator begin() 
		{
			return iterator(m_IDXs.begin(), *this);
		}

		iterator end() 
		{
			return iterator(m_IDXs.end(), *this);
		}
#pragma endregion

#pragma region Insert Remove
		void insert_or_assign(const TKey& InKey, const TValue& InValue)
		{
			auto iter = m_IDXs.find(InKey);
			if (iter != m_IDXs.end())
			{
				m_values[iter->second] = InValue;
			}
			else
			{
				size_t newIndex = m_values.size();
				m_values.push_back(InValue);
				m_keys.push_back(InKey);
				m_IDXs.insert({ InKey, newIndex });
			}
		}

		void mark_for_deletion(const TKey& InKey)
		{
			auto iter = m_IDXs.find(InKey);
			if (iter != m_IDXs.end())
			{
				m_keysMarkedForDeletion.insert(InKey);
			}
		}
		void clear_marked_for_deletion()
		{
			removeRange(m_keysMarkedForDeletion.begin(), m_keysMarkedForDeletion.end());
			m_keysMarkedForDeletion.clear();
		}

		void remove(const TKey& InKey) 
		{
			auto iter = m_IDXs.find(InKey);
			if (iter != m_IDXs.end())
			{
				size_t foundIndex = iter->second;
				if constexpr (b_ALLOW_REORDERING_VALUES)
				{
					if (foundIndex != m_values.size() - 1)
					{
						//swap value and sorted keys
						std::swap(m_values[foundIndex], m_values.back());
						std::swap(m_keys[foundIndex], m_keys.back());

						//update key previously pointing to back, now points to index previously occupied by the deleted element
						m_IDXs[m_keys[foundIndex]] = foundIndex;
					}
					m_values.pop_back();
					m_keys.pop_back();
					m_IDXs.erase(InKey);
				}
				else //Maintain vector order
				{
					m_values.erase(m_values.begin() + foundIndex);
					m_keys.erase(m_keys.begin() + foundIndex);
					m_IDXs.erase(InKey);

					//Keys witch pointed to an elent past the old 'foundIndex' need to be update (normally oldIndex - 1)
					for (size_t KeyToUpdateIndex = foundIndex; KeyToUpdateIndex < m_keys.size(); ++KeyToUpdateIndex)
					{
						m_IDXs[m_keys[KeyToUpdateIndex]] = KeyToUpdateIndex;
					}
				}
			}
		}
		template<typename TKeyIterator>
			requires IteratorOf<TKeyIterator, TKey>
		void removeRange(TKeyIterator begin, TKeyIterator end)
		{
			if (!(begin != end))
			{
				return;
			}

			if constexpr (b_ALLOW_REORDERING_VALUES)
			{
				for (auto it = begin; it != end; ++it)
				{
					remove(*it);
				}
			}
			else
			{
				size_t smallerFoundIndex = std::numeric_limits<size_t>::max();
				for (auto it = begin; it != end; ++it)
				{
					auto iter = m_IDXs.find(*it);
					if (iter != m_IDXs.end())
					{
						size_t foundIndex = iter->second;
						smallerFoundIndex = std::min(smallerFoundIndex, foundIndex);
						m_values.erase(m_values.begin() + foundIndex);
						m_keys.erase(m_keys.begin() + foundIndex);
						m_IDXs.erase(*iter);
					}
				}
				//udate all keys starting from the smaller changed key
				for (size_t KeyToUpdateIndex = smallerFoundIndex; KeyToUpdateIndex < m_keys.size(); ++KeyToUpdateIndex)
				{
					m_IDXs[m_keys[KeyToUpdateIndex]] = KeyToUpdateIndex;
				}
			}
		}
		void clear()
		{
			m_values.clear();
			m_keys.clear();
			m_IDXs.clear();
			m_keysMarkedForDeletion.clear();
		}

#pragma endregion
#pragma region Data

		inline bool contains(const TKey& InKey) const
		{
			return m_IDXs.contains(InKey);
		}
		inline size_t size() const 
		{ 
			return m_IDXs.size(); 
		}
		inline void reserve(const size_t InNewCapacity)
		{
			m_IDXs.reserve(InNewCapacity);
			m_values.reserve(InNewCapacity);
			m_keys.reserve(InNewCapacity);
		}
		inline void reserve_over_current_size(const size_t InAdditionalCapacity)
		{
			reserve(m_values.size() + InAdditionalCapacity);
		}

		TValue& operator[](const TKey& InKey)
		{
			return m_values[m_IDXs[InKey]];
		}
		const TValue& operator[](const TKey& InKey) const
		{
			return m_values[m_IDXs[InKey]];
		}

#pragma endregion
	};
}