#pragma once

#include "App/Libs/WinInclude.h"
#include "App/Data/Physic/RSRPPConnection.h"
#include <DirectXMath.h>
#include <vector>
#include <cstdint>

#include <memory>
#include <deque>


namespace RSRush
{
	struct RSRPPNode
	{
#pragma region Defs
	public:
		static const RSRPPNode INVALID_NODE;
		static constexpr float LOWER_LIMIT_Z_AXIS = -100000000.f;
		//Update Create function if change value
		static constexpr uint8_t MAX_PARALLEL_PATHS = 4;
		static constexpr uint8_t MAX_NODE_POINT_PATH_SPLIT = ((uint8_t)3 < MAX_PARALLEL_PATHS) ? (uint8_t)3 : MAX_PARALLEL_PATHS;

		static constexpr float LOWER_THAN_LIMIT_Z_AXIS = LOWER_LIMIT_Z_AXIS - 90000000.f;

	public:
		struct RSRPPNodePoint
		{
			static const RSRPPNodePoint INVALID_POINT;
			static constexpr DirectX::XMFLOAT3 INVALID_POINT_POS = { RSRPPNode::LOWER_THAN_LIMIT_Z_AXIS, RSRPPNode::LOWER_THAN_LIMIT_Z_AXIS, RSRPPNode::LOWER_THAN_LIMIT_Z_AXIS };
			static constexpr int16_t INVALID_PATH_IDX = -1;
		public:
			DirectX::XMFLOAT3 Location;
			std::shared_ptr<RSRPPConnection> FromToConnections[MAX_NODE_POINT_PATH_SPLIT];
		public:
			RSRPPNodePoint() = default;
			RSRPPNodePoint(const DirectX::XMFLOAT3& InLocation);
			static constexpr bool IsValid(const RSRPPNodePoint& InPoint) { return InPoint.Location.z >= LOWER_LIMIT_Z_AXIS; }
			constexpr bool IsValid() const { return IsValid(*this); }
			//This assume FromToConnections are always sorted as Valid Pointers First
			size_t NumConnections() const;
			inline bool NextAvalableConnectionIndex(size_t& OutNextIndex) const { OutNextIndex = NumConnections() + 1; return (OutNextIndex < MAX_NODE_POINT_PATH_SPLIT); }
		};
#pragma endregion Defs

	public:
		double Progression;
		RSRPPNodePoint PathPoints[MAX_PARALLEL_PATHS];

	public:
		int ComputePathCount() const;
	public:
		static const RSRPPNode& GetInvalid();
		static RSRPPNode Create(const double InProgression, const RSRPPNode::RSRPPNodePoint InPathPoints[MAX_PARALLEL_PATHS]);
		static RSRPPNode Create(const double InProgression, RSRPPNode::RSRPPNodePoint(&&InPathPoints)[MAX_PARALLEL_PATHS]);
		inline static RSRPPNode Create(const double InProgression,
			RSRPPNode::RSRPPNodePoint InPathPoint0 = RSRPPNode::RSRPPNodePoint::INVALID_POINT,
			RSRPPNode::RSRPPNodePoint InPathPoint1 = RSRPPNode::RSRPPNodePoint::INVALID_POINT,
			RSRPPNode::RSRPPNodePoint InPathPoint2 = RSRPPNode::RSRPPNodePoint::INVALID_POINT,
			RSRPPNode::RSRPPNodePoint InPathPoint3 = RSRPPNode::RSRPPNodePoint::INVALID_POINT
		)
		{
			return Create(InProgression, { InPathPoint0 , InPathPoint1, InPathPoint2, InPathPoint3 });
		}
		inline constexpr bool IsValid() const { return this->Progression >= 0.0; }
	};


	class RSRPlayerPath
	{
	private:
		std::deque<RSRPPNode> m_pathPoints;
	public:
		RSRPPNode* GetLastNode() { return ((m_pathPoints.size() > 0)? &m_pathPoints.back():nullptr); }
		inline const RSRPPNode * GetLastNode() const { return ((m_pathPoints.size() > 0) ? &m_pathPoints.back() : nullptr); }
		inline void PushBackNode(RSRPPNode&& InNewNode) { m_pathPoints.push_back(std::move(InNewNode)); }
		inline RSRPPNode* CreateGetBackNode() { return &m_pathPoints.emplace_back(); }
		inline int PathCountAt(const size_t InIndex) const { (InIndex < m_pathPoints.size()) ? m_pathPoints.at(InIndex).ComputePathCount() : -1; }

		bool ConnectNodePoints(RSRPPNode& FromNode, const size_t InFromNodePointIndex, RSRPPNode& ToNode, const size_t InToNodePointIndex);
		bool ConnectNodePoints(RSRPPNode::RSRPPNodePoint& FromPoint, RSRPPNode& ToNode, const size_t InToNodePointIndex);
	public:
		void TrimForProgression(const double InProgression);
		inline double GetMaxProgression() const { return (m_pathPoints.size() > 0) ? m_pathPoints.back().Progression : -1.; };
	private:
		void OnNodeToBeRemoved(RSRPPNode& InToBeRemoved);
	};
};