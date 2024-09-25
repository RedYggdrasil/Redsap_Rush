#include "App/Data/Meshes/RSRMesh2D.h"

static constexpr UINT VERTEX2D_SIZE = (UINT)sizeof(RSRush::VertexPosition2DUV);
static constexpr bool B_APPLY_TRS_MATRIX = false;

RSRush::RSRMesh2D::RSRMesh2D(const std::vector<VertexPosition2DUV>& InVertexes)
    :RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX2D_SIZE), m_vertexes(InVertexes.size())
{
    for (const VertexPosition2DUV& vertex : InVertexes)
    {
        m_vertexes.push_back(vertex);
    }
}

RSRush::RSRMesh2D::RSRMesh2D(std::vector<VertexPosition2DUV>&& InVertexes)
    :RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX2D_SIZE), m_vertexes(std::move(InVertexes))
{
}

RSRush::RSRMesh2D::RSRMesh2D(const std::vector<VertexPosition2DUV>& InVertexes, const std::vector<unsigned short>& InIndexes)
    : RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX2D_SIZE, InIndexes)
{
    m_vertexes.reserve(InVertexes.size());

    for (const VertexPosition2DUV& Vertex : InVertexes)
    {
        m_vertexes.push_back(Vertex);
    }
}

RSRush::RSRMesh2D::RSRMesh2D(std::vector<VertexPosition2DUV>&& InVertexes, std::vector<unsigned short>&& InIndexes)
    :RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX2D_SIZE, std::move(InIndexes)), m_vertexes(std::move(InVertexes))
{
}

RSRush::RSRMesh2D::~RSRMesh2D()
{

}

RSRush::RSRMesh2D::RSRMesh2D()
    :RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX2D_SIZE), m_vertexes(0)
{
}