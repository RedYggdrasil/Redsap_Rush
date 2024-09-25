#include "App/Data/Materials/RSRMaterial.h"

bool RSRush::Mat::RSRMaterial::ApplyMaterial(ID3D12GraphicsCommandList7* InCommandList)
{
	bool bAllSucessfull = true;
	bAllSucessfull = this->ApplyContext(InCommandList) && bAllSucessfull;
	bAllSucessfull = this->ApplyObject(InCommandList) && bAllSucessfull;
	return bAllSucessfull;
}
