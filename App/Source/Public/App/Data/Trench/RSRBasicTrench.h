#pragma once

#include "MDS/RTool.h"
#include "App/Data/Trench/RSRTrench.h"
#include <vector>

namespace RSRush
{
	class RSRBasicTrench;

	struct RSRTaskTrenchGeometry
	{
	public:
		virtual void StartTask() R_PURE;
	};
	class RSRBasicTrench : public RSRTrench
	{
	protected:
		std::shared_ptr<RSRTaskTrenchGeometry> m_GeometryTask;

	public:
		RSRBasicTrench(RSRTrenchManager* InTrenchManager);
		/*virtual*/ ~RSRBasicTrench() /*override*/;
		virtual void StartGeneratingMesh();

	protected:
		virtual void PositionSelf()/*pure_override*/;

	protected:

		virtual void GeneratingMeshInitialSetup();
		virtual void GeneratingMeshThreadSensitive();
		virtual std::shared_ptr<RSRTaskTrenchGeometry> CreateGeometryTask();
		virtual void StartGeneratingMeshThreadInsensitive();
		virtual void ReportGeometryGenerated();

		virtual void GeneratePath();
		virtual void GenerateGeometry();
		virtual void GeneratePositionData();
		virtual void GeneratePhysicColliders();

	public:
		friend struct RSRTaskBasicTrenchGeometry;
	};
}