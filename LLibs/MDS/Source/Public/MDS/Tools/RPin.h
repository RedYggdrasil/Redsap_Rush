#pragma once

namespace mds
{
	class RPin
	{
	public:
		virtual bool HasResource() = 0;
		virtual RPin* Clone() = 0;
		virtual bool Release() = 0;
		virtual ~RPin();
	protected:
		RPin() = default;
	};
};