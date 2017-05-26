#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/8ec51d02/ASTCReader.o \
	${OBJECTDIR}/_ext/8ec51d02/ApplicationDelegate.o \
	${OBJECTDIR}/_ext/8ec51d02/ApplicationSettings.o \
	${OBJECTDIR}/_ext/8ec51d02/ApplicationViewport.o \
	${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDelegate.o \
	${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDescription.o \
	${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportsController.o \
	${OBJECTDIR}/_ext/8ec51d02/Application_General.o \
	${OBJECTDIR}/_ext/8ec51d02/Asset.o \
	${OBJECTDIR}/_ext/8ec51d02/AssetClass.o \
	${OBJECTDIR}/_ext/8ec51d02/AssetClassFileReader.o \
	${OBJECTDIR}/_ext/8ec51d02/AssetCountsSettings.o \
	${OBJECTDIR}/_ext/8ec51d02/AssetFileReader.o \
	${OBJECTDIR}/_ext/8ec51d02/AssetPath.o \
	${OBJECTDIR}/_ext/8ec51d02/AssetReadHandle.o \
	${OBJECTDIR}/_ext/8ec51d02/AssetReadQueue.o \
	${OBJECTDIR}/_ext/8ec51d02/AssetReference.o \
	${OBJECTDIR}/_ext/8ec51d02/Camera.o \
	${OBJECTDIR}/_ext/8ec51d02/CameraState.o \
	${OBJECTDIR}/_ext/8ec51d02/DDSReader.o \
	${OBJECTDIR}/_ext/8ec51d02/DXGIIncludes.o \
	${OBJECTDIR}/_ext/8ec51d02/FinjinPrecompiled.o \
	${OBJECTDIR}/_ext/8ec51d02/FinjinSceneAssets.o \
	${OBJECTDIR}/_ext/8ec51d02/FinjinSceneReader.o \
	${OBJECTDIR}/_ext/8ec51d02/FlyingCameraInputBindings.o \
	${OBJECTDIR}/_ext/8ec51d02/GenericGpuNumericStructs.o \
	${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonImpl.o \
	${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonSettings.o \
	${OBJECTDIR}/_ext/8ec51d02/GpuRenderTargetSize.o \
	${OBJECTDIR}/_ext/8ec51d02/InputComponents.o \
	${OBJECTDIR}/_ext/8ec51d02/InputConstants.o \
	${OBJECTDIR}/_ext/8ec51d02/InputContextCommonSettings.o \
	${OBJECTDIR}/_ext/8ec51d02/InputCoordinate.o \
	${OBJECTDIR}/_ext/8ec51d02/InputSource.o \
	${OBJECTDIR}/_ext/8ec51d02/JobPipelineStage.o \
	${OBJECTDIR}/_ext/8ec51d02/KTXReader.o \
	${OBJECTDIR}/_ext/8ec51d02/LightState.o \
	${OBJECTDIR}/_ext/8ec51d02/MemorySettings.o \
	${OBJECTDIR}/_ext/8ec51d02/PVRReader.o \
	${OBJECTDIR}/_ext/8ec51d02/PlatformCapabilities_General.o \
	${OBJECTDIR}/_ext/8ec51d02/ShaderFeatureFlag.o \
	${OBJECTDIR}/_ext/8ec51d02/ShaderFeatures.o \
	${OBJECTDIR}/_ext/8ec51d02/SoundContextCommonSettings.o \
	${OBJECTDIR}/_ext/8ec51d02/SoundFormat.o \
	${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentChunkNames.o \
	${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyNames.o \
	${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyValues.o \
	${OBJECTDIR}/_ext/8ec51d02/StandardGameControllerInputBindings.o \
	${OBJECTDIR}/_ext/8ec51d02/StringTable.o \
	${OBJECTDIR}/_ext/8ec51d02/TextureDimension.o \
	${OBJECTDIR}/_ext/8ec51d02/UserDataTypes.o \
	${OBJECTDIR}/_ext/8ec51d02/VRContextCommonSettings.o \
	${OBJECTDIR}/_ext/8ec51d02/VRGameControllerInputBindings.o \
	${OBJECTDIR}/_ext/8ec51d02/WAVReader.o \
	${OBJECTDIR}/_ext/8ec51d02/WindowBounds.o \
	${OBJECTDIR}/_ext/8ec51d02/WindowSize.o \
	${OBJECTDIR}/_ext/40bb3adf/Application.o \
	${OBJECTDIR}/_ext/40bb3adf/ApplicationGlobals.o \
	${OBJECTDIR}/_ext/40bb3adf/DisplayInfo.o \
	${OBJECTDIR}/_ext/40bb3adf/FinjinMain.o \
	${OBJECTDIR}/_ext/40bb3adf/OSWindow.o \
	${OBJECTDIR}/_ext/40bb3adf/PlatformCapabilities.o \
	${OBJECTDIR}/_ext/40bb3adf/XcbConnection.o \
	${OBJECTDIR}/_ext/40bb3adf/XcbWindow.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanDescriptorSetLayout.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanFrameBuffer.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanFrameStage.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanGpuBuffer.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanGpuContext.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextImpl.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextSettings.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanGpuDescription.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanIncludes.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanInputFormat.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanLayerProperties.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanLight.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanMaterial.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanMemoryResource.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanMesh.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanPipeline.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanPipelineBuilder.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanPipelineLayout.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanPresentModes.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanPrimaryQueues.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanQueueFamilyIndexes.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanRenderTarget.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanResources.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanShader.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanSimpleWork.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanStaticMeshRenderer.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanSurfaceFormats.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanSwapChainFrameBuffer.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanSystem.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanSystemImpl.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanSystemSettings.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanTexture.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanUtilities.o \
	${OBJECTDIR}/_ext/32e60500/LinuxGameController.o \
	${OBJECTDIR}/_ext/32e60500/LinuxInputContext.o \
	${OBJECTDIR}/_ext/32e60500/LinuxInputSystem.o \
	${OBJECTDIR}/_ext/32e60500/LinuxKeyboard.o \
	${OBJECTDIR}/_ext/32e60500/LinuxMouse.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALAdapterDescription.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALContext.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALContextImpl.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALEffect.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALEffectSlot.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALExtensions.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALFilter.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALListener.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALSoundBuffer.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALSoundSource.o \
	${OBJECTDIR}/_ext/b85d39ba/OpenALSystem.o \
	${OBJECTDIR}/_ext/b0d4447f/b3DynamicBvh.o \
	${OBJECTDIR}/_ext/b0d4447f/b3DynamicBvhBroadphase.o \
	${OBJECTDIR}/_ext/b0d4447f/b3OverlappingPairCache.o \
	${OBJECTDIR}/_ext/ccaef08c/b3ConvexUtility.o \
	${OBJECTDIR}/_ext/ccaef08c/b3CpuNarrowPhase.o \
	${OBJECTDIR}/_ext/6792bd68/b3AlignedAllocator.o \
	${OBJECTDIR}/_ext/6792bd68/b3Logging.o \
	${OBJECTDIR}/_ext/6792bd68/b3Vector3.o \
	${OBJECTDIR}/_ext/f9590a6e/b3FixedConstraint.o \
	${OBJECTDIR}/_ext/f9590a6e/b3Generic6DofConstraint.o \
	${OBJECTDIR}/_ext/f9590a6e/b3PgsJacobiSolver.o \
	${OBJECTDIR}/_ext/f9590a6e/b3Point2PointConstraint.o \
	${OBJECTDIR}/_ext/f9590a6e/b3TypedConstraint.o \
	${OBJECTDIR}/_ext/47c35051/b3CpuRigidBodyPipeline.o \
	${OBJECTDIR}/_ext/5fc4b0af/b3ConvexHullComputer.o \
	${OBJECTDIR}/_ext/5fc4b0af/b3GeometryUtil.o \
	${OBJECTDIR}/_ext/32f8a646/btAxisSweep3.o \
	${OBJECTDIR}/_ext/32f8a646/btBroadphaseProxy.o \
	${OBJECTDIR}/_ext/32f8a646/btCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/32f8a646/btDbvt.o \
	${OBJECTDIR}/_ext/32f8a646/btDbvtBroadphase.o \
	${OBJECTDIR}/_ext/32f8a646/btDispatcher.o \
	${OBJECTDIR}/_ext/32f8a646/btOverlappingPairCache.o \
	${OBJECTDIR}/_ext/32f8a646/btQuantizedBvh.o \
	${OBJECTDIR}/_ext/32f8a646/btSimpleBroadphase.o \
	${OBJECTDIR}/_ext/188ee859/SphereTriangleDetector.o \
	${OBJECTDIR}/_ext/188ee859/btActivatingCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btBox2dBox2dCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btBoxBoxCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btBoxBoxDetector.o \
	${OBJECTDIR}/_ext/188ee859/btCollisionDispatcher.o \
	${OBJECTDIR}/_ext/188ee859/btCollisionObject.o \
	${OBJECTDIR}/_ext/188ee859/btCollisionWorld.o \
	${OBJECTDIR}/_ext/188ee859/btCollisionWorldImporter.o \
	${OBJECTDIR}/_ext/188ee859/btCompoundCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btCompoundCompoundCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btConvex2dConvex2dAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btConvexConcaveCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btConvexConvexAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btConvexPlaneCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btDefaultCollisionConfiguration.o \
	${OBJECTDIR}/_ext/188ee859/btEmptyCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btGhostObject.o \
	${OBJECTDIR}/_ext/188ee859/btHashedSimplePairCache.o \
	${OBJECTDIR}/_ext/188ee859/btInternalEdgeUtility.o \
	${OBJECTDIR}/_ext/188ee859/btManifoldResult.o \
	${OBJECTDIR}/_ext/188ee859/btSimulationIslandManager.o \
	${OBJECTDIR}/_ext/188ee859/btSphereBoxCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btSphereSphereCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btSphereTriangleCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/188ee859/btUnionFind.o \
	${OBJECTDIR}/_ext/ec3fb491/btBox2dShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btBoxShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btBvhTriangleMeshShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btCapsuleShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btCollisionShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btCompoundShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btConcaveShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btConeShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btConvex2dShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btConvexHullShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btConvexInternalShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btConvexPointCloudShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btConvexPolyhedron.o \
	${OBJECTDIR}/_ext/ec3fb491/btConvexShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btConvexTriangleMeshShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btCylinderShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btEmptyShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btHeightfieldTerrainShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btMinkowskiSumShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btMultiSphereShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btMultimaterialTriangleMeshShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btOptimizedBvh.o \
	${OBJECTDIR}/_ext/ec3fb491/btPolyhedralConvexShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btScaledBvhTriangleMeshShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btShapeHull.o \
	${OBJECTDIR}/_ext/ec3fb491/btSphereShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btStaticPlaneShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btStridingMeshInterface.o \
	${OBJECTDIR}/_ext/ec3fb491/btTetrahedronShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btTriangleBuffer.o \
	${OBJECTDIR}/_ext/ec3fb491/btTriangleCallback.o \
	${OBJECTDIR}/_ext/ec3fb491/btTriangleIndexVertexArray.o \
	${OBJECTDIR}/_ext/ec3fb491/btTriangleIndexVertexMaterialArray.o \
	${OBJECTDIR}/_ext/ec3fb491/btTriangleMesh.o \
	${OBJECTDIR}/_ext/ec3fb491/btTriangleMeshShape.o \
	${OBJECTDIR}/_ext/ec3fb491/btUniformScalingShape.o \
	${OBJECTDIR}/_ext/6348bb3a/btContactProcessing.o \
	${OBJECTDIR}/_ext/6348bb3a/btGImpactBvh.o \
	${OBJECTDIR}/_ext/6348bb3a/btGImpactCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/6348bb3a/btGImpactQuantizedBvh.o \
	${OBJECTDIR}/_ext/6348bb3a/btGImpactShape.o \
	${OBJECTDIR}/_ext/6348bb3a/btGenericPoolAllocator.o \
	${OBJECTDIR}/_ext/6348bb3a/btTriangleShapeEx.o \
	${OBJECTDIR}/_ext/6348bb3a/gim_box_set.o \
	${OBJECTDIR}/_ext/6348bb3a/gim_contact.o \
	${OBJECTDIR}/_ext/6348bb3a/gim_memory.o \
	${OBJECTDIR}/_ext/6348bb3a/gim_tri_collision.o \
	${OBJECTDIR}/_ext/53a97f85/btContinuousConvexCollision.o \
	${OBJECTDIR}/_ext/53a97f85/btConvexCast.o \
	${OBJECTDIR}/_ext/53a97f85/btGjkConvexCast.o \
	${OBJECTDIR}/_ext/53a97f85/btGjkEpa2.o \
	${OBJECTDIR}/_ext/53a97f85/btGjkEpaPenetrationDepthSolver.o \
	${OBJECTDIR}/_ext/53a97f85/btGjkPairDetector.o \
	${OBJECTDIR}/_ext/53a97f85/btMinkowskiPenetrationDepthSolver.o \
	${OBJECTDIR}/_ext/53a97f85/btPersistentManifold.o \
	${OBJECTDIR}/_ext/53a97f85/btPolyhedralContactClipping.o \
	${OBJECTDIR}/_ext/53a97f85/btRaycastCallback.o \
	${OBJECTDIR}/_ext/53a97f85/btSubSimplexConvexCast.o \
	${OBJECTDIR}/_ext/53a97f85/btVoronoiSimplexSolver.o \
	${OBJECTDIR}/_ext/6f4bfb44/btKinematicCharacterController.o \
	${OBJECTDIR}/_ext/78dd43d5/btConeTwistConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btContactConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btFixedConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btGearConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofSpring2Constraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofSpringConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btHinge2Constraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btHingeConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btNNCGConstraintSolver.o \
	${OBJECTDIR}/_ext/78dd43d5/btPoint2PointConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btSequentialImpulseConstraintSolver.o \
	${OBJECTDIR}/_ext/78dd43d5/btSliderConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btSolve2LinearConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btTypedConstraint.o \
	${OBJECTDIR}/_ext/78dd43d5/btUniversalConstraint.o \
	${OBJECTDIR}/_ext/a9307899/btDiscreteDynamicsWorld.o \
	${OBJECTDIR}/_ext/a9307899/btDiscreteDynamicsWorldMt.o \
	${OBJECTDIR}/_ext/a9307899/btRigidBody.o \
	${OBJECTDIR}/_ext/a9307899/btSimpleDynamicsWorld.o \
	${OBJECTDIR}/_ext/a9307899/btSimulationIslandManagerMt.o \
	${OBJECTDIR}/_ext/e8ada44f/btDantzigLCP.o \
	${OBJECTDIR}/_ext/e8ada44f/btLemkeAlgorithm.o \
	${OBJECTDIR}/_ext/e8ada44f/btMLCPSolver.o \
	${OBJECTDIR}/_ext/7a2b1a07/btRaycastVehicle.o \
	${OBJECTDIR}/_ext/7a2b1a07/btWheelInfo.o \
	${OBJECTDIR}/_ext/458d3eae/IDMath.o \
	${OBJECTDIR}/_ext/458d3eae/MultiBodyTree.o \
	${OBJECTDIR}/_ext/3122dfe1/MultiBodyTreeImpl.o \
	${OBJECTDIR}/_ext/3122dfe1/MultiBodyTreeInitCache.o \
	${OBJECTDIR}/_ext/8f7034e2/btDefaultSoftBodySolver.o \
	${OBJECTDIR}/_ext/8f7034e2/btSoftBody.o \
	${OBJECTDIR}/_ext/8f7034e2/btSoftBodyConcaveCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/8f7034e2/btSoftBodyHelpers.o \
	${OBJECTDIR}/_ext/8f7034e2/btSoftBodyRigidBodyCollisionConfiguration.o \
	${OBJECTDIR}/_ext/8f7034e2/btSoftMultiBodyDynamicsWorld.o \
	${OBJECTDIR}/_ext/8f7034e2/btSoftRigidCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/8f7034e2/btSoftRigidDynamicsWorld.o \
	${OBJECTDIR}/_ext/8f7034e2/btSoftSoftCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/981e8aa1/btAlignedAllocator.o \
	${OBJECTDIR}/_ext/981e8aa1/btConvexHull.o \
	${OBJECTDIR}/_ext/981e8aa1/btConvexHullComputer.o \
	${OBJECTDIR}/_ext/981e8aa1/btGeometryUtil.o \
	${OBJECTDIR}/_ext/981e8aa1/btPolarDecomposition.o \
	${OBJECTDIR}/_ext/981e8aa1/btQuickprof.o \
	${OBJECTDIR}/_ext/981e8aa1/btSerializer.o \
	${OBJECTDIR}/_ext/981e8aa1/btThreads.o \
	${OBJECTDIR}/_ext/981e8aa1/btVector3.o \
	${OBJECTDIR}/_ext/44fa8293/imgui.o \
	${OBJECTDIR}/_ext/44fa8293/imgui_draw.o \
	${OBJECTDIR}/_ext/1d4ce04c/DetourAlloc.o \
	${OBJECTDIR}/_ext/1d4ce04c/DetourAssert.o \
	${OBJECTDIR}/_ext/1d4ce04c/DetourCommon.o \
	${OBJECTDIR}/_ext/1d4ce04c/DetourNavMesh.o \
	${OBJECTDIR}/_ext/1d4ce04c/DetourNavMeshBuilder.o \
	${OBJECTDIR}/_ext/1d4ce04c/DetourNavMeshQuery.o \
	${OBJECTDIR}/_ext/1d4ce04c/DetourNode.o \
	${OBJECTDIR}/_ext/56123e1b/DetourCrowd.o \
	${OBJECTDIR}/_ext/56123e1b/DetourLocalBoundary.o \
	${OBJECTDIR}/_ext/56123e1b/DetourObstacleAvoidance.o \
	${OBJECTDIR}/_ext/56123e1b/DetourPathCorridor.o \
	${OBJECTDIR}/_ext/56123e1b/DetourPathQueue.o \
	${OBJECTDIR}/_ext/56123e1b/DetourProximityGrid.o \
	${OBJECTDIR}/_ext/ac078074/DetourTileCache.o \
	${OBJECTDIR}/_ext/ac078074/DetourTileCacheBuilder.o \
	${OBJECTDIR}/_ext/907ca893/Recast.o \
	${OBJECTDIR}/_ext/907ca893/RecastAlloc.o \
	${OBJECTDIR}/_ext/907ca893/RecastArea.o \
	${OBJECTDIR}/_ext/907ca893/RecastAssert.o \
	${OBJECTDIR}/_ext/907ca893/RecastContour.o \
	${OBJECTDIR}/_ext/907ca893/RecastFilter.o \
	${OBJECTDIR}/_ext/907ca893/RecastLayers.o \
	${OBJECTDIR}/_ext/907ca893/RecastMesh.o \
	${OBJECTDIR}/_ext/907ca893/RecastMeshDetail.o \
	${OBJECTDIR}/_ext/907ca893/RecastRasterization.o \
	${OBJECTDIR}/_ext/907ca893/RecastRegion.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-engine.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-engine.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-engine.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-engine.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-engine.a

${OBJECTDIR}/_ext/8ec51d02/ASTCReader.o: ../../src/finjin/engine/ASTCReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ASTCReader.o ../../src/finjin/engine/ASTCReader.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationDelegate.o: ../../src/finjin/engine/ApplicationDelegate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationDelegate.o ../../src/finjin/engine/ApplicationDelegate.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationSettings.o: ../../src/finjin/engine/ApplicationSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationSettings.o ../../src/finjin/engine/ApplicationSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewport.o: ../../src/finjin/engine/ApplicationViewport.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewport.o ../../src/finjin/engine/ApplicationViewport.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDelegate.o: ../../src/finjin/engine/ApplicationViewportDelegate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDelegate.o ../../src/finjin/engine/ApplicationViewportDelegate.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDescription.o: ../../src/finjin/engine/ApplicationViewportDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDescription.o ../../src/finjin/engine/ApplicationViewportDescription.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportsController.o: ../../src/finjin/engine/ApplicationViewportsController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportsController.o ../../src/finjin/engine/ApplicationViewportsController.cpp

${OBJECTDIR}/_ext/8ec51d02/Application_General.o: ../../src/finjin/engine/Application_General.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Application_General.o ../../src/finjin/engine/Application_General.cpp

${OBJECTDIR}/_ext/8ec51d02/Asset.o: ../../src/finjin/engine/Asset.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Asset.o ../../src/finjin/engine/Asset.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetClass.o: ../../src/finjin/engine/AssetClass.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetClass.o ../../src/finjin/engine/AssetClass.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetClassFileReader.o: ../../src/finjin/engine/AssetClassFileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetClassFileReader.o ../../src/finjin/engine/AssetClassFileReader.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetCountsSettings.o: ../../src/finjin/engine/AssetCountsSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetCountsSettings.o ../../src/finjin/engine/AssetCountsSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetFileReader.o: ../../src/finjin/engine/AssetFileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetFileReader.o ../../src/finjin/engine/AssetFileReader.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetPath.o: ../../src/finjin/engine/AssetPath.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetPath.o ../../src/finjin/engine/AssetPath.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReadHandle.o: ../../src/finjin/engine/AssetReadHandle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReadHandle.o ../../src/finjin/engine/AssetReadHandle.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReadQueue.o: ../../src/finjin/engine/AssetReadQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReadQueue.o ../../src/finjin/engine/AssetReadQueue.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReference.o: ../../src/finjin/engine/AssetReference.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReference.o ../../src/finjin/engine/AssetReference.cpp

${OBJECTDIR}/_ext/8ec51d02/Camera.o: ../../src/finjin/engine/Camera.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Camera.o ../../src/finjin/engine/Camera.cpp

${OBJECTDIR}/_ext/8ec51d02/CameraState.o: ../../src/finjin/engine/CameraState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/CameraState.o ../../src/finjin/engine/CameraState.cpp

${OBJECTDIR}/_ext/8ec51d02/DDSReader.o: ../../src/finjin/engine/DDSReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/DDSReader.o ../../src/finjin/engine/DDSReader.cpp

${OBJECTDIR}/_ext/8ec51d02/DXGIIncludes.o: ../../src/finjin/engine/DXGIIncludes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/DXGIIncludes.o ../../src/finjin/engine/DXGIIncludes.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinPrecompiled.o: ../../src/finjin/engine/FinjinPrecompiled.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinPrecompiled.o ../../src/finjin/engine/FinjinPrecompiled.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinSceneAssets.o: ../../src/finjin/engine/FinjinSceneAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinSceneAssets.o ../../src/finjin/engine/FinjinSceneAssets.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinSceneReader.o: ../../src/finjin/engine/FinjinSceneReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinSceneReader.o ../../src/finjin/engine/FinjinSceneReader.cpp

${OBJECTDIR}/_ext/8ec51d02/FlyingCameraInputBindings.o: ../../src/finjin/engine/FlyingCameraInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FlyingCameraInputBindings.o ../../src/finjin/engine/FlyingCameraInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/GenericGpuNumericStructs.o: ../../src/finjin/engine/GenericGpuNumericStructs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/GenericGpuNumericStructs.o ../../src/finjin/engine/GenericGpuNumericStructs.cpp

${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonImpl.o: ../../src/finjin/engine/GpuContextCommonImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonImpl.o ../../src/finjin/engine/GpuContextCommonImpl.cpp

${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonSettings.o: ../../src/finjin/engine/GpuContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonSettings.o ../../src/finjin/engine/GpuContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/GpuRenderTargetSize.o: ../../src/finjin/engine/GpuRenderTargetSize.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/GpuRenderTargetSize.o ../../src/finjin/engine/GpuRenderTargetSize.cpp

${OBJECTDIR}/_ext/8ec51d02/InputComponents.o: ../../src/finjin/engine/InputComponents.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputComponents.o ../../src/finjin/engine/InputComponents.cpp

${OBJECTDIR}/_ext/8ec51d02/InputConstants.o: ../../src/finjin/engine/InputConstants.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputConstants.o ../../src/finjin/engine/InputConstants.cpp

${OBJECTDIR}/_ext/8ec51d02/InputContextCommonSettings.o: ../../src/finjin/engine/InputContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputContextCommonSettings.o ../../src/finjin/engine/InputContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/InputCoordinate.o: ../../src/finjin/engine/InputCoordinate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputCoordinate.o ../../src/finjin/engine/InputCoordinate.cpp

${OBJECTDIR}/_ext/8ec51d02/InputSource.o: ../../src/finjin/engine/InputSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputSource.o ../../src/finjin/engine/InputSource.cpp

${OBJECTDIR}/_ext/8ec51d02/JobPipelineStage.o: ../../src/finjin/engine/JobPipelineStage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/JobPipelineStage.o ../../src/finjin/engine/JobPipelineStage.cpp

${OBJECTDIR}/_ext/8ec51d02/KTXReader.o: ../../src/finjin/engine/KTXReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/KTXReader.o ../../src/finjin/engine/KTXReader.cpp

${OBJECTDIR}/_ext/8ec51d02/LightState.o: ../../src/finjin/engine/LightState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/LightState.o ../../src/finjin/engine/LightState.cpp

${OBJECTDIR}/_ext/8ec51d02/MemorySettings.o: ../../src/finjin/engine/MemorySettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/MemorySettings.o ../../src/finjin/engine/MemorySettings.cpp

${OBJECTDIR}/_ext/8ec51d02/PVRReader.o: ../../src/finjin/engine/PVRReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/PVRReader.o ../../src/finjin/engine/PVRReader.cpp

${OBJECTDIR}/_ext/8ec51d02/PlatformCapabilities_General.o: ../../src/finjin/engine/PlatformCapabilities_General.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/PlatformCapabilities_General.o ../../src/finjin/engine/PlatformCapabilities_General.cpp

${OBJECTDIR}/_ext/8ec51d02/ShaderFeatureFlag.o: ../../src/finjin/engine/ShaderFeatureFlag.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ShaderFeatureFlag.o ../../src/finjin/engine/ShaderFeatureFlag.cpp

${OBJECTDIR}/_ext/8ec51d02/ShaderFeatures.o: ../../src/finjin/engine/ShaderFeatures.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ShaderFeatures.o ../../src/finjin/engine/ShaderFeatures.cpp

${OBJECTDIR}/_ext/8ec51d02/SoundContextCommonSettings.o: ../../src/finjin/engine/SoundContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/SoundContextCommonSettings.o ../../src/finjin/engine/SoundContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/SoundFormat.o: ../../src/finjin/engine/SoundFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/SoundFormat.o ../../src/finjin/engine/SoundFormat.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentChunkNames.o: ../../src/finjin/engine/StandardAssetDocumentChunkNames.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentChunkNames.o ../../src/finjin/engine/StandardAssetDocumentChunkNames.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyNames.o: ../../src/finjin/engine/StandardAssetDocumentPropertyNames.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyNames.o ../../src/finjin/engine/StandardAssetDocumentPropertyNames.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyValues.o: ../../src/finjin/engine/StandardAssetDocumentPropertyValues.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyValues.o ../../src/finjin/engine/StandardAssetDocumentPropertyValues.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardGameControllerInputBindings.o: ../../src/finjin/engine/StandardGameControllerInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardGameControllerInputBindings.o ../../src/finjin/engine/StandardGameControllerInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/StringTable.o: ../../src/finjin/engine/StringTable.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StringTable.o ../../src/finjin/engine/StringTable.cpp

${OBJECTDIR}/_ext/8ec51d02/TextureDimension.o: ../../src/finjin/engine/TextureDimension.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/TextureDimension.o ../../src/finjin/engine/TextureDimension.cpp

${OBJECTDIR}/_ext/8ec51d02/UserDataTypes.o: ../../src/finjin/engine/UserDataTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/UserDataTypes.o ../../src/finjin/engine/UserDataTypes.cpp

${OBJECTDIR}/_ext/8ec51d02/VRContextCommonSettings.o: ../../src/finjin/engine/VRContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/VRContextCommonSettings.o ../../src/finjin/engine/VRContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/VRGameControllerInputBindings.o: ../../src/finjin/engine/VRGameControllerInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/VRGameControllerInputBindings.o ../../src/finjin/engine/VRGameControllerInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/WAVReader.o: ../../src/finjin/engine/WAVReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WAVReader.o ../../src/finjin/engine/WAVReader.cpp

${OBJECTDIR}/_ext/8ec51d02/WindowBounds.o: ../../src/finjin/engine/WindowBounds.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WindowBounds.o ../../src/finjin/engine/WindowBounds.cpp

${OBJECTDIR}/_ext/8ec51d02/WindowSize.o: ../../src/finjin/engine/WindowSize.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WindowSize.o ../../src/finjin/engine/WindowSize.cpp

${OBJECTDIR}/_ext/40bb3adf/Application.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/Application.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/Application.o ../../src/finjin/engine/internal/app/linux/finjin/engine/Application.cpp

${OBJECTDIR}/_ext/40bb3adf/ApplicationGlobals.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/ApplicationGlobals.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/ApplicationGlobals.o ../../src/finjin/engine/internal/app/linux/finjin/engine/ApplicationGlobals.cpp

${OBJECTDIR}/_ext/40bb3adf/DisplayInfo.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/DisplayInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/DisplayInfo.o ../../src/finjin/engine/internal/app/linux/finjin/engine/DisplayInfo.cpp

${OBJECTDIR}/_ext/40bb3adf/FinjinMain.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/FinjinMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/FinjinMain.o ../../src/finjin/engine/internal/app/linux/finjin/engine/FinjinMain.cpp

${OBJECTDIR}/_ext/40bb3adf/OSWindow.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/OSWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/OSWindow.o ../../src/finjin/engine/internal/app/linux/finjin/engine/OSWindow.cpp

${OBJECTDIR}/_ext/40bb3adf/PlatformCapabilities.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/PlatformCapabilities.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/PlatformCapabilities.o ../../src/finjin/engine/internal/app/linux/finjin/engine/PlatformCapabilities.cpp

${OBJECTDIR}/_ext/40bb3adf/XcbConnection.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/XcbConnection.o ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbConnection.cpp

${OBJECTDIR}/_ext/40bb3adf/XcbWindow.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/XcbWindow.o ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbWindow.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanDescriptorSetLayout.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanDescriptorSetLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanDescriptorSetLayout.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanDescriptorSetLayout.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanFrameBuffer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanFrameBuffer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameBuffer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanFrameStage.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameStage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanFrameStage.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameStage.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuBuffer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuBuffer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuBuffer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContext.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContext.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContext.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextImpl.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextImpl.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextImpl.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextSettings.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextSettings.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextSettings.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuDescription.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuDescription.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuDescription.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanIncludes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanIncludes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanIncludes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanIncludes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanInputFormat.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanInputFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanInputFormat.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanInputFormat.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanLayerProperties.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanLayerProperties.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanLayerProperties.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanLayerProperties.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanLight.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanLight.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanLight.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanLight.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMaterial.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMaterial.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMaterial.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMaterial.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMemoryResource.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMemoryResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMemoryResource.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMemoryResource.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMesh.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMesh.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMesh.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPipeline.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipeline.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPipeline.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipeline.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPipelineBuilder.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipelineBuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPipelineBuilder.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipelineBuilder.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPipelineLayout.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipelineLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPipelineLayout.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipelineLayout.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPresentModes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPresentModes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPresentModes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPresentModes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPrimaryQueues.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPrimaryQueues.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPrimaryQueues.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPrimaryQueues.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanQueueFamilyIndexes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanQueueFamilyIndexes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanQueueFamilyIndexes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanQueueFamilyIndexes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanRenderTarget.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanRenderTarget.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanRenderTarget.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanRenderTarget.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanResources.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanResources.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanResources.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanResources.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanShader.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanShader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanShader.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanShader.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSimpleWork.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSimpleWork.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSimpleWork.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSimpleWork.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanStaticMeshRenderer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanStaticMeshRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanStaticMeshRenderer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanStaticMeshRenderer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSurfaceFormats.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSurfaceFormats.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSurfaceFormats.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSurfaceFormats.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSwapChainFrameBuffer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSwapChainFrameBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSwapChainFrameBuffer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSwapChainFrameBuffer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystem.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystem.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystem.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystemImpl.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystemImpl.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemImpl.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystemSettings.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystemSettings.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemSettings.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanTexture.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanTexture.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanTexture.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanTexture.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanUtilities.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanUtilities.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanUtilities.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanUtilities.cpp

${OBJECTDIR}/_ext/32e60500/LinuxGameController.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxGameController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxGameController.o ../../src/finjin/engine/internal/input/linuxinput/LinuxGameController.cpp

${OBJECTDIR}/_ext/32e60500/LinuxInputContext.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxInputContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxInputContext.o ../../src/finjin/engine/internal/input/linuxinput/LinuxInputContext.cpp

${OBJECTDIR}/_ext/32e60500/LinuxInputSystem.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxInputSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxInputSystem.o ../../src/finjin/engine/internal/input/linuxinput/LinuxInputSystem.cpp

${OBJECTDIR}/_ext/32e60500/LinuxKeyboard.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxKeyboard.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxKeyboard.o ../../src/finjin/engine/internal/input/linuxinput/LinuxKeyboard.cpp

${OBJECTDIR}/_ext/32e60500/LinuxMouse.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxMouse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxMouse.o ../../src/finjin/engine/internal/input/linuxinput/LinuxMouse.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALAdapterDescription.o: ../../src/finjin/engine/internal/sound/openal/OpenALAdapterDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALAdapterDescription.o ../../src/finjin/engine/internal/sound/openal/OpenALAdapterDescription.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALContext.o: ../../src/finjin/engine/internal/sound/openal/OpenALContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALContext.o ../../src/finjin/engine/internal/sound/openal/OpenALContext.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALContextImpl.o: ../../src/finjin/engine/internal/sound/openal/OpenALContextImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALContextImpl.o ../../src/finjin/engine/internal/sound/openal/OpenALContextImpl.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALEffect.o: ../../src/finjin/engine/internal/sound/openal/OpenALEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALEffect.o ../../src/finjin/engine/internal/sound/openal/OpenALEffect.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALEffectSlot.o: ../../src/finjin/engine/internal/sound/openal/OpenALEffectSlot.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALEffectSlot.o ../../src/finjin/engine/internal/sound/openal/OpenALEffectSlot.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALExtensions.o: ../../src/finjin/engine/internal/sound/openal/OpenALExtensions.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALExtensions.o ../../src/finjin/engine/internal/sound/openal/OpenALExtensions.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALFilter.o: ../../src/finjin/engine/internal/sound/openal/OpenALFilter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALFilter.o ../../src/finjin/engine/internal/sound/openal/OpenALFilter.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALListener.o: ../../src/finjin/engine/internal/sound/openal/OpenALListener.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALListener.o ../../src/finjin/engine/internal/sound/openal/OpenALListener.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSoundBuffer.o: ../../src/finjin/engine/internal/sound/openal/OpenALSoundBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSoundBuffer.o ../../src/finjin/engine/internal/sound/openal/OpenALSoundBuffer.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSoundSource.o: ../../src/finjin/engine/internal/sound/openal/OpenALSoundSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSoundSource.o ../../src/finjin/engine/internal/sound/openal/OpenALSoundSource.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSystem.o: ../../src/finjin/engine/internal/sound/openal/OpenALSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSystem.o ../../src/finjin/engine/internal/sound/openal/OpenALSystem.cpp

${OBJECTDIR}/_ext/b0d4447f/b3DynamicBvh.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b0d4447f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b0d4447f/b3DynamicBvh.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp

${OBJECTDIR}/_ext/b0d4447f/b3DynamicBvhBroadphase.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b0d4447f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b0d4447f/b3DynamicBvhBroadphase.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp

${OBJECTDIR}/_ext/b0d4447f/b3OverlappingPairCache.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b0d4447f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b0d4447f/b3OverlappingPairCache.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp

${OBJECTDIR}/_ext/ccaef08c/b3ConvexUtility.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ccaef08c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ccaef08c/b3ConvexUtility.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp

${OBJECTDIR}/_ext/ccaef08c/b3CpuNarrowPhase.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ccaef08c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ccaef08c/b3CpuNarrowPhase.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp

${OBJECTDIR}/_ext/6792bd68/b3AlignedAllocator.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Common/b3AlignedAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6792bd68
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6792bd68/b3AlignedAllocator.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Common/b3AlignedAllocator.cpp

${OBJECTDIR}/_ext/6792bd68/b3Logging.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Common/b3Logging.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6792bd68
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6792bd68/b3Logging.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Common/b3Logging.cpp

${OBJECTDIR}/_ext/6792bd68/b3Vector3.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Common/b3Vector3.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6792bd68
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6792bd68/b3Vector3.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Common/b3Vector3.cpp

${OBJECTDIR}/_ext/f9590a6e/b3FixedConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3FixedConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9590a6e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9590a6e/b3FixedConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3FixedConstraint.cpp

${OBJECTDIR}/_ext/f9590a6e/b3Generic6DofConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9590a6e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9590a6e/b3Generic6DofConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.cpp

${OBJECTDIR}/_ext/f9590a6e/b3PgsJacobiSolver.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3PgsJacobiSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9590a6e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9590a6e/b3PgsJacobiSolver.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3PgsJacobiSolver.cpp

${OBJECTDIR}/_ext/f9590a6e/b3Point2PointConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3Point2PointConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9590a6e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9590a6e/b3Point2PointConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3Point2PointConstraint.cpp

${OBJECTDIR}/_ext/f9590a6e/b3TypedConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3TypedConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f9590a6e
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f9590a6e/b3TypedConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/ConstraintSolver/b3TypedConstraint.cpp

${OBJECTDIR}/_ext/47c35051/b3CpuRigidBodyPipeline.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/b3CpuRigidBodyPipeline.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/47c35051
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/47c35051/b3CpuRigidBodyPipeline.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Dynamics/b3CpuRigidBodyPipeline.cpp

${OBJECTDIR}/_ext/5fc4b0af/b3ConvexHullComputer.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Geometry/b3ConvexHullComputer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5fc4b0af
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5fc4b0af/b3ConvexHullComputer.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Geometry/b3ConvexHullComputer.cpp

${OBJECTDIR}/_ext/5fc4b0af/b3GeometryUtil.o: /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Geometry/b3GeometryUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5fc4b0af
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5fc4b0af/b3GeometryUtil.o /home/build/finjin-3rd-party/cpp/bullet/src/Bullet3Geometry/b3GeometryUtil.cpp

${OBJECTDIR}/_ext/32f8a646/btAxisSweep3.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btAxisSweep3.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp

${OBJECTDIR}/_ext/32f8a646/btBroadphaseProxy.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btBroadphaseProxy.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp

${OBJECTDIR}/_ext/32f8a646/btCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/32f8a646/btDbvt.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btDbvt.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btDbvt.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btDbvt.cpp

${OBJECTDIR}/_ext/32f8a646/btDbvtBroadphase.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btDbvtBroadphase.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp

${OBJECTDIR}/_ext/32f8a646/btDispatcher.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btDispatcher.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp

${OBJECTDIR}/_ext/32f8a646/btOverlappingPairCache.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btOverlappingPairCache.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp

${OBJECTDIR}/_ext/32f8a646/btQuantizedBvh.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btQuantizedBvh.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp

${OBJECTDIR}/_ext/32f8a646/btSimpleBroadphase.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32f8a646
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32f8a646/btSimpleBroadphase.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp

${OBJECTDIR}/_ext/188ee859/SphereTriangleDetector.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/SphereTriangleDetector.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp

${OBJECTDIR}/_ext/188ee859/btActivatingCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btActivatingCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btBox2dBox2dCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btBox2dBox2dCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btBoxBoxCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btBoxBoxCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btBoxBoxDetector.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btBoxBoxDetector.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp

${OBJECTDIR}/_ext/188ee859/btCollisionDispatcher.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btCollisionDispatcher.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp

${OBJECTDIR}/_ext/188ee859/btCollisionObject.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btCollisionObject.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp

${OBJECTDIR}/_ext/188ee859/btCollisionWorld.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btCollisionWorld.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp

${OBJECTDIR}/_ext/188ee859/btCollisionWorldImporter.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCollisionWorldImporter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btCollisionWorldImporter.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCollisionWorldImporter.cpp

${OBJECTDIR}/_ext/188ee859/btCompoundCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btCompoundCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btCompoundCompoundCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btCompoundCompoundCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btConvex2dConvex2dAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btConvex2dConvex2dAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btConvexConcaveCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btConvexConcaveCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btConvexConvexAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btConvexConvexAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btConvexPlaneCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btConvexPlaneCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btDefaultCollisionConfiguration.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btDefaultCollisionConfiguration.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp

${OBJECTDIR}/_ext/188ee859/btEmptyCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btEmptyCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btGhostObject.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btGhostObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btGhostObject.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btGhostObject.cpp

${OBJECTDIR}/_ext/188ee859/btHashedSimplePairCache.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btHashedSimplePairCache.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp

${OBJECTDIR}/_ext/188ee859/btInternalEdgeUtility.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btInternalEdgeUtility.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp

${OBJECTDIR}/_ext/188ee859/btManifoldResult.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btManifoldResult.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp

${OBJECTDIR}/_ext/188ee859/btSimulationIslandManager.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btSimulationIslandManager.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp

${OBJECTDIR}/_ext/188ee859/btSphereBoxCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btSphereBoxCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btSphereSphereCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btSphereSphereCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btSphereTriangleCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btSphereTriangleCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/188ee859/btUnionFind.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btUnionFind.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/188ee859
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/188ee859/btUnionFind.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionDispatch/btUnionFind.cpp

${OBJECTDIR}/_ext/ec3fb491/btBox2dShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btBox2dShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btBox2dShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btBox2dShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btBoxShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btBoxShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btBoxShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btBoxShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btBvhTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btBvhTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btCapsuleShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btCapsuleShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btCollisionShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btCollisionShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btCollisionShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btCollisionShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btCompoundShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btCompoundShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btCompoundShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btCompoundShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btConcaveShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConcaveShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConcaveShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConcaveShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btConeShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConeShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConeShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConeShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btConvex2dShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConvex2dShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btConvexHullShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConvexHullShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btConvexInternalShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConvexInternalShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btConvexPointCloudShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConvexPointCloudShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btConvexPolyhedron.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConvexPolyhedron.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp

${OBJECTDIR}/_ext/ec3fb491/btConvexShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConvexShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btConvexTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btConvexTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btCylinderShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btCylinderShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btCylinderShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btCylinderShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btEmptyShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btEmptyShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btEmptyShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btEmptyShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btHeightfieldTerrainShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btHeightfieldTerrainShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btMinkowskiSumShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btMinkowskiSumShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btMultiSphereShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btMultiSphereShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btMultimaterialTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btMultimaterialTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btOptimizedBvh.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btOptimizedBvh.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp

${OBJECTDIR}/_ext/ec3fb491/btPolyhedralConvexShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btPolyhedralConvexShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btScaledBvhTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btScaledBvhTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btShapeHull.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btShapeHull.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btShapeHull.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btShapeHull.cpp

${OBJECTDIR}/_ext/ec3fb491/btSphereShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btSphereShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btSphereShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btSphereShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btStaticPlaneShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btStaticPlaneShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btStridingMeshInterface.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btStridingMeshInterface.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp

${OBJECTDIR}/_ext/ec3fb491/btTetrahedronShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btTetrahedronShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btTriangleBuffer.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btTriangleBuffer.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp

${OBJECTDIR}/_ext/ec3fb491/btTriangleCallback.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btTriangleCallback.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp

${OBJECTDIR}/_ext/ec3fb491/btTriangleIndexVertexArray.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btTriangleIndexVertexArray.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp

${OBJECTDIR}/_ext/ec3fb491/btTriangleIndexVertexMaterialArray.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btTriangleIndexVertexMaterialArray.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp

${OBJECTDIR}/_ext/ec3fb491/btTriangleMesh.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btTriangleMesh.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp

${OBJECTDIR}/_ext/ec3fb491/btTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp

${OBJECTDIR}/_ext/ec3fb491/btUniformScalingShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ec3fb491
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ec3fb491/btUniformScalingShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp

${OBJECTDIR}/_ext/6348bb3a/btContactProcessing.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btContactProcessing.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/btContactProcessing.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btContactProcessing.cpp

${OBJECTDIR}/_ext/6348bb3a/btGImpactBvh.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGImpactBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/btGImpactBvh.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGImpactBvh.cpp

${OBJECTDIR}/_ext/6348bb3a/btGImpactCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/btGImpactCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/6348bb3a/btGImpactQuantizedBvh.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/btGImpactQuantizedBvh.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp

${OBJECTDIR}/_ext/6348bb3a/btGImpactShape.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGImpactShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/btGImpactShape.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGImpactShape.cpp

${OBJECTDIR}/_ext/6348bb3a/btGenericPoolAllocator.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/btGenericPoolAllocator.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp

${OBJECTDIR}/_ext/6348bb3a/btTriangleShapeEx.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/btTriangleShapeEx.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp

${OBJECTDIR}/_ext/6348bb3a/gim_box_set.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/gim_box_set.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/gim_box_set.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/gim_box_set.cpp

${OBJECTDIR}/_ext/6348bb3a/gim_contact.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/gim_contact.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/gim_contact.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/gim_contact.cpp

${OBJECTDIR}/_ext/6348bb3a/gim_memory.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/gim_memory.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/gim_memory.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/gim_memory.cpp

${OBJECTDIR}/_ext/6348bb3a/gim_tri_collision.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/gim_tri_collision.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6348bb3a
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6348bb3a/gim_tri_collision.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/Gimpact/gim_tri_collision.cpp

${OBJECTDIR}/_ext/53a97f85/btContinuousConvexCollision.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btContinuousConvexCollision.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp

${OBJECTDIR}/_ext/53a97f85/btConvexCast.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btConvexCast.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp

${OBJECTDIR}/_ext/53a97f85/btGjkConvexCast.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btGjkConvexCast.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp

${OBJECTDIR}/_ext/53a97f85/btGjkEpa2.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btGjkEpa2.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp

${OBJECTDIR}/_ext/53a97f85/btGjkEpaPenetrationDepthSolver.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btGjkEpaPenetrationDepthSolver.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp

${OBJECTDIR}/_ext/53a97f85/btGjkPairDetector.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btGjkPairDetector.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp

${OBJECTDIR}/_ext/53a97f85/btMinkowskiPenetrationDepthSolver.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btMinkowskiPenetrationDepthSolver.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp

${OBJECTDIR}/_ext/53a97f85/btPersistentManifold.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btPersistentManifold.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp

${OBJECTDIR}/_ext/53a97f85/btPolyhedralContactClipping.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btPolyhedralContactClipping.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp

${OBJECTDIR}/_ext/53a97f85/btRaycastCallback.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btRaycastCallback.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp

${OBJECTDIR}/_ext/53a97f85/btSubSimplexConvexCast.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btSubSimplexConvexCast.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp

${OBJECTDIR}/_ext/53a97f85/btVoronoiSimplexSolver.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53a97f85
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53a97f85/btVoronoiSimplexSolver.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp

${OBJECTDIR}/_ext/6f4bfb44/btKinematicCharacterController.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Character/btKinematicCharacterController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f4bfb44
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f4bfb44/btKinematicCharacterController.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Character/btKinematicCharacterController.cpp

${OBJECTDIR}/_ext/78dd43d5/btConeTwistConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btConeTwistConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btContactConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btContactConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btFixedConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btFixedConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btGearConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btGearConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btGearConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btGearConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofSpring2Constraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofSpring2Constraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofSpringConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btGeneric6DofSpringConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btHinge2Constraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btHinge2Constraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btHingeConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btHingeConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btNNCGConstraintSolver.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btNNCGConstraintSolver.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.cpp

${OBJECTDIR}/_ext/78dd43d5/btPoint2PointConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btPoint2PointConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btSequentialImpulseConstraintSolver.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btSequentialImpulseConstraintSolver.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp

${OBJECTDIR}/_ext/78dd43d5/btSliderConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btSliderConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btSolve2LinearConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btSolve2LinearConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btTypedConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btTypedConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp

${OBJECTDIR}/_ext/78dd43d5/btUniversalConstraint.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/78dd43d5
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/78dd43d5/btUniversalConstraint.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp

${OBJECTDIR}/_ext/a9307899/btDiscreteDynamicsWorld.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/a9307899
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9307899/btDiscreteDynamicsWorld.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp

${OBJECTDIR}/_ext/a9307899/btDiscreteDynamicsWorldMt.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/a9307899
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9307899/btDiscreteDynamicsWorldMt.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.cpp

${OBJECTDIR}/_ext/a9307899/btRigidBody.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btRigidBody.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/a9307899
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9307899/btRigidBody.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btRigidBody.cpp

${OBJECTDIR}/_ext/a9307899/btSimpleDynamicsWorld.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/a9307899
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9307899/btSimpleDynamicsWorld.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp

${OBJECTDIR}/_ext/a9307899/btSimulationIslandManagerMt.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btSimulationIslandManagerMt.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/a9307899
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/a9307899/btSimulationIslandManagerMt.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Dynamics/btSimulationIslandManagerMt.cpp

${OBJECTDIR}/_ext/e8ada44f/btDantzigLCP.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e8ada44f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e8ada44f/btDantzigLCP.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp

${OBJECTDIR}/_ext/e8ada44f/btLemkeAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/MLCPSolvers/btLemkeAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e8ada44f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e8ada44f/btLemkeAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/MLCPSolvers/btLemkeAlgorithm.cpp

${OBJECTDIR}/_ext/e8ada44f/btMLCPSolver.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e8ada44f
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e8ada44f/btMLCPSolver.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp

${OBJECTDIR}/_ext/7a2b1a07/btRaycastVehicle.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2b1a07
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2b1a07/btRaycastVehicle.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp

${OBJECTDIR}/_ext/7a2b1a07/btWheelInfo.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Vehicle/btWheelInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/7a2b1a07
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/7a2b1a07/btWheelInfo.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletDynamics/Vehicle/btWheelInfo.cpp

${OBJECTDIR}/_ext/458d3eae/IDMath.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletInverseDynamics/IDMath.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/458d3eae
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/458d3eae/IDMath.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletInverseDynamics/IDMath.cpp

${OBJECTDIR}/_ext/458d3eae/MultiBodyTree.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletInverseDynamics/MultiBodyTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/458d3eae
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/458d3eae/MultiBodyTree.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletInverseDynamics/MultiBodyTree.cpp

${OBJECTDIR}/_ext/3122dfe1/MultiBodyTreeImpl.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletInverseDynamics/details/MultiBodyTreeImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/3122dfe1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3122dfe1/MultiBodyTreeImpl.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletInverseDynamics/details/MultiBodyTreeImpl.cpp

${OBJECTDIR}/_ext/3122dfe1/MultiBodyTreeInitCache.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletInverseDynamics/details/MultiBodyTreeInitCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/3122dfe1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3122dfe1/MultiBodyTreeInitCache.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletInverseDynamics/details/MultiBodyTreeInitCache.cpp

${OBJECTDIR}/_ext/8f7034e2/btDefaultSoftBodySolver.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btDefaultSoftBodySolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btDefaultSoftBodySolver.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btDefaultSoftBodySolver.cpp

${OBJECTDIR}/_ext/8f7034e2/btSoftBody.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftBody.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btSoftBody.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftBody.cpp

${OBJECTDIR}/_ext/8f7034e2/btSoftBodyConcaveCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btSoftBodyConcaveCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/8f7034e2/btSoftBodyHelpers.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftBodyHelpers.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btSoftBodyHelpers.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftBodyHelpers.cpp

${OBJECTDIR}/_ext/8f7034e2/btSoftBodyRigidBodyCollisionConfiguration.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btSoftBodyRigidBodyCollisionConfiguration.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp

${OBJECTDIR}/_ext/8f7034e2/btSoftMultiBodyDynamicsWorld.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftMultiBodyDynamicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btSoftMultiBodyDynamicsWorld.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftMultiBodyDynamicsWorld.cpp

${OBJECTDIR}/_ext/8f7034e2/btSoftRigidCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btSoftRigidCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/8f7034e2/btSoftRigidDynamicsWorld.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btSoftRigidDynamicsWorld.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp

${OBJECTDIR}/_ext/8f7034e2/btSoftSoftCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8f7034e2
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8f7034e2/btSoftSoftCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/981e8aa1/btAlignedAllocator.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btAlignedAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btAlignedAllocator.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btAlignedAllocator.cpp

${OBJECTDIR}/_ext/981e8aa1/btConvexHull.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btConvexHull.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btConvexHull.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btConvexHull.cpp

${OBJECTDIR}/_ext/981e8aa1/btConvexHullComputer.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btConvexHullComputer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btConvexHullComputer.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btConvexHullComputer.cpp

${OBJECTDIR}/_ext/981e8aa1/btGeometryUtil.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btGeometryUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btGeometryUtil.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btGeometryUtil.cpp

${OBJECTDIR}/_ext/981e8aa1/btPolarDecomposition.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btPolarDecomposition.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btPolarDecomposition.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btPolarDecomposition.cpp

${OBJECTDIR}/_ext/981e8aa1/btQuickprof.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btQuickprof.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btQuickprof.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btQuickprof.cpp

${OBJECTDIR}/_ext/981e8aa1/btSerializer.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btSerializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btSerializer.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btSerializer.cpp

${OBJECTDIR}/_ext/981e8aa1/btThreads.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btThreads.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btThreads.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btThreads.cpp

${OBJECTDIR}/_ext/981e8aa1/btVector3.o: /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btVector3.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/981e8aa1
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/981e8aa1/btVector3.o /home/build/finjin-3rd-party/cpp/bullet/src/LinearMath/btVector3.cpp

${OBJECTDIR}/_ext/44fa8293/imgui.o: /home/build/finjin-3rd-party/cpp/imgui/imgui.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/44fa8293
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/44fa8293/imgui.o /home/build/finjin-3rd-party/cpp/imgui/imgui.cpp

${OBJECTDIR}/_ext/44fa8293/imgui_draw.o: /home/build/finjin-3rd-party/cpp/imgui/imgui_draw.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/44fa8293
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/44fa8293/imgui_draw.o /home/build/finjin-3rd-party/cpp/imgui/imgui_draw.cpp

${OBJECTDIR}/_ext/1d4ce04c/DetourAlloc.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourAlloc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1d4ce04c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1d4ce04c/DetourAlloc.o /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourAlloc.cpp

${OBJECTDIR}/_ext/1d4ce04c/DetourAssert.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourAssert.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1d4ce04c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1d4ce04c/DetourAssert.o /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourAssert.cpp

${OBJECTDIR}/_ext/1d4ce04c/DetourCommon.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourCommon.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1d4ce04c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1d4ce04c/DetourCommon.o /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourCommon.cpp

${OBJECTDIR}/_ext/1d4ce04c/DetourNavMesh.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourNavMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1d4ce04c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1d4ce04c/DetourNavMesh.o /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourNavMesh.cpp

${OBJECTDIR}/_ext/1d4ce04c/DetourNavMeshBuilder.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourNavMeshBuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1d4ce04c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1d4ce04c/DetourNavMeshBuilder.o /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourNavMeshBuilder.cpp

${OBJECTDIR}/_ext/1d4ce04c/DetourNavMeshQuery.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourNavMeshQuery.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1d4ce04c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1d4ce04c/DetourNavMeshQuery.o /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourNavMeshQuery.cpp

${OBJECTDIR}/_ext/1d4ce04c/DetourNode.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourNode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1d4ce04c
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1d4ce04c/DetourNode.o /home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Source/DetourNode.cpp

${OBJECTDIR}/_ext/56123e1b/DetourCrowd.o: /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourCrowd.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56123e1b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56123e1b/DetourCrowd.o /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourCrowd.cpp

${OBJECTDIR}/_ext/56123e1b/DetourLocalBoundary.o: /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourLocalBoundary.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56123e1b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56123e1b/DetourLocalBoundary.o /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourLocalBoundary.cpp

${OBJECTDIR}/_ext/56123e1b/DetourObstacleAvoidance.o: /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourObstacleAvoidance.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56123e1b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56123e1b/DetourObstacleAvoidance.o /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourObstacleAvoidance.cpp

${OBJECTDIR}/_ext/56123e1b/DetourPathCorridor.o: /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourPathCorridor.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56123e1b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56123e1b/DetourPathCorridor.o /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourPathCorridor.cpp

${OBJECTDIR}/_ext/56123e1b/DetourPathQueue.o: /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourPathQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56123e1b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56123e1b/DetourPathQueue.o /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourPathQueue.cpp

${OBJECTDIR}/_ext/56123e1b/DetourProximityGrid.o: /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourProximityGrid.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/56123e1b
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/56123e1b/DetourProximityGrid.o /home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Source/DetourProximityGrid.cpp

${OBJECTDIR}/_ext/ac078074/DetourTileCache.o: /home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Source/DetourTileCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ac078074
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ac078074/DetourTileCache.o /home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Source/DetourTileCache.cpp

${OBJECTDIR}/_ext/ac078074/DetourTileCacheBuilder.o: /home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Source/DetourTileCacheBuilder.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ac078074
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ac078074/DetourTileCacheBuilder.o /home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Source/DetourTileCacheBuilder.cpp

${OBJECTDIR}/_ext/907ca893/Recast.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/Recast.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/Recast.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/Recast.cpp

${OBJECTDIR}/_ext/907ca893/RecastAlloc.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastAlloc.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastAlloc.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastAlloc.cpp

${OBJECTDIR}/_ext/907ca893/RecastArea.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastArea.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastArea.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastArea.cpp

${OBJECTDIR}/_ext/907ca893/RecastAssert.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastAssert.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastAssert.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastAssert.cpp

${OBJECTDIR}/_ext/907ca893/RecastContour.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastContour.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastContour.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastContour.cpp

${OBJECTDIR}/_ext/907ca893/RecastFilter.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastFilter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastFilter.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastFilter.cpp

${OBJECTDIR}/_ext/907ca893/RecastLayers.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastLayers.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastLayers.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastLayers.cpp

${OBJECTDIR}/_ext/907ca893/RecastMesh.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastMesh.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastMesh.cpp

${OBJECTDIR}/_ext/907ca893/RecastMeshDetail.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastMeshDetail.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastMeshDetail.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastMeshDetail.cpp

${OBJECTDIR}/_ext/907ca893/RecastRasterization.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastRasterization.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastRasterization.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastRasterization.cpp

${OBJECTDIR}/_ext/907ca893/RecastRegion.o: /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastRegion.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/907ca893
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I/home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/Detour/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourCrowd/Include -I/home/build/finjin-3rd-party/cpp/recastnavigation/DetourTileCache/Include -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/907ca893/RecastRegion.o /home/build/finjin-3rd-party/cpp/recastnavigation/Recast/Source/RecastRegion.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libfinjin-engine.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
