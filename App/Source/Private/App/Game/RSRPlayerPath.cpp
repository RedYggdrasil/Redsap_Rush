#include "App/Game/RSRPlayerPath.h"
#include "App/Tools/RSRLog.h"
#include <Tracy.hpp>

using namespace DirectX;
using namespace RSRush;

static RSRPPNode::RSRPPNodePoint CreateInvalidPoint()
{
	RSRPPNode::RSRPPNodePoint result;
	result.Location = { RSRPPNode::LOWER_THAN_LIMIT_Z_AXIS, RSRPPNode::LOWER_THAN_LIMIT_Z_AXIS, RSRPPNode::LOWER_THAN_LIMIT_Z_AXIS };
	for (size_t i = 0; i < RSRPPNode::MAX_NODE_POINT_PATH_SPLIT; ++i)
	{
		result.FromToConnections[i] = nullptr;
	}
	return result;
}
const RSRPPNode::RSRPPNodePoint RSRPPNode::RSRPPNodePoint::INVALID_POINT = CreateInvalidPoint();

static RSRPPNode CreateInvalidNode()
{
	RSRPPNode result;
	result.Progression = -1.0;
	for (size_t i = 0; i < RSRPPNode::MAX_PARALLEL_PATHS; ++i)
	{
		result.PathPoints[i] = RSRPPNode::RSRPPNodePoint::INVALID_POINT;
	}
	return result;
}

const RSRPPNode RSRPPNode::INVALID_NODE = CreateInvalidNode();


int RSRPPNode::ComputePathCount() const
{
	int pCount = 0;
	while (pCount < RSRPPNode::MAX_PARALLEL_PATHS && this->PathPoints[pCount].Location.z > RSRPPNode::LOWER_LIMIT_Z_AXIS)
	{
		++pCount;
	}
	return pCount;
}

const RSRPPNode& RSRush::RSRPPNode::GetInvalid()
{
	return INVALID_NODE;
}

RSRPPNode RSRush::RSRPPNode::Create(const double InProgression, const RSRPPNode::RSRPPNodePoint InPathPoints[MAX_PARALLEL_PATHS])
{
	RSRPPNode result =
	{
		.Progression = InProgression,
		.PathPoints = {}
	};
	std::memcpy(result.PathPoints, InPathPoints, sizeof(result.PathPoints));
	return result;
}

RSRPPNode RSRush::RSRPPNode::Create(const double InProgression, RSRPPNode::RSRPPNodePoint(&& InPathPoints)[MAX_PARALLEL_PATHS])
{
	RSRPPNode result = 
	{
		.Progression = InProgression,
		.PathPoints = {}
	};
	std::memcpy(result.PathPoints, InPathPoints, sizeof(result.PathPoints));
	return result;
}


bool RSRush::RSRPlayerPath::ConnectNodePoints(RSRPPNode& FromNode, const size_t InFromNodePointIndex, RSRPPNode& ToNode, const size_t InToNodePointIndex)
{
	if (!FromNode.IsValid() || InFromNodePointIndex >= RSRPPNode::MAX_PARALLEL_PATHS || !FromNode.PathPoints[InToNodePointIndex].IsValid())
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot connect Points, first Node '%s' is Invalid  !"),
			 mds::_tostring(FromNode.PathPoints[std::max<int>((int)InFromNodePointIndex, RSRPPNode::MAX_PARALLEL_PATHS - 1)].Location)
		);
		return false;
	}
	return ConnectNodePoints(FromNode.PathPoints[InFromNodePointIndex], ToNode, InToNodePointIndex);
}

bool RSRush::RSRPlayerPath::ConnectNodePoints(RSRPPNode::RSRPPNodePoint& FromPoint, RSRPPNode& ToNode, const size_t InToNodePointIndex)
{
	if (!FromPoint.IsValid()  || !ToNode.IsValid() || InToNodePointIndex >= RSRPPNode::MAX_PARALLEL_PATHS || !ToNode.PathPoints[InToNodePointIndex].IsValid())
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot connect Points, '%s' is Invalid  !"),
			FromPoint.IsValid() ? mds::_tostring(FromPoint.Location) : mds::_tostring(ToNode.PathPoints[std::max<int>((int)InToNodePointIndex, RSRPPNode::MAX_PARALLEL_PATHS - 1)].Location)
		);
		return false;
	}
	RSRPPNode::RSRPPNodePoint& ToPoint = ToNode.PathPoints[InToNodePointIndex];
	size_t conxIdx;
	if (!FromPoint.NextAvalableConnectionIndex(/*Out*/ conxIdx))
	{
		RSRLog::Log(LOG_ERROR, TEXT("Cannot connect Points, as FromPoint as already the max number of connections !"));
		return false;
	}
	
	FromPoint.FromToConnections[conxIdx] = RSRPPConnection::Create(*this, (uint16_t)InToNodePointIndex, FromPoint.Location, ToPoint.Location);
	return true;
}

void RSRush::RSRPlayerPath::TrimForProgression(const double InProgression)
{
	ZoneScoped;
	size_t count = m_pathPoints.size();
	if (m_pathPoints.size() > 4) //4 is arbitrary
	{
		size_t toFrontPop = 0;
		//What we are looking to trims, is all the points where [PointIdx +1].Progression > InProgression
		while (count > toFrontPop && m_pathPoints[toFrontPop].Progression < InProgression)
		{
			++toFrontPop;
		}
		toFrontPop = (toFrontPop > 0) ? (toFrontPop - 1) : 0;

		for (size_t i = 0; i < toFrontPop; ++i)
		{
			this->OnNodeToBeRemoved(m_pathPoints.front());
			m_pathPoints.pop_front();
		}
	}
}

void RSRush::RSRPlayerPath::OnNodeToBeRemoved(RSRPPNode& InToBeRemoved)
{
	if (InToBeRemoved.IsValid())
	{
		for (RSRPPNode::RSRPPNodePoint& NodePoint : InToBeRemoved.PathPoints)
		{
			if (NodePoint.IsValid())
			{
				size_t numConnection = NodePoint.NumConnections();
				for (size_t i = 0; i < numConnection; ++i)
				{
					RSRPPConnection::Clean(NodePoint.FromToConnections[i]);
				}
			}
		}
	}
}

RSRPPNode::RSRPPNodePoint::RSRPPNodePoint(const DirectX::XMFLOAT3& InLocation)
: Location(InLocation)
{
	for (size_t i = 0; i < MAX_NODE_POINT_PATH_SPLIT; ++i)
	{
		FromToConnections[i] = RSRPPNode::RSRPPNodePoint::INVALID_POINT.FromToConnections[i];
	}
}

size_t RSRPPNode::RSRPPNodePoint::NumConnections() const
{
	for (size_t i = 0; i < MAX_NODE_POINT_PATH_SPLIT; ++i)
	{
		if (FromToConnections[i] == nullptr)
		{
			return i;
		}
	}
	return MAX_NODE_POINT_PATH_SPLIT;
}