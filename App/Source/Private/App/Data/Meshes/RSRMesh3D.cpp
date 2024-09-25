#include "App/Data/Meshes/RSRMesh3D.h"

static constexpr UINT VERTEX3D_SIZE = (UINT)sizeof(RSRush::VertexPositionUVColor);
static constexpr bool B_APPLY_TRS_MATRIX = true;

RSRush::RSRMesh3D::RSRMesh3D(const std::vector<VertexPositionUVColor>& InVertexes)
:RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX3D_SIZE), m_vertexes(InVertexes.size())
{
	for (const VertexPositionUVColor& vertex : InVertexes)
	{
		m_vertexes.push_back(vertex);
	}
}

RSRush::RSRMesh3D::RSRMesh3D(std::vector<VertexPositionUVColor>&& InVertexes)
:RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX3D_SIZE), m_vertexes(std::move(InVertexes))
{
}

RSRush::RSRMesh3D::RSRMesh3D(const std::vector<VertexPositionUVColor>& InVertexes, const std::vector<unsigned short>& InIndexes)
: RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX3D_SIZE, InIndexes)
{
    m_vertexes.reserve(InVertexes.size());

    for (const VertexPositionUVColor& Vertex : InVertexes)
    {
        m_vertexes.push_back(Vertex);
    }
}

RSRush::RSRMesh3D::RSRMesh3D(std::vector<VertexPositionUVColor>&& InVertexes, std::vector<unsigned short>&& InIndexes)
:RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX3D_SIZE, std::move(InIndexes)), m_vertexes(std::move(InVertexes))
{
}

RSRush::RSRMesh3D::~RSRMesh3D()
{

}

RSRush::RSRMesh3D::RSRMesh3D()
:RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX3D_SIZE), m_vertexes(0)
{
}