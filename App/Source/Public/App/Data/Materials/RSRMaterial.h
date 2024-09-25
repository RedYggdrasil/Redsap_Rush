#pragma once
#include "App/Libs/WinInclude.h"
#include "App/Data/Materials/RSRMaterialParts.h"


//#include <wrl/client.h>
#include <memory>


namespace RSRush::Mat
{
	class RSRMaterial
	{
	protected:
		std::unique_ptr<RSRMaterialPartContext> m_context;
		std::unique_ptr<RSRMaterialPartContext> m_object;
	public:
		RSRMaterialPartContext* GetContext() { return m_context.get(); }
		const RSRMaterialPartContext* GetContext() const { return m_context.get(); }

		RSRMaterialPartContext* GetObject() { return m_context.get(); }
		const RSRMaterialPartContext* GetObject() const { return m_context.get(); }
	public:
		bool ApplyMaterial(ID3D12GraphicsCommandList7* InCommandList);
		inline bool ApplyContext(ID3D12GraphicsCommandList7* InCommandList) 
		{ if (m_context) { return m_context->ApplyMaterialPart(InCommandList); } return false; };
		inline bool ApplyObject(ID3D12GraphicsCommandList7* InCommandList)
		{ if (m_object) { return m_object->ApplyMaterialPart(InCommandList); } return false; };

	};
}