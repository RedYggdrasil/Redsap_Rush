#include "App/Data/Meshes/RSRMesh3D.h"
#include "App/Tools/RSRLog.h"

static constexpr UINT VERTEX3D_SIZE = (UINT)sizeof(RSRush::VertexPositionUVColor);
static constexpr bool B_APPLY_TRS_MATRIX = true;

DirectX::BoundingBox RSRush::RSRMesh3D::ComputeBoundingBox() const
{
    size_t count = m_vertexes.size();
    if (m_vertexes.size() < count)
    {
        RSRLog::Log(LOG_ERROR, TEXT("No CPU vertex data to compute bounds ! returning default"));
        return DirectX::BoundingBox();
    }
    const auto extremums = ComputeExtremums(m_vertexes.data(), m_vertexes.size());
    DirectX::BoundingBox result;
    DirectX::BoundingBox::CreateFromPoints(/*Out*/result, DirectX::XMLoadFloat3(&extremums[0]), DirectX::XMLoadFloat3(&extremums[1]));
    return result;
}

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

RSRush::RSRMesh3D::RSRMesh3D(const std::filesystem::path& InImagePath)
: RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX3D_SIZE)
{
}

RSRush::RSRMesh3D::~RSRMesh3D()
{

}

RSRush::RSRMesh3D::RSRMesh3D()
:RSRush::RSRMesh(B_APPLY_TRS_MATRIX, VERTEX3D_SIZE), m_vertexes(0)
{
}
