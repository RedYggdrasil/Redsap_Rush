#pragma once
#include "App/Libs/WinInclude.h"
#include "MDS/Tools/RCoordinate.h"

namespace RSRush::Mat
{
	class RSRMaterialPart
	{
	public:
		virtual bool ApplyMaterialPart(ID3D12GraphicsCommandList7* InCommandList) {};
	};
	class RSRMaterialPartContext : public RSRMaterialPart
	{

	};
	class RSRMaterialPartElement : public RSRMaterialPart
	{

	};
}