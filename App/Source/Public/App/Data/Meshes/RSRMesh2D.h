#pragma once

#include "App/Data/Meshes/RSRMesh.h"
#include <vector>

namespace RSRush
{
	class RSRMesh2D;
	typedef std::weak_ptr<RSRush::RSRMesh2D> RSRWeakMesh2DhPtr;
	typedef std::shared_ptr<RSRush::RSRMesh2D> RSRSharedMesh2DPtr;

	class RSRMesh2D : public RSRush::RSRMesh
	{
	protected:
		//CPU Resources
		std::vector<VertexPosition2DUV> m_vertexes;

	public:
		virtual UINT GetVertexCount() const override { return (UINT)m_vertexes.size(); }
		virtual const void* GetVertexAdress() const override { return &m_vertexes[0]; }

	public:
		RSRMesh2D(const std::vector<VertexPosition2DUV>& InVertexes);
		RSRMesh2D(std::vector<VertexPosition2DUV>&& InVertexes);

		RSRMesh2D(const std::vector<VertexPosition2DUV>& InVertexes, const std::vector<unsigned short>& InIndexes);
		RSRMesh2D(std::vector<VertexPosition2DUV>&& InVertexes, std::vector<unsigned short>&& InIndexes);

		/*virtual*/~RSRMesh2D()/*override*/;
	private:
		RSRMesh2D();
	};
}