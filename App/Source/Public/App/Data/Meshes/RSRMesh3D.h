#pragma once

#include "App/Data/Meshes/RSRMesh.h"
#include "MDS/Tools/RCoordinate.h"
#include <vector>

namespace RSRush
{
	class RSRMesh3D;
	typedef std::weak_ptr<RSRush::RSRMesh3D> RSRWeakMesh3DhPtr;
	typedef std::shared_ptr<RSRush::RSRMesh3D> RSRSharedMesh3DPtr;

	class RSRMesh3D : public RSRush::RSRMesh
	{
	protected:
		//CPU Resources
		std::vector<VertexPositionUVColor> m_vertexes;

	public:
		virtual UINT GetVertexCount() const override { return (UINT)m_vertexes.size(); }
		virtual const void* GetVertexAdress() const override { return &m_vertexes[0]; }

	public:
		RSRMesh3D(const std::vector<VertexPositionUVColor>& InVertexes);
		RSRMesh3D(std::vector<VertexPositionUVColor>&& InVertexes);

		RSRMesh3D(const std::vector<VertexPositionUVColor>& InVertexes, const std::vector<unsigned short>& InIndexes);
		RSRMesh3D(std::vector<VertexPositionUVColor>&& InVertexes, std::vector<unsigned short>&& InIndexes);

		/*virtual*/~RSRMesh3D()/*override*/;
	private:
		RSRMesh3D();
	};
}