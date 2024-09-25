#pragma once
#include "App/Physic/LD/RSRLDType.h"
#include "App/Data/RSRTransform.h"
#include <string>
namespace RSRush
{
	struct RSRLDElem
	{
	public:
		RSRLDType Type = RSRLDType::Unknown;
		RSRTransform Transform;
		DirectX::XMFLOAT4 Quat;
		std::string Datas = "";
	};
};