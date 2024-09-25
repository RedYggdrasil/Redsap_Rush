#pragma once

#ifndef IS_INCLUDE_FROM_GREEBLE_DECL

#define IS_INCLUDE_FROM_GREEBLE_IMPLS
#include "MDS/Algorithm/RGreeble.h"
#undef IS_INCLUDE_FROM_GREEBLE_IMPLS

#endif // !IS_INCLUDE_FROM_GREEBLE_IMPLS

namespace mds
{
	namespace NSRGreeble
	{
		RGREEBLE_TEMPLATE
		void RGREEBLE::Generate(std::mt19937_64&& InGenerator, RVoxelSurface&& InPlane, const DirectX::XMFLOAT4X4& InPlaneToModelSpace, const DirectX::XMFLOAT3& InPlaneExtrusionDir, const bool bInFaceClockwise, std::vector<TVertex>& InOutVertices, std::vector<TUintVertIdx>& InOutTirangles)
		{

			DirectX::XMFLOAT4X4 TransposeMatrix = mds::RMath::ExtractTranspostionMatrix(InPlaneToModelSpace);
			DirectX::XMFLOAT3 TranslationVector = mds::RMath::ExtractTranslationVector(InPlaneToModelSpace);
			DirectX::XMMATRIX loadedTransposeMatrix = DirectX::XMLoadFloat4x4(&TransposeMatrix);

			DirectX::XMFLOAT3 TranspositionScale
			{
				DirectX::XMVectorGetX(DirectX::XMVector3Length(loadedTransposeMatrix.r[0])), //scale applied to X by transposition
				DirectX::XMVectorGetX(DirectX::XMVector3Length(loadedTransposeMatrix.r[1])), //scale applied to Y by transposition
				DirectX::XMVectorGetX(DirectX::XMVector3Length(loadedTransposeMatrix.r[2]))  //scale applied to Z by transposition
			};
			//DirectX::XMVECTOR loadedDepthVector{ 0.f, 0.f, 1.f, 1.f };
			//loadedDepthVector = DirectX::XMVector3Transform(loadedDepthVector, loadedTransposeMatrix);
			//if (bInFaceClockwise)
			//{
			//	loadedDepthVector = loadedDepthVector * -1.f;
			//}
			//loadedDepthVector = XMVector3Normalize(loadedDepthVector);
			DirectX::XMFLOAT3 depthVector = InPlaneExtrusionDir;
			DirectX::XMVECTOR loadedPlaneExtrusionDir = XMLoadFloat3(&InPlaneExtrusionDir);
			loadedPlaneExtrusionDir = XMVector3Normalize(loadedPlaneExtrusionDir);
			DirectX::XMStoreFloat3(&depthVector, loadedPlaneExtrusionDir);

			mp_context = std::make_unique<RGContext>
				(
					RGContext{
					.Gen = std::move(InGenerator),
					.Vertexes = InOutVertices,
					.Triangles = InOutTirangles,
					.DefaultTranspose = TransposeMatrix,
					.bFaceClockwise = bInFaceClockwise,
					.DefaultScale = TranspositionScale,
					.DefaultTranslate = TranslationVector,
					.DepthAxis = depthVector,
					.MemoryTreeRoot = std::make_shared<RGMemoryTree>()
					});

			mp_context->MemoryTreeRoot->ParentElement.reset(); //explicit parent null for root
			mp_context->MemoryTreeRoot->ParentCoordVoxelSurface = RVoxelSurface();
			mp_context->MemoryTreeRoot->SelfCoordVoxelSurface = std::move(InPlane);
			mp_context->MemoryTreeRoot->SurfaceData = RGSurfaceData
			{
				.TransposeMatrix = InPlaneToModelSpace,
				.bFaceClockwise = bInFaceClockwise,
				.DepthAxis = depthVector,
				.ParentCoordSurfaceData = RVoxelSurface::RVoxelSurfaceData(),
				.SelfCoordSurfaceData = mp_context->MemoryTreeRoot->SelfCoordVoxelSurface.ComputeVoxelSurfaceData()
			};
			if (mp_context->MemoryTreeRoot->SurfaceData.SelfCoordSurfaceData.EffectiveShapePartCount > 0)
			{
				GreebleSurface(mp_context->MemoryTreeRoot);
			}

			mp_context.reset();
		};

		RGREEBLE_TEMPLATE
		std::vector<std::unique_ptr<RVoxelSurface>> RGREEBLE::SubdivideSurface(SPMemTree InParent) 
		{
			static constexpr uint16_t MAX_GREEBLE_SUBDIVIDE = 1;
			std::mt19937_64& gen = mp_context->Gen;
			const RGSurfaceData& surfaceData = InParent->SurfaceData;
			std::uniform_int_distribution<uint16_t> dist
			(
				1,
				//Limiting to EffectiveShapePartCount is a hard limit until starting to subdivide VXLs
				std::min(MAX_GREEBLE_SUBDIVIDE, (uint16_t)surfaceData.SurfaceData.EffectiveShapePartCount)
			);

			uint16_t maxNBChildSurface = dist(gen);
			std::vector<std::unique_ptr<RVoxelSurface>> childSurfaces;
			childSurfaces.reserve(maxNBChildSurface);

			//Have a std::unordered_set<XMINT2> taken coords, and start drowing clockwise contour until i =  max or all tiles in set
			//if max > 1 then it is good to avoid taking all tiles in a single contour to augment diversity
			for (uint16_t i = 0; i < maxNBChildSurface; ++i)
			{
				//childSurfaces
			}

			return childSurfaces;
		}

		RGREEBLE_TEMPLATE
		void RGREEBLE::SubdivideSurfaceByTileGrowth(SPMemTree InParent)
		{
			std::mt19937_64& gen = mp_context->Gen;
			RVoxelSurface& pS = InParent->SelfCoordVoxelSurface;
			const RGSurfaceData& pSD = InParent->SurfaceData;

			size_t nbshapeTiles = (uint16_t)pSD.SelfCoordSurfaceData.EffectiveShapePartCount;

			std::unordered_set<XMINT2, XMINT2Hash, XMINT2Equal> remainingTiles;
			pS.FillShapePartsSet(/*InOut*/ remainingTiles);

			/*starting at 8 a tiledshape can have holes*/
			std::uniform_int_distribution<uint16_t> nbTilesDistribution(1, 6);
			static std::uniform_int_distribution<uint16_t> nbTilesDir4Distribution(0, _countof(mds::DIRECTIONS2D_4) - 1);
			std::uniform_real_distribution<float> extrusionHeightDist(0.05f, 0.40f);
			std::uniform_int_distribution<int32_t> tileDefRatioDist(2, 2);

			int32_t MAX_CHILD = 9999999;
			while (!remainingTiles.empty() && InParent->ChildElements.size() < MAX_CHILD)
			{
				InParent->ChildElements.push_back(std::make_shared<RGMemoryTree>());
				SPMemTree newMemTree = InParent->ChildElements.back();
				newMemTree->ParentElement = InParent;
				RVoxelSurface& newPC_VS = newMemTree->ParentCoordVoxelSurface = RVoxelSurface
				{
					.Gobal_VXL_ToLocal = pS.Gobal_VXL_ToLocal,
					.DepthLevel = (uint8_t)pS.DepthLevel,
					.MinCoord = pS.MinCoord,
					.MaxCoord = pS.MaxCoord
				};
				RVoxelSurface& newSC_VS = newMemTree->SelfCoordVoxelSurface = RVoxelSurface
				{
					.Gobal_VXL_ToLocal = pS.Gobal_VXL_ToLocal,
					.DepthLevel = (uint8_t)(pS.DepthLevel + (uint8_t)1),
					//.MinCoord = pS.MinCoord,
					//.MaxCoord = pS.MaxCoord
					
				};

				size_t cRemainingTiles = remainingTiles.size();
				XMINT2 coord = *remainingTiles.begin();
				size_t targetTileCount = std::min((size_t)nbTilesDistribution(gen), cRemainingTiles);

				//First tile
				newPC_VS.CoordToTileData.insert({ coord, RTileData{.bIsShapePart = true } });
				remainingTiles.erase(coord);

				//Optional additional tiles
				for (size_t i = 1; i < targetTileCount; ++i)
				{
					XMINT2 attemptGrowDir = mds::DIRECTIONS2D_4[nbTilesDir4Distribution(gen)];

					//From begin in map, we advant by the max number of tiles modulo the current size using random generator
					auto it = newPC_VS.CoordToTileData.begin();
					std::advance(it, nbTilesDistribution(gen) % newPC_VS.CoordToTileData.size());

					XMINT2 attemptGrowFrom = it->first;

					XMINT2 newTargetTile = mds::unoptimized_add2(attemptGrowFrom, attemptGrowDir);
					if (/*pS[newTargetTile].bIsShapePart &&*/ remainingTiles.contains(newTargetTile))
					{
						newPC_VS.CoordToTileData.insert({ newTargetTile, RTileData{ .bIsShapePart = true } });
						remainingTiles.erase(newTargetTile);
					}
				}
				newMemTree->SurfaceData = RGSurfaceData
				{
					.TransposeMatrix = pSD.TransposeMatrix,
					.bFaceClockwise = pSD.bFaceClockwise,
					.DepthAxis = pSD.DepthAxis,
					.ParentCoordSurfaceData = newMemTree->ParentCoordVoxelSurface.ComputeVoxelSurfaceData(),
					.SelfCoordSurfaceData = RVoxelSurface::RVoxelSurfaceData()
				};

				newMemTree->SurfaceData.ExtrusionHeight				= extrusionHeightDist(gen);
				newMemTree->SurfaceData.TileDefRatioRelatParent		= tileDefRatioDist(gen);
				//newMemTree->SurfaceData.TileDefRatioRelatParent = 1;

				XMVECTOR depthAxisLVec = XMLoadFloat3(&newMemTree->SurfaceData.DepthAxis);
				XMMATRIX trsposeLMat = XMLoadFloat4x4(&newMemTree->SurfaceData.TransposeMatrix);

				XMVECTOR extrusionVector = depthAxisLVec * newMemTree->SurfaceData.ExtrusionHeight;

				//ensure vector status
				extrusionVector = XMVectorSetW(extrusionVector, 0.f);

				//Origin for matrix is moved by height amount along the depth axis
				trsposeLMat.r[3] = trsposeLMat.r[3] + extrusionVector;


				int32_t iParentToChildCoordScale= newMemTree->SurfaceData.TileDefRatioRelatParent;
				float fParentToChildCoordScale = (float)iParentToChildCoordScale;
				float childToparentCoordScale = 1.f / fParentToChildCoordScale;

				//scale tile coord to model space transposition
				trsposeLMat.r[0] = trsposeLMat.r[0] * childToparentCoordScale;
				trsposeLMat.r[1] = trsposeLMat.r[1] * childToparentCoordScale;
				trsposeLMat.r[2] = trsposeLMat.r[2] * childToparentCoordScale;

				XMVECTOR childTileHalfScaleLVec = XMVECTOR{ -0.5f, -0.5f, 0.f, 0.f };
				childTileHalfScaleLVec = XMVector4Transform(childTileHalfScaleLVec, trsposeLMat);

				//Origin for matrix is moved by half the child tile width and height toward the bottom left
				trsposeLMat.r[3] = trsposeLMat.r[3] + childTileHalfScaleLVec;


				XMStoreFloat4x4(&newMemTree->SurfaceData.TransposeMatrix, trsposeLMat);
				DuplicateParentTileSetWithScale(newMemTree, false);
				GenerateContourAndChildTile(newMemTree);

				//for (const auto& lineData = newMemTree->SurfaceData.)
				//float extrudeMe
				//Generate contour
			}
			//return subDivisions;
		}

		RGREEBLE_TEMPLATE
		void RGREEBLE::DuplicateParentTileSetWithScale(SPMemTree InChildMem, const bool bInComputeSurfaceData)
		{
			if (InChildMem->ParentElement.expired())
			{
				assert("Cannot generate a contour for a child element without parent !");
				return;
			}
			SPMemTree ParentMem = InChildMem->ParentElement.lock();
			RVoxelSurface& cPC_VS = InChildMem->ParentCoordVoxelSurface;
			RVoxelSurface& cSC_VS = InChildMem->SelfCoordVoxelSurface;

			RGSurfaceData& cSD = InChildMem->SurfaceData;

			int32_t iPtChildCoord = cSD.TileDefRatioRelatParent;
			cSC_VS.MinCoord = unoptimized_multiply2(cPC_VS.MinCoord, iPtChildCoord);
			cSC_VS.MaxCoord = unoptimized_add2(unoptimized_multiply2(cPC_VS.MaxCoord, iPtChildCoord), XMINT2{ 1, 1 });

			cSC_VS.CoordToTileData.clear();

			//yes, tile growth is very steep
			cSC_VS.CoordToTileData.reserve((size_t)std::pow<size_t>(cPC_VS.CoordToTileData.size(), (size_t)iPtChildCoord));

			for (const auto& pair : cPC_VS.CoordToTileData)
			{
				XMINT2 cMin = mds::unoptimized_multiply2(pair.first, iPtChildCoord);

				XMINT2 cMax = mds::unoptimized_multiply2
				(
					mds::unoptimized_add2(pair.first, XMINT2{ 1, 1 }),
					iPtChildCoord
				);
				for (int32_t x = cMin.x; x < cMax.x; ++x)
				{
					for (int32_t y = cMin.y; y < cMax.y; ++y)
					{
						cSC_VS.CoordToTileData.insert({ XMINT2{x, y}, pair.second });
					}
				}
			}
			if (bInComputeSurfaceData)
			{
				cSD.SelfCoordSurfaceData = cSC_VS.ComputeVoxelSurfaceData();
			}
		}

		RGREEBLE_TEMPLATE
		void RGREEBLE::GenerateContourAndChildTile(SPMemTree InChildMem)
		{
			if (InChildMem->ParentElement.expired())
			{
				assert("Cannot generate a contour for a child element without parent !");
				return;
			}
			SPMemTree ParentMem = InChildMem->ParentElement.lock();
			constexpr XMINT2Different XMINT2Diff;
			//		       FRWD
			//		     _________
			//		   frwdL | frwdR
			//		LEFT ____|____ RIGHT
			//		   backL | backR
			//		     ____|____
			//		        BACK
			RVoxelSurface& cSC_VS = InChildMem->SelfCoordVoxelSurface;
			RVoxelSurface& cPC_VS = InChildMem->ParentCoordVoxelSurface;

			//RGSurfaceData& pSD = ParentMem->SurfaceData;
			RGSurfaceData& cSD = InChildMem->SurfaceData;

			RGContourData& cPC_CD = cSD.ParentContourData;
			RGContourData& cSC_CD = cSD.ChildContourData;

			std::vector<RGLineData>& pLines = cPC_CD.Lines;
			std::vector<RGLineData>& cLines = cSC_CD.Lines;

			//We generate the contour using the parent VoxelSurface, then generate the corresponding Child surface


			int32_t iPtChildCoord = cSD.TileDefRatioRelatParent;


			XMINT2 startCoord = cPC_VS.MaxCoord;
			//Search Leftmost, min tile
			for (const auto& Pair : cPC_VS.CoordToTileData)
			{
				if (Pair.first.y < startCoord.y || (Pair.first.x < startCoord.x && Pair.first.y == startCoord.y))
				{
					startCoord = Pair.first;
				}
			}
			bool bIsFirstLine = true;
			XMINT2 prevDir = mds::DIR2D_LEFT;
			XMINT2 nextCoord = startCoord;

			static const std::unordered_map<XMINT2, XMINT2, XMINT2Hash, XMINT2Equal> DIR_TO_FRWD_RIGHT_TILE =
			{
				{
					DIR2D_TOP	, XMINT2{0, 0}
				},
				{
					DIR2D_RIGHT	, XMINT2{-1, 0}
				},
				{
					DIR2D_BOTTOM, XMINT2{-1, -1}
				},
				{
					DIR2D_LEFT	, XMINT2{0, -1}
				}
			};
			do
			{
				size_t currentLineCount = pLines.size();
				XMINT2 currentCoord = nextCoord;

				XMINT2 frwrdVec = mds::RMath2D::ForwardVector(prevDir);
				XMINT2 rightVec = mds::RMath2D::RightVector(prevDir);
				XMINT2 backVec = mds::RMath2D::BackVector(prevDir);
				XMINT2 leftVec = mds::RMath2D::LeftVector(prevDir);

				XMINT2 frwdRightTile = mds::unoptimized_add2(currentCoord, DIR_TO_FRWD_RIGHT_TILE.at(prevDir));
				XMINT2 frwdLeftTile = mds::unoptimized_add2(frwdRightTile, leftVec);

				XMINT2 backRightTile = mds::unoptimized_add2(currentCoord, backVec);
				XMINT2 backLeftTile = mds::unoptimized_add2(backRightTile, leftVec);

				bool bFrwdRight = cPC_VS[frwdRightTile].bIsShapePart;
				bool bFrwdLeft = cPC_VS[frwdLeftTile].bIsShapePart;
#if _DEBUG
				bool bBackRight = cPC_VS[backRightTile].bIsShapePart;
				bool bBackLeft = cPC_VS[backLeftTile].bIsShapePart;
				//assert(bBackRight && "Back Right is always suppose to be true !");
				//assert(!bBackLeft && "Back Left is always suppose to be false !");
#else
				bool bBackRight = true;
				bool bBackLeft = false;
#endif // _DEBUG
				XMINT2 ChoosenDirVec;
				bool bIsForward = false;
				//Go Right
				if (!bFrwdRight)
				{
					ChoosenDirVec = rightVec;
					nextCoord = mds::unoptimized_add2(currentCoord, rightVec);
				}
				//Go Left
				else if (bFrwdLeft)
				{
					ChoosenDirVec = leftVec;
					nextCoord = mds::unoptimized_add2(currentCoord, leftVec);
				}
				//Go Forward
				else
				{
					bIsForward = true;
					ChoosenDirVec = frwrdVec;
					nextCoord = mds::unoptimized_add2(currentCoord, frwrdVec);
				}

				if (bIsForward && !bIsFirstLine)
				{
					pLines[currentLineCount - 1].EndCoord = nextCoord;
					cLines[currentLineCount - 1].EndCoord = cSD.ParentToChildPos(nextCoord/*, ChoosenDirVec*/);
				}
				else
				{
					pLines.push_back
					(
						RGLineData
						{
							.StartCoord = currentCoord,
							.EndCoord = nextCoord
						}
					);
					cLines.push_back
					(
						RGLineData
						{
							.StartCoord = cSD.ParentToChildPos(currentCoord/*, ChoosenDirVec*/),
							.EndCoord = cSD.ParentToChildPos(nextCoord/*, ChoosenDirVec*/)
						}
					);
				}
				prevDir = mds::unoptimized_substract2(nextCoord, currentCoord);
				bIsFirstLine = false;
			} while (XMINT2Diff(nextCoord, startCoord));


			const std::unordered_map<XMINT2, XMINT2, XMINT2Hash, XMINT2Equal> FRWRD_TO_RIGHT_CHAMFER =
			{
				{
					DIR2D_TOP	, XMINT2{0, +1}
				},
				{
					DIR2D_RIGHT	, XMINT2{0, 0}
				},
				{
					DIR2D_BOTTOM, XMINT2{0, 0}
				},
				{
					DIR2D_LEFT	, XMINT2{1, 0}
				}
			};
			auto ChamferVector = [&FRWRD_TO_RIGHT_CHAMFER](const int32_t InChildVXLChamfer, const XMINT2& LineForward) -> XMINT2
				{
					return mds::unoptimized_add2
					(
						mds::unoptimized_multiply2(mds::RMath2D::RightVector(LineForward), (InChildVXLChamfer - 1)),
						FRWRD_TO_RIGHT_CHAMFER.at(LineForward)
					);
				};
			if (cLines.size() > 0)
			{
				const size_t cLineCount = cLines.size();
				//we cache the result as during iteration, currentLine is in an unstalble state form previous move operation
				XMINT2 cached_iLineForward = cLines[0].ForwardVector();
				XMINT2 cached_i0LineForward = cached_iLineForward;
				for (size_t i = 0; i < cLineCount; ++i)
				{
					size_t i_next = (i + 1) % cLineCount;
					RGLineData& iLine = cLines[i];
					RGLineData& iNextLine = cLines[i_next];

					XMINT2 ChamferiVec = ChamferVector(cSD.ChildVXLChamfer, cached_iLineForward);
					XMINT2 ChamferiNextVec = ChamferVector(cSD.ChildVXLChamfer, (i_next > 0) ? iNextLine.ForwardVector() : cached_i0LineForward);
					XMINT2 TotalChamferVec =
						mds::unoptimized_add2
						(
							ChamferiVec,//ChamferVector(cSD.ChildVXLChamfer, cached_iLineForward),
							ChamferiNextVec//ChamferVector(cSD.ChildVXLChamfer, iNextLine.ForwardVector())
						);
					//prepare nex cached forward vector before editing line
					cached_iLineForward = iNextLine.ForwardVector();
					iLine.EndCoord = iNextLine.StartCoord = mds::unoptimized_add2(iLine.EndCoord, TotalChamferVec);
				}
			}
			if (true)
			{
				int32_t xMin, yMin, xMax, yMax;
				xMin = yMin = INT32_MAX;
				xMax = yMax = INT32_MIN;

				auto UpdateMinMax = [&xMin, &yMin, &xMax, &yMax](const XMINT2& InCoord) -> void
				{
					if (xMin > InCoord.x)
					{
						xMin = InCoord.x;
					}
					if (yMin > InCoord.y)
					{
						yMin = InCoord.y;
					}

					if (xMax < InCoord.x)
					{
						xMax = InCoord.x;
					}
					if (yMax < InCoord.y)
					{
						yMax = InCoord.y;
					}
				};
				//clean rought parent copy in chind tile to take chamfer into account
				for (const RGLineData& Line : cLines)
				{
					UpdateMinMax(Line.StartCoord);
					XMINT2 forwrdVec = Line.ForwardVector();
					XMINT2 rightVec = Line.RightVector();
					XMINT2 leftVec = mds::unoptimized_multiply2(Line.RightVector(), -1);

					//Probably a problem for iter = Start or End
					for (XMINT2 iter = Line.StartCoord; XMINT2Diff(iter, Line.EndCoord); iter = mds::unoptimized_add2(iter, forwrdVec))
					{
						XMINT2 frwdRightTile = mds::unoptimized_add2(iter, DIR_TO_FRWD_RIGHT_TILE.at(forwrdVec));
						XMINT2 frwdLeftTile = mds::unoptimized_add2(frwdRightTile, leftVec);
						for (int32_t i = 1; i <= cSD.ChildVXLChamfer; ++i)
						{
							XMINT2 rightPos = mds::unoptimized_add2(frwdRightTile, mds::unoptimized_multiply2(rightVec, i - 1));
							XMINT2 leftPos = mds::unoptimized_add2(frwdLeftTile, mds::unoptimized_multiply2(leftVec, i - 1));
							if (cSC_VS.CoordToTileData.contains(leftPos))
							{
								cSC_VS.CoordToTileData.erase(leftPos);
							}
							if (cSC_VS.CoordToTileData.contains(rightPos))
							{
								cSC_VS.CoordToTileData[rightPos].bIsShapePart = true;
							}
							else
							{
								cSC_VS.CoordToTileData.insert({ rightPos , RTileData{.bIsShapePart = true} });
							}
						}
					}
				}
				//some tiles may not be cached by contour cleaning algorythm, especially around the start of the line, these tiles are
				//generally out of minMax of the line
				std::unordered_set<XMINT2, XMINT2Hash, XMINT2Equal> outOfBounds;
				outOfBounds.reserve(5);//5 is arbitrary
				for (auto& pair : cSC_VS.CoordToTileData)
				{
					if
						(
							pair.first.x < xMin || pair.first.x >= xMax ||
							pair.first.y < yMin || pair.first.y >= yMax
							)
					{
						outOfBounds.insert(pair.first);
					}
				}
				for (const auto& coord : outOfBounds)
				{
					cSC_VS.CoordToTileData.erase(coord);
				}
			}
			cSD.SelfCoordSurfaceData = cSC_VS.ComputeVoxelSurfaceData();
		}

		RGREEBLE_TEMPLATE
		void RGREEBLE::DrawContour(SPMemTree InMemLeaf)
		{
			if (InMemLeaf->ParentElement.expired())
			{
				return;
			}
			SPMemTree parentMem = InMemLeaf->ParentElement.lock();

			DirectX::XMFLOAT4X4 cTRS = InMemLeaf->SurfaceData.TransposeMatrix;
			DirectX::XMFLOAT4X4 pTRS = parentMem->SurfaceData.TransposeMatrix;

			XMMATRIX cVrtxToMsh = XMLoadFloat4x4(&cTRS);
			XMMATRIX pVrtxToMsh = XMLoadFloat4x4(&pTRS);

			RGSurfaceData& cSD = InMemLeaf->SurfaceData;

			std::vector<TVertex>& Verts = mp_context->Vertexes;
			std::vector<TUintVertIdx>& Tris = mp_context->Triangles;


			RGContourData& cPC_CD = cSD.ParentContourData;
			RGContourData& cSC_CD = cSD.ChildContourData;

			std::vector<RGLineData>& pLines = cPC_CD.Lines;
			std::vector<RGLineData>& cLines = cSC_CD.Lines;

			Verts.reserve(Verts.size() + (cLines.size() * 4));
			Tris.reserve(Tris.size() + (cLines.size() * 6));

			auto HandleVertice = [this, &Verts, &Tris](XMMATRIX* vrtxToMsh, const XMINT2 InCoord2D, const XMFLOAT2 InNormal2D, TUintVertIdx& OutVerticeID) ->void
			{
				OutVerticeID = (TUintVertIdx)Verts.size();

				XMVECTOR LoadedPos = DirectX::XMVectorSet((float)InCoord2D.x, (float)InCoord2D.y, 0.f, 1.f);

				XMVECTOR res = DirectX::XMVector3Transform(LoadedPos, *vrtxToMsh);
				XMFLOAT3 verticePos;
				DirectX::XMStoreFloat3(&verticePos, res);
				//LoadedPos = vrtxToMsh * LoadedPos;
				Verts.push_back(this->m_vertexProvider.TVertexFromNormVertAndSurNormCoord
				(
					m_vertexProvider.SurfaceNormal,
					verticePos,
					InNormal2D
				));
			};
			for (size_t i = 0; i < cLines.size(); ++i)
			{
				const RGLineData& pLineData = pLines[i];
				const RGLineData& cLineData = cLines[i];
				constexpr XMFLOAT2 startPLineUV = XMFLOAT2{ 0.f, 0.f };
				constexpr XMFLOAT2 endPLineUV	= XMFLOAT2{ 1.f, 0.f };
				constexpr XMFLOAT2 startCLineUV = XMFLOAT2{ 0.f, 1.f };
				constexpr XMFLOAT2 endCLineUV	= XMFLOAT2{ 1.f, 1.f };

				TUintVertIdx pMin, pMax, cMin, cMax;

				HandleVertice(&pVrtxToMsh, pLineData.StartCoord	, startPLineUV	, pMin);
				HandleVertice(&pVrtxToMsh, pLineData.EndCoord	, endPLineUV	, pMax);

				HandleVertice(&cVrtxToMsh, cLineData.StartCoord	, startCLineUV	, cMin);
				HandleVertice(&cVrtxToMsh, cLineData.EndCoord	, endCLineUV	, cMax);

				size_t startTriIdx = Tris.size();
				Tris.resize(startTriIdx + 6);

				bool revrs = !mp_context->bFaceClockwise;
				Tris[startTriIdx + (revrs ? 2 : 0)] = pMax;
				Tris[startTriIdx + 1] = cMax;
				Tris[startTriIdx + (revrs ? 0 : 2)] = pMin;

				Tris[startTriIdx + (revrs ? 5 : 3)] = cMax;
				Tris[startTriIdx + 4] = cMin;
				Tris[startTriIdx + (revrs ? 3 : 5)] = pMin;
			}
		};

		RGREEBLE_TEMPLATE
		void RGREEBLE::DrawFinalSurface(SPMemTree InMemLeaf) 
		{
			DirectX::XMFLOAT4X4 trs = InMemLeaf->SurfaceData.TransposeMatrix;
			
			bool bDebugUsingParent = false;
			RVoxelSurface* pVoxalSurf = &InMemLeaf->SelfCoordVoxelSurface;
			if (bDebugUsingParent && !InMemLeaf->ParentElement.expired())
			{
				trs = InMemLeaf->ParentElement.lock()->SurfaceData.TransposeMatrix;
				pVoxalSurf = &InMemLeaf->ParentCoordVoxelSurface;
			}
			else if (bDebugUsingParent)
			{
				return;
			}
			else
			{
				pVoxalSurf = &InMemLeaf->SelfCoordVoxelSurface;
			}
			RVoxelSurface& voxelSurface = *pVoxalSurf;

			XMMATRIX vrtxToMsh = XMLoadFloat4x4(&trs);

			XMVECTOR minPlane = DirectX::XMVectorSet((float)voxelSurface.MinCoord.x, (float)voxelSurface.MinCoord.y, 0.f, 0.f);
			XMVECTOR maxPlane = DirectX::XMVectorSet((float)voxelSurface.MaxCoord.x, (float)voxelSurface.MaxCoord.y, 0.f, 0.f);
			XMVECTOR sizePlane = maxPlane - minPlane;

			std::vector<TVertex>& Verts = mp_context->Vertexes;
			std::vector<TUintVertIdx>& Tris = mp_context->Triangles;

			std::vector<RGreedyMesh::XMQuads> quads;
			RGreedyMesh::GenerateSurfaceQuads(voxelSurface, quads, true);
			size_t cQuads = quads.size();

			std::unordered_map<XMINT2, TUintVertIdx, XMINT2Hash, XMINT2Equal> coordToVerticeIdx;

			Verts.reserve(Verts.size() + (cQuads * 4));
			Tris.reserve(Tris.size() + (cQuads * 6));

			auto HandleVertice = [this, &Verts, &Tris, &coordToVerticeIdx, &voxelSurface, &vrtxToMsh, &minPlane, &sizePlane/*, &normalizer, &DEFAULT_COLOR, &UseTextureIndex*/](const XMINT2 InCoord2D, TUintVertIdx& OutVerticeID) ->void
				{
					auto NormalizeCoord = [&minPlane, &sizePlane](const XMINT2& InCoord) -> XMFLOAT2
						{
							XMVECTOR LoadedCoord = DirectX::XMVectorSet((float)InCoord.x, (float)InCoord.y, 0.f, 0.f);
							XMFLOAT2 result;
							XMStoreFloat2(&result, (LoadedCoord - minPlane) / sizePlane);
							return result;
						};
					if (coordToVerticeIdx.contains(InCoord2D)) { OutVerticeID = coordToVerticeIdx[InCoord2D]; }
					else
					{
						OutVerticeID = (TUintVertIdx)Verts.size();
						coordToVerticeIdx.insert({ InCoord2D, OutVerticeID });

						XMVECTOR LoadedPos = DirectX::XMVectorSet((float)InCoord2D.x, (float)InCoord2D.y, 0.f, 1.f);



						XMVECTOR res = DirectX::XMVector3Transform(LoadedPos, vrtxToMsh);
						XMFLOAT3 verticePos;
						DirectX::XMStoreFloat3(&verticePos, res);
						//LoadedPos = vrtxToMsh * LoadedPos;
						Verts.push_back(this->m_vertexProvider.TVertexFromNormVertAndSurNormCoord
						(
							m_vertexProvider.SurfaceNormal,
							verticePos,
							NormalizeCoord(InCoord2D)
						));
					}
				};

			for (const RGreedyMesh::XMQuads& quad : quads)
			{

				TUintVertIdx minXminY, maxXminY, minXmaxY, maxXmaxY;

				XMINT2 q_minXminY = quad.Min;
				XMINT2 q_maxXminY = { quad.Min.x + quad.SizeXY.x,		quad.Min.y };
				XMINT2 q_minXmaxY = { quad.Min.x,						quad.Min.y + quad.SizeXY.y };
				XMINT2 q_maxXmaxY = { quad.Min.x + quad.SizeXY.x,		quad.Min.y + quad.SizeXY.y };
				HandleVertice(q_minXminY, /*Out*/minXminY);
				HandleVertice(q_maxXminY, /*Out*/maxXminY);
				HandleVertice(q_minXmaxY, /*Out*/minXmaxY);
				HandleVertice(q_maxXmaxY, /*Out*/maxXmaxY);

				size_t startTriIdx = Tris.size();
				Tris.resize(startTriIdx + 6);

				bool revrs = !mp_context->bFaceClockwise;
				Tris[startTriIdx + (revrs ? 2 : 0)] = minXminY;
				Tris[startTriIdx + 1] = maxXminY;
				Tris[startTriIdx + (revrs ? 0 : 2)] = minXmaxY;

				Tris[startTriIdx + (revrs ? 5 : 3)] = minXmaxY;
				Tris[startTriIdx + 4] = maxXminY;
				Tris[startTriIdx + (revrs ? 3 : 5)] = maxXmaxY;
			}
		}
	};
};


#ifdef RGREEBLE_TEMPLATE
#undef RGREEBLE_TEMPLATE
#endif // RGREEBLE_TEMPLATE

#ifdef RGREEBLE
#undef RGREEBLE
#endif // RGREEBLE