#pragma once
#include <DirectXMath.h>
#include <string>
#include <string_view>
#include <format>
#include <sstream>
#include <iomanip>
namespace mds
{
	class RLog
	{
	public:
	public:
		RLog() = delete;
	public:
        static std::string XMMatrixToString(const DirectX::XMMATRIX& InMatrix);
		static std::string XMFloat4x4ToString(const DirectX::XMFLOAT4X4& InFloat4x4);
		static std::string XMFloat4ToString(const DirectX::XMFLOAT4& InFloat4);
		static std::string XMFloat3ToString(const DirectX::XMFLOAT3& InFloat3);
		static std::string XMFloat2ToString(const DirectX::XMFLOAT2& InFloat2);
		static std::string XMInt4ToString(const DirectX::XMINT4& InInt4);
		static std::string XMInt3ToString(const DirectX::XMINT3& InInt3);
		static std::string XMInt2ToString(const DirectX::XMINT2& InInt2);
		static std::string XM_CALLCONV XMVECTORToString(DirectX::FXMVECTOR InVector, const uint8_t InVectorSize = 4);
	};

	struct RBlob
	{
	public:
		const char8_t* data;
		const size_t size;
	};
	inline constexpr std::string_view ByteView(const void* InData, const size_t size) 
	{
		return std::string_view(reinterpret_cast<const char*>(InData), size);
	};
	inline std::string HexadecimalView(const void* InData, const size_t size)
	{
		const unsigned char* uchar_data = static_cast<const unsigned char*>(InData);
		// Stringstream as no reserve function, we know the actual string size is size * 3, but can't use it there, too bad !
		std::stringstream ss;

		const size_t size_minus = (size > 0) ? (size-1) : 0Ui64;
		// Convert each byte to a 2-digit hexadecimal number
		for (size_t i = 0; i < size; ++i) {
			ss << std::hex << std::setw(2) << std::setfill('0')
				<< static_cast<const int>(uchar_data[i]);
			if (i < size_minus)
			{
				ss << ' ';
			}
		}
		return ss.str();
	};

	inline std::string _tostring(const DirectX::XMMATRIX& InMatrix)
	{ return RLog::XMMatrixToString(InMatrix); };
	static std::string XMFloat4x4ToString(const DirectX::XMFLOAT4X4& InFloat4x4)
	{ return RLog::XMFloat4x4ToString(InFloat4x4); };
	inline std::string XM_CALLCONV _tostring(DirectX::FXMVECTOR InVector, const uint8_t InVectorSize = 4)
	{ return RLog::XMVECTORToString(InVector, InVectorSize); };


	inline std::string _tostring(const DirectX::XMFLOAT4& InFloat4)
	{ return RLog::XMFloat4ToString(InFloat4); }
	inline std::string _tostring(const DirectX::XMFLOAT3& InFloat3)
	{ return RLog::XMFloat3ToString(InFloat3); }
	inline std::string _tostring(const DirectX::XMFLOAT2& InFloat2)
	{ return RLog::XMFloat2ToString(InFloat2); }
	
	inline std::string _tostring(const DirectX::XMINT4& InInt4)
	{ return RLog::XMInt4ToString(InInt4); }
	inline std::string _tostring(const DirectX::XMINT3& InInt3)
	{ return RLog::XMInt3ToString(InInt3); }
	inline std::string _tostring(const DirectX::XMINT2& InInt2)
	{ return RLog::XMInt2ToString(InInt2); }
};
//template <>
//struct std::formatter<mds::RBlob> : std::formatter<string_view> {
//	auto format(const mds::RBlob& InRBlob, std::format_context& ctx) const {
//		std::string temp;
//		std::format_to(std::back_inserter(temp), "({}, {}, {})",
//			col.r, col.g, col.b);
//		return std::formatter<string_view>::format(temp, ctx);
//	}
//};

