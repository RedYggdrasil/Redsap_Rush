#pragma once
#include <DirectXMath.h>

namespace mds
{
	class ZYXInteratorMaxExcluded
	{
	private:
		DirectX::XMINT3 m_min;
		DirectX::XMINT3 m_max;
		DirectX::XMINT3 m_current;
	public:
		ZYXInteratorMaxExcluded(const DirectX::XMINT3& InMin, const DirectX::XMINT3& InMax, const DirectX::XMINT3& InCurrent)
			: m_min(InMin), m_max(InMax), m_current(InCurrent) {}

		DirectX::XMINT3 operator*() const { return m_current; }

		const ZYXInteratorMaxExcluded& operator++()
		{
			if (++m_current.z >= m_max.z)
			{
				m_current.z = m_min.z;
				if (++m_current.y >= m_max.y)
				{
					m_current.y = m_min.y;

					//This goes until X = Max included, so to out of bound end()
					if (m_current.x < m_max.x)
					{
						++m_current.x;
					}
				}
			}
			return *this;
		}

		bool operator!=(const ZYXInteratorMaxExcluded& InOther) const
		{
			return m_current.x != InOther.m_current.x || m_current.y != InOther.m_current.y || m_current.z != InOther.m_current.z;
		}
	public:
		static inline ZYXInteratorMaxExcluded GenerateBegin(const DirectX::XMINT3& InMin, const DirectX::XMINT3& InMax)
		{
			return ZYXInteratorMaxExcluded(InMin, InMax, InMin);
		};
		static inline ZYXInteratorMaxExcluded GenerateEnd(const DirectX::XMINT3& InMin, const DirectX::XMINT3& InMax)
		{
			return ZYXInteratorMaxExcluded(InMin, InMax, DirectX::XMINT3{ InMax.x, InMin.y, InMin.z });
		};
	};
	class ZYXInteratorMaxIncluded
	{
	private:
		DirectX::XMINT3 m_min;
		DirectX::XMINT3 m_max;
		DirectX::XMINT3 m_current;
	public:
		ZYXInteratorMaxIncluded(const DirectX::XMINT3& InMin, const DirectX::XMINT3& InMax, const DirectX::XMINT3& InCurrent)
			: m_min(InMin), m_max(InMax), m_current(InCurrent) {}

		DirectX::XMINT3 operator*() const { return m_current; }

		const ZYXInteratorMaxIncluded& operator++()
		{
			if (++m_current.z > m_max.z)
			{
				m_current.z = m_min.z;
				if (++m_current.y > m_max.y)
				{
					m_current.y = m_min.y;

					//This goes until X = Max+1 included, so to out of bound end()
					if (m_current.x <= m_max.x)
					{
						++m_current.x;
					}
				}
			}
			return *this;
		}

		bool operator!=(const ZYXInteratorMaxIncluded& InOther) const
		{
			return m_current.x != InOther.m_current.x || m_current.y != InOther.m_current.y || m_current.z != InOther.m_current.z;
		}
	public:
		static inline ZYXInteratorMaxIncluded GenerateBegin(const DirectX::XMINT3& InMin, const DirectX::XMINT3& InMax)
		{
			return ZYXInteratorMaxIncluded(InMin, InMax, InMin);
		};
		static inline ZYXInteratorMaxIncluded GenerateEnd(const DirectX::XMINT3& InMin, const DirectX::XMINT3& InMax)
		{
			return ZYXInteratorMaxIncluded(InMin, InMax, DirectX::XMINT3{ InMax.x + 1, InMin.y, InMin.z });
		};
	};

	class YXInteratorMaxExcluded
	{
	private:
		DirectX::XMINT2 m_min;
		DirectX::XMINT2 m_max;
		DirectX::XMINT2 m_current;
	public:
		YXInteratorMaxExcluded(const DirectX::XMINT2& InMin, const DirectX::XMINT2& InMax, const DirectX::XMINT2& InCurrent)
			: m_min(InMin), m_max(InMax), m_current(InCurrent) {}

		DirectX::XMINT2 operator*() const { return m_current; }

		const YXInteratorMaxExcluded& operator++()
		{
			if (++m_current.y >= m_max.y)
			{
				m_current.y = m_min.y;

				//This goes until X = Max included, so to out of bound end()
				if (m_current.x < m_max.x)
				{
					++m_current.x;
				}
			}				
			return *this;
		}

		bool operator!=(const YXInteratorMaxExcluded& InOther) const
		{
			return m_current.x != InOther.m_current.x || m_current.y != InOther.m_current.y;
		}
	public:
		static inline YXInteratorMaxExcluded GenerateBegin(const DirectX::XMINT2& InMin, const DirectX::XMINT2& InMax)
		{
			return YXInteratorMaxExcluded(InMin, InMax, InMin);
		};
		static inline YXInteratorMaxExcluded GenerateEnd(const DirectX::XMINT2& InMin, const DirectX::XMINT2& InMax)
		{
			return YXInteratorMaxExcluded(InMin, InMax, DirectX::XMINT2{ InMax.x, InMin.y });
		};
	};
	class YXInteratorMaxIncluded
	{
	private:
		DirectX::XMINT2 m_min;
		DirectX::XMINT2 m_max;
		DirectX::XMINT2 m_current;
	public:
		YXInteratorMaxIncluded(const DirectX::XMINT2& InMin, const DirectX::XMINT2& InMax, const DirectX::XMINT2& InCurrent)
			: m_min(InMin), m_max(InMax), m_current(InCurrent) {}

		DirectX::XMINT2 operator*() const { return m_current; }

		const YXInteratorMaxIncluded& operator++()
		{
			if (++m_current.y > m_max.y)
			{
				m_current.y = m_min.y;

				//This goes until X = Max+1 included, so to out of bound end()
				if (m_current.x <= m_max.x)
				{
					++m_current.x;
				}
			}
			return *this;
		}

		bool operator!=(const YXInteratorMaxIncluded& InOther) const
		{
			return m_current.x != InOther.m_current.x || m_current.y != InOther.m_current.y;
		}
	public:
		static inline YXInteratorMaxIncluded GenerateBegin(const DirectX::XMINT2& InMin, const DirectX::XMINT2& InMax)
		{
			return YXInteratorMaxIncluded(InMin, InMax, InMin);
		};
		static inline YXInteratorMaxIncluded GenerateEnd(const DirectX::XMINT2& InMin, const DirectX::XMINT2& InMax)
		{
			return YXInteratorMaxIncluded(InMin, InMax, DirectX::XMINT2{ InMax.x + 1, InMin.y });
		};
	};
};