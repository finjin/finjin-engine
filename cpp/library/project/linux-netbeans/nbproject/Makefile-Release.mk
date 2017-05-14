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
CND_CONF=Release
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
	${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonSettings.o \
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
	${OBJECTDIR}/_ext/f85409f3/VulkanPipelineLayout.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanPresentModes.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanPrimaryQueues.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanQueueFamilyIndexes.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanRenderTarget.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanResources.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanShader.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanStaticMeshRenderer.o \
	${OBJECTDIR}/_ext/f85409f3/VulkanSurfaceFormats.o \
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
	${OBJECTDIR}/_ext/94966a78/b3DynamicBvh.o \
	${OBJECTDIR}/_ext/94966a78/b3DynamicBvhBroadphase.o \
	${OBJECTDIR}/_ext/94966a78/b3OverlappingPairCache.o \
	${OBJECTDIR}/_ext/613189b3/b3ConvexUtility.o \
	${OBJECTDIR}/_ext/613189b3/b3CpuNarrowPhase.o \
	${OBJECTDIR}/_ext/15f39b8f/b3AlignedAllocator.o \
	${OBJECTDIR}/_ext/15f39b8f/b3Logging.o \
	${OBJECTDIR}/_ext/15f39b8f/b3Vector3.o \
	${OBJECTDIR}/_ext/908714e7/b3FixedConstraint.o \
	${OBJECTDIR}/_ext/908714e7/b3Generic6DofConstraint.o \
	${OBJECTDIR}/_ext/908714e7/b3PgsJacobiSolver.o \
	${OBJECTDIR}/_ext/908714e7/b3Point2PointConstraint.o \
	${OBJECTDIR}/_ext/908714e7/b3TypedConstraint.o \
	${OBJECTDIR}/_ext/e16540b8/b3CpuRigidBodyPipeline.o \
	${OBJECTDIR}/_ext/f966a116/b3ConvexHullComputer.o \
	${OBJECTDIR}/_ext/f966a116/b3GeometryUtil.o \
	${OBJECTDIR}/_ext/29cd5d2d/btAxisSweep3.o \
	${OBJECTDIR}/_ext/29cd5d2d/btBroadphaseProxy.o \
	${OBJECTDIR}/_ext/29cd5d2d/btCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/29cd5d2d/btDbvt.o \
	${OBJECTDIR}/_ext/29cd5d2d/btDbvtBroadphase.o \
	${OBJECTDIR}/_ext/29cd5d2d/btDispatcher.o \
	${OBJECTDIR}/_ext/29cd5d2d/btOverlappingPairCache.o \
	${OBJECTDIR}/_ext/29cd5d2d/btQuantizedBvh.o \
	${OBJECTDIR}/_ext/29cd5d2d/btSimpleBroadphase.o \
	${OBJECTDIR}/_ext/67222d00/SphereTriangleDetector.o \
	${OBJECTDIR}/_ext/67222d00/btActivatingCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btBox2dBox2dCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btBoxBoxCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btBoxBoxDetector.o \
	${OBJECTDIR}/_ext/67222d00/btCollisionDispatcher.o \
	${OBJECTDIR}/_ext/67222d00/btCollisionObject.o \
	${OBJECTDIR}/_ext/67222d00/btCollisionWorld.o \
	${OBJECTDIR}/_ext/67222d00/btCollisionWorldImporter.o \
	${OBJECTDIR}/_ext/67222d00/btCompoundCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btCompoundCompoundCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btConvex2dConvex2dAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btConvexConcaveCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btConvexConvexAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btConvexPlaneCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btDefaultCollisionConfiguration.o \
	${OBJECTDIR}/_ext/67222d00/btEmptyCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btGhostObject.o \
	${OBJECTDIR}/_ext/67222d00/btHashedSimplePairCache.o \
	${OBJECTDIR}/_ext/67222d00/btInternalEdgeUtility.o \
	${OBJECTDIR}/_ext/67222d00/btManifoldResult.o \
	${OBJECTDIR}/_ext/67222d00/btSimulationIslandManager.o \
	${OBJECTDIR}/_ext/67222d00/btSphereBoxCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btSphereSphereCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btSphereTriangleCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/67222d00/btUnionFind.o \
	${OBJECTDIR}/_ext/d859f6f8/btBox2dShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btBoxShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btBvhTriangleMeshShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btCapsuleShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btCollisionShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btCompoundShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btConcaveShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btConeShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btConvex2dShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btConvexHullShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btConvexInternalShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btConvexPointCloudShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btConvexPolyhedron.o \
	${OBJECTDIR}/_ext/d859f6f8/btConvexShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btConvexTriangleMeshShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btCylinderShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btEmptyShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btHeightfieldTerrainShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btMinkowskiSumShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btMultiSphereShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btMultimaterialTriangleMeshShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btOptimizedBvh.o \
	${OBJECTDIR}/_ext/d859f6f8/btPolyhedralConvexShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btScaledBvhTriangleMeshShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btShapeHull.o \
	${OBJECTDIR}/_ext/d859f6f8/btSphereShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btStaticPlaneShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btStridingMeshInterface.o \
	${OBJECTDIR}/_ext/d859f6f8/btTetrahedronShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btTriangleBuffer.o \
	${OBJECTDIR}/_ext/d859f6f8/btTriangleCallback.o \
	${OBJECTDIR}/_ext/d859f6f8/btTriangleIndexVertexArray.o \
	${OBJECTDIR}/_ext/d859f6f8/btTriangleIndexVertexMaterialArray.o \
	${OBJECTDIR}/_ext/d859f6f8/btTriangleMesh.o \
	${OBJECTDIR}/_ext/d859f6f8/btTriangleMeshShape.o \
	${OBJECTDIR}/_ext/d859f6f8/btUniformScalingShape.o \
	${OBJECTDIR}/_ext/9b8354a1/btContactProcessing.o \
	${OBJECTDIR}/_ext/9b8354a1/btGImpactBvh.o \
	${OBJECTDIR}/_ext/9b8354a1/btGImpactCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/9b8354a1/btGImpactQuantizedBvh.o \
	${OBJECTDIR}/_ext/9b8354a1/btGImpactShape.o \
	${OBJECTDIR}/_ext/9b8354a1/btGenericPoolAllocator.o \
	${OBJECTDIR}/_ext/9b8354a1/btTriangleShapeEx.o \
	${OBJECTDIR}/_ext/9b8354a1/gim_box_set.o \
	${OBJECTDIR}/_ext/9b8354a1/gim_contact.o \
	${OBJECTDIR}/_ext/9b8354a1/gim_memory.o \
	${OBJECTDIR}/_ext/9b8354a1/gim_tri_collision.o \
	${OBJECTDIR}/_ext/376ba57e/btContinuousConvexCollision.o \
	${OBJECTDIR}/_ext/376ba57e/btConvexCast.o \
	${OBJECTDIR}/_ext/376ba57e/btGjkConvexCast.o \
	${OBJECTDIR}/_ext/376ba57e/btGjkEpa2.o \
	${OBJECTDIR}/_ext/376ba57e/btGjkEpaPenetrationDepthSolver.o \
	${OBJECTDIR}/_ext/376ba57e/btGjkPairDetector.o \
	${OBJECTDIR}/_ext/376ba57e/btMinkowskiPenetrationDepthSolver.o \
	${OBJECTDIR}/_ext/376ba57e/btPersistentManifold.o \
	${OBJECTDIR}/_ext/376ba57e/btPolyhedralContactClipping.o \
	${OBJECTDIR}/_ext/376ba57e/btRaycastCallback.o \
	${OBJECTDIR}/_ext/376ba57e/btSubSimplexConvexCast.o \
	${OBJECTDIR}/_ext/376ba57e/btVoronoiSimplexSolver.o \
	${OBJECTDIR}/_ext/3e648ebd/btKinematicCharacterController.o \
	${OBJECTDIR}/_ext/64f7863c/btConeTwistConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btContactConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btFixedConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btGearConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btGeneric6DofConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btGeneric6DofSpring2Constraint.o \
	${OBJECTDIR}/_ext/64f7863c/btGeneric6DofSpringConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btHinge2Constraint.o \
	${OBJECTDIR}/_ext/64f7863c/btHingeConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btNNCGConstraintSolver.o \
	${OBJECTDIR}/_ext/64f7863c/btPoint2PointConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btSequentialImpulseConstraintSolver.o \
	${OBJECTDIR}/_ext/64f7863c/btSliderConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btSolve2LinearConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btTypedConstraint.o \
	${OBJECTDIR}/_ext/64f7863c/btUniversalConstraint.o \
	${OBJECTDIR}/_ext/e16b1200/btDiscreteDynamicsWorld.o \
	${OBJECTDIR}/_ext/e16b1200/btDiscreteDynamicsWorldMt.o \
	${OBJECTDIR}/_ext/e16b1200/btRigidBody.o \
	${OBJECTDIR}/_ext/e16b1200/btSimpleDynamicsWorld.o \
	${OBJECTDIR}/_ext/e16b1200/btSimulationIslandManagerMt.o \
	${OBJECTDIR}/_ext/53ef3d88/btDantzigLCP.o \
	${OBJECTDIR}/_ext/53ef3d88/btLemkeAlgorithm.o \
	${OBJECTDIR}/_ext/53ef3d88/btMLCPSolver.o \
	${OBJECTDIR}/_ext/df1437c0/btRaycastVehicle.o \
	${OBJECTDIR}/_ext/df1437c0/btWheelInfo.o \
	${OBJECTDIR}/_ext/829d05d5/IDMath.o \
	${OBJECTDIR}/_ext/829d05d5/MultiBodyTree.o \
	${OBJECTDIR}/_ext/b7e89008/MultiBodyTreeImpl.o \
	${OBJECTDIR}/_ext/b7e89008/MultiBodyTreeInitCache.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btDefaultSoftBodySolver.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btSoftBody.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyConcaveCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyHelpers.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyRigidBodyCollisionConfiguration.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btSoftMultiBodyDynamicsWorld.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btSoftRigidCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btSoftRigidDynamicsWorld.o \
	${OBJECTDIR}/_ext/ad2b1b9b/btSoftSoftCollisionAlgorithm.o \
	${OBJECTDIR}/_ext/6f43b5da/btAlignedAllocator.o \
	${OBJECTDIR}/_ext/6f43b5da/btConvexHull.o \
	${OBJECTDIR}/_ext/6f43b5da/btConvexHullComputer.o \
	${OBJECTDIR}/_ext/6f43b5da/btGeometryUtil.o \
	${OBJECTDIR}/_ext/6f43b5da/btPolarDecomposition.o \
	${OBJECTDIR}/_ext/6f43b5da/btQuickprof.o \
	${OBJECTDIR}/_ext/6f43b5da/btSerializer.o \
	${OBJECTDIR}/_ext/6f43b5da/btThreads.o \
	${OBJECTDIR}/_ext/6f43b5da/btVector3.o


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
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ASTCReader.o ../../src/finjin/engine/ASTCReader.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationDelegate.o: ../../src/finjin/engine/ApplicationDelegate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationDelegate.o ../../src/finjin/engine/ApplicationDelegate.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationSettings.o: ../../src/finjin/engine/ApplicationSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationSettings.o ../../src/finjin/engine/ApplicationSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewport.o: ../../src/finjin/engine/ApplicationViewport.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewport.o ../../src/finjin/engine/ApplicationViewport.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDelegate.o: ../../src/finjin/engine/ApplicationViewportDelegate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDelegate.o ../../src/finjin/engine/ApplicationViewportDelegate.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDescription.o: ../../src/finjin/engine/ApplicationViewportDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDescription.o ../../src/finjin/engine/ApplicationViewportDescription.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportsController.o: ../../src/finjin/engine/ApplicationViewportsController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportsController.o ../../src/finjin/engine/ApplicationViewportsController.cpp

${OBJECTDIR}/_ext/8ec51d02/Application_General.o: ../../src/finjin/engine/Application_General.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Application_General.o ../../src/finjin/engine/Application_General.cpp

${OBJECTDIR}/_ext/8ec51d02/Asset.o: ../../src/finjin/engine/Asset.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Asset.o ../../src/finjin/engine/Asset.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetClass.o: ../../src/finjin/engine/AssetClass.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetClass.o ../../src/finjin/engine/AssetClass.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetClassFileReader.o: ../../src/finjin/engine/AssetClassFileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetClassFileReader.o ../../src/finjin/engine/AssetClassFileReader.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetCountsSettings.o: ../../src/finjin/engine/AssetCountsSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetCountsSettings.o ../../src/finjin/engine/AssetCountsSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetFileReader.o: ../../src/finjin/engine/AssetFileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetFileReader.o ../../src/finjin/engine/AssetFileReader.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetPath.o: ../../src/finjin/engine/AssetPath.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetPath.o ../../src/finjin/engine/AssetPath.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReadHandle.o: ../../src/finjin/engine/AssetReadHandle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReadHandle.o ../../src/finjin/engine/AssetReadHandle.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReadQueue.o: ../../src/finjin/engine/AssetReadQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReadQueue.o ../../src/finjin/engine/AssetReadQueue.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReference.o: ../../src/finjin/engine/AssetReference.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReference.o ../../src/finjin/engine/AssetReference.cpp

${OBJECTDIR}/_ext/8ec51d02/Camera.o: ../../src/finjin/engine/Camera.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Camera.o ../../src/finjin/engine/Camera.cpp

${OBJECTDIR}/_ext/8ec51d02/CameraState.o: ../../src/finjin/engine/CameraState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/CameraState.o ../../src/finjin/engine/CameraState.cpp

${OBJECTDIR}/_ext/8ec51d02/DDSReader.o: ../../src/finjin/engine/DDSReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/DDSReader.o ../../src/finjin/engine/DDSReader.cpp

${OBJECTDIR}/_ext/8ec51d02/DXGIIncludes.o: ../../src/finjin/engine/DXGIIncludes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/DXGIIncludes.o ../../src/finjin/engine/DXGIIncludes.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinPrecompiled.o: ../../src/finjin/engine/FinjinPrecompiled.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinPrecompiled.o ../../src/finjin/engine/FinjinPrecompiled.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinSceneAssets.o: ../../src/finjin/engine/FinjinSceneAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinSceneAssets.o ../../src/finjin/engine/FinjinSceneAssets.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinSceneReader.o: ../../src/finjin/engine/FinjinSceneReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinSceneReader.o ../../src/finjin/engine/FinjinSceneReader.cpp

${OBJECTDIR}/_ext/8ec51d02/FlyingCameraInputBindings.o: ../../src/finjin/engine/FlyingCameraInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FlyingCameraInputBindings.o ../../src/finjin/engine/FlyingCameraInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/GenericGpuNumericStructs.o: ../../src/finjin/engine/GenericGpuNumericStructs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/GenericGpuNumericStructs.o ../../src/finjin/engine/GenericGpuNumericStructs.cpp

${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonSettings.o: ../../src/finjin/engine/GpuContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonSettings.o ../../src/finjin/engine/GpuContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/InputComponents.o: ../../src/finjin/engine/InputComponents.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputComponents.o ../../src/finjin/engine/InputComponents.cpp

${OBJECTDIR}/_ext/8ec51d02/InputConstants.o: ../../src/finjin/engine/InputConstants.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputConstants.o ../../src/finjin/engine/InputConstants.cpp

${OBJECTDIR}/_ext/8ec51d02/InputContextCommonSettings.o: ../../src/finjin/engine/InputContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputContextCommonSettings.o ../../src/finjin/engine/InputContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/InputCoordinate.o: ../../src/finjin/engine/InputCoordinate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputCoordinate.o ../../src/finjin/engine/InputCoordinate.cpp

${OBJECTDIR}/_ext/8ec51d02/InputSource.o: ../../src/finjin/engine/InputSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputSource.o ../../src/finjin/engine/InputSource.cpp

${OBJECTDIR}/_ext/8ec51d02/JobPipelineStage.o: ../../src/finjin/engine/JobPipelineStage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/JobPipelineStage.o ../../src/finjin/engine/JobPipelineStage.cpp

${OBJECTDIR}/_ext/8ec51d02/KTXReader.o: ../../src/finjin/engine/KTXReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/KTXReader.o ../../src/finjin/engine/KTXReader.cpp

${OBJECTDIR}/_ext/8ec51d02/LightState.o: ../../src/finjin/engine/LightState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/LightState.o ../../src/finjin/engine/LightState.cpp

${OBJECTDIR}/_ext/8ec51d02/MemorySettings.o: ../../src/finjin/engine/MemorySettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/MemorySettings.o ../../src/finjin/engine/MemorySettings.cpp

${OBJECTDIR}/_ext/8ec51d02/PVRReader.o: ../../src/finjin/engine/PVRReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/PVRReader.o ../../src/finjin/engine/PVRReader.cpp

${OBJECTDIR}/_ext/8ec51d02/PlatformCapabilities_General.o: ../../src/finjin/engine/PlatformCapabilities_General.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/PlatformCapabilities_General.o ../../src/finjin/engine/PlatformCapabilities_General.cpp

${OBJECTDIR}/_ext/8ec51d02/ShaderFeatureFlag.o: ../../src/finjin/engine/ShaderFeatureFlag.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ShaderFeatureFlag.o ../../src/finjin/engine/ShaderFeatureFlag.cpp

${OBJECTDIR}/_ext/8ec51d02/ShaderFeatures.o: ../../src/finjin/engine/ShaderFeatures.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ShaderFeatures.o ../../src/finjin/engine/ShaderFeatures.cpp

${OBJECTDIR}/_ext/8ec51d02/SoundContextCommonSettings.o: ../../src/finjin/engine/SoundContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/SoundContextCommonSettings.o ../../src/finjin/engine/SoundContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/SoundFormat.o: ../../src/finjin/engine/SoundFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/SoundFormat.o ../../src/finjin/engine/SoundFormat.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentChunkNames.o: ../../src/finjin/engine/StandardAssetDocumentChunkNames.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentChunkNames.o ../../src/finjin/engine/StandardAssetDocumentChunkNames.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyNames.o: ../../src/finjin/engine/StandardAssetDocumentPropertyNames.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyNames.o ../../src/finjin/engine/StandardAssetDocumentPropertyNames.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyValues.o: ../../src/finjin/engine/StandardAssetDocumentPropertyValues.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyValues.o ../../src/finjin/engine/StandardAssetDocumentPropertyValues.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardGameControllerInputBindings.o: ../../src/finjin/engine/StandardGameControllerInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardGameControllerInputBindings.o ../../src/finjin/engine/StandardGameControllerInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/StringTable.o: ../../src/finjin/engine/StringTable.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StringTable.o ../../src/finjin/engine/StringTable.cpp

${OBJECTDIR}/_ext/8ec51d02/TextureDimension.o: ../../src/finjin/engine/TextureDimension.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/TextureDimension.o ../../src/finjin/engine/TextureDimension.cpp

${OBJECTDIR}/_ext/8ec51d02/UserDataTypes.o: ../../src/finjin/engine/UserDataTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/UserDataTypes.o ../../src/finjin/engine/UserDataTypes.cpp

${OBJECTDIR}/_ext/8ec51d02/VRContextCommonSettings.o: ../../src/finjin/engine/VRContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/VRContextCommonSettings.o ../../src/finjin/engine/VRContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/VRGameControllerInputBindings.o: ../../src/finjin/engine/VRGameControllerInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/VRGameControllerInputBindings.o ../../src/finjin/engine/VRGameControllerInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/WAVReader.o: ../../src/finjin/engine/WAVReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WAVReader.o ../../src/finjin/engine/WAVReader.cpp

${OBJECTDIR}/_ext/8ec51d02/WindowBounds.o: ../../src/finjin/engine/WindowBounds.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WindowBounds.o ../../src/finjin/engine/WindowBounds.cpp

${OBJECTDIR}/_ext/8ec51d02/WindowSize.o: ../../src/finjin/engine/WindowSize.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WindowSize.o ../../src/finjin/engine/WindowSize.cpp

${OBJECTDIR}/_ext/40bb3adf/Application.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/Application.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/Application.o ../../src/finjin/engine/internal/app/linux/finjin/engine/Application.cpp

${OBJECTDIR}/_ext/40bb3adf/ApplicationGlobals.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/ApplicationGlobals.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/ApplicationGlobals.o ../../src/finjin/engine/internal/app/linux/finjin/engine/ApplicationGlobals.cpp

${OBJECTDIR}/_ext/40bb3adf/DisplayInfo.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/DisplayInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/DisplayInfo.o ../../src/finjin/engine/internal/app/linux/finjin/engine/DisplayInfo.cpp

${OBJECTDIR}/_ext/40bb3adf/FinjinMain.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/FinjinMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/FinjinMain.o ../../src/finjin/engine/internal/app/linux/finjin/engine/FinjinMain.cpp

${OBJECTDIR}/_ext/40bb3adf/OSWindow.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/OSWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/OSWindow.o ../../src/finjin/engine/internal/app/linux/finjin/engine/OSWindow.cpp

${OBJECTDIR}/_ext/40bb3adf/PlatformCapabilities.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/PlatformCapabilities.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/PlatformCapabilities.o ../../src/finjin/engine/internal/app/linux/finjin/engine/PlatformCapabilities.cpp

${OBJECTDIR}/_ext/40bb3adf/XcbConnection.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/XcbConnection.o ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbConnection.cpp

${OBJECTDIR}/_ext/40bb3adf/XcbWindow.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/XcbWindow.o ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbWindow.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanDescriptorSetLayout.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanDescriptorSetLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanDescriptorSetLayout.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanDescriptorSetLayout.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanFrameBuffer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanFrameBuffer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameBuffer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanFrameStage.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameStage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanFrameStage.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameStage.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuBuffer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuBuffer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuBuffer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContext.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContext.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContext.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextImpl.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextImpl.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextImpl.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextSettings.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextSettings.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextSettings.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuDescription.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuDescription.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuDescription.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanIncludes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanIncludes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanIncludes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanIncludes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanInputFormat.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanInputFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanInputFormat.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanInputFormat.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanLayerProperties.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanLayerProperties.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanLayerProperties.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanLayerProperties.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanLight.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanLight.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanLight.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanLight.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMaterial.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMaterial.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMaterial.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMaterial.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMemoryResource.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMemoryResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMemoryResource.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMemoryResource.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMesh.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMesh.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMesh.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPipeline.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipeline.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPipeline.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipeline.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPipelineLayout.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipelineLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPipelineLayout.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipelineLayout.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPresentModes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPresentModes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPresentModes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPresentModes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPrimaryQueues.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPrimaryQueues.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPrimaryQueues.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPrimaryQueues.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanQueueFamilyIndexes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanQueueFamilyIndexes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanQueueFamilyIndexes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanQueueFamilyIndexes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanRenderTarget.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanRenderTarget.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanRenderTarget.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanRenderTarget.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanResources.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanResources.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanResources.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanResources.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanShader.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanShader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanShader.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanShader.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanStaticMeshRenderer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanStaticMeshRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanStaticMeshRenderer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanStaticMeshRenderer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSurfaceFormats.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSurfaceFormats.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSurfaceFormats.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSurfaceFormats.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystem.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystem.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystem.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystemImpl.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystemImpl.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemImpl.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystemSettings.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystemSettings.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemSettings.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanTexture.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanTexture.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanTexture.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanTexture.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanUtilities.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanUtilities.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanUtilities.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanUtilities.cpp

${OBJECTDIR}/_ext/32e60500/LinuxGameController.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxGameController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxGameController.o ../../src/finjin/engine/internal/input/linuxinput/LinuxGameController.cpp

${OBJECTDIR}/_ext/32e60500/LinuxInputContext.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxInputContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxInputContext.o ../../src/finjin/engine/internal/input/linuxinput/LinuxInputContext.cpp

${OBJECTDIR}/_ext/32e60500/LinuxInputSystem.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxInputSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxInputSystem.o ../../src/finjin/engine/internal/input/linuxinput/LinuxInputSystem.cpp

${OBJECTDIR}/_ext/32e60500/LinuxKeyboard.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxKeyboard.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxKeyboard.o ../../src/finjin/engine/internal/input/linuxinput/LinuxKeyboard.cpp

${OBJECTDIR}/_ext/32e60500/LinuxMouse.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxMouse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxMouse.o ../../src/finjin/engine/internal/input/linuxinput/LinuxMouse.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALAdapterDescription.o: ../../src/finjin/engine/internal/sound/openal/OpenALAdapterDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALAdapterDescription.o ../../src/finjin/engine/internal/sound/openal/OpenALAdapterDescription.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALContext.o: ../../src/finjin/engine/internal/sound/openal/OpenALContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALContext.o ../../src/finjin/engine/internal/sound/openal/OpenALContext.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALContextImpl.o: ../../src/finjin/engine/internal/sound/openal/OpenALContextImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALContextImpl.o ../../src/finjin/engine/internal/sound/openal/OpenALContextImpl.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALEffect.o: ../../src/finjin/engine/internal/sound/openal/OpenALEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALEffect.o ../../src/finjin/engine/internal/sound/openal/OpenALEffect.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALEffectSlot.o: ../../src/finjin/engine/internal/sound/openal/OpenALEffectSlot.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALEffectSlot.o ../../src/finjin/engine/internal/sound/openal/OpenALEffectSlot.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALExtensions.o: ../../src/finjin/engine/internal/sound/openal/OpenALExtensions.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALExtensions.o ../../src/finjin/engine/internal/sound/openal/OpenALExtensions.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALFilter.o: ../../src/finjin/engine/internal/sound/openal/OpenALFilter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALFilter.o ../../src/finjin/engine/internal/sound/openal/OpenALFilter.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALListener.o: ../../src/finjin/engine/internal/sound/openal/OpenALListener.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALListener.o ../../src/finjin/engine/internal/sound/openal/OpenALListener.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSoundBuffer.o: ../../src/finjin/engine/internal/sound/openal/OpenALSoundBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSoundBuffer.o ../../src/finjin/engine/internal/sound/openal/OpenALSoundBuffer.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSoundSource.o: ../../src/finjin/engine/internal/sound/openal/OpenALSoundSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSoundSource.o ../../src/finjin/engine/internal/sound/openal/OpenALSoundSource.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSystem.o: ../../src/finjin/engine/internal/sound/openal/OpenALSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSystem.o ../../src/finjin/engine/internal/sound/openal/OpenALSystem.cpp

${OBJECTDIR}/_ext/94966a78/b3DynamicBvh.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/94966a78
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/94966a78/b3DynamicBvh.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvh.cpp

${OBJECTDIR}/_ext/94966a78/b3DynamicBvhBroadphase.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/94966a78
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/94966a78/b3DynamicBvhBroadphase.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/BroadPhaseCollision/b3DynamicBvhBroadphase.cpp

${OBJECTDIR}/_ext/94966a78/b3OverlappingPairCache.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/94966a78
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/94966a78/b3OverlappingPairCache.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/BroadPhaseCollision/b3OverlappingPairCache.cpp

${OBJECTDIR}/_ext/613189b3/b3ConvexUtility.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/613189b3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/613189b3/b3ConvexUtility.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/NarrowPhaseCollision/b3ConvexUtility.cpp

${OBJECTDIR}/_ext/613189b3/b3CpuNarrowPhase.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/613189b3
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/613189b3/b3CpuNarrowPhase.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Collision/NarrowPhaseCollision/b3CpuNarrowPhase.cpp

${OBJECTDIR}/_ext/15f39b8f/b3AlignedAllocator.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Common/b3AlignedAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/15f39b8f
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/15f39b8f/b3AlignedAllocator.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Common/b3AlignedAllocator.cpp

${OBJECTDIR}/_ext/15f39b8f/b3Logging.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Common/b3Logging.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/15f39b8f
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/15f39b8f/b3Logging.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Common/b3Logging.cpp

${OBJECTDIR}/_ext/15f39b8f/b3Vector3.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Common/b3Vector3.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/15f39b8f
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/15f39b8f/b3Vector3.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Common/b3Vector3.cpp

${OBJECTDIR}/_ext/908714e7/b3FixedConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3FixedConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/908714e7
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/908714e7/b3FixedConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3FixedConstraint.cpp

${OBJECTDIR}/_ext/908714e7/b3Generic6DofConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/908714e7
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/908714e7/b3Generic6DofConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3Generic6DofConstraint.cpp

${OBJECTDIR}/_ext/908714e7/b3PgsJacobiSolver.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3PgsJacobiSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/908714e7
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/908714e7/b3PgsJacobiSolver.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3PgsJacobiSolver.cpp

${OBJECTDIR}/_ext/908714e7/b3Point2PointConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3Point2PointConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/908714e7
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/908714e7/b3Point2PointConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3Point2PointConstraint.cpp

${OBJECTDIR}/_ext/908714e7/b3TypedConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3TypedConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/908714e7
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/908714e7/b3TypedConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/ConstraintSolver/b3TypedConstraint.cpp

${OBJECTDIR}/_ext/e16540b8/b3CpuRigidBodyPipeline.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/b3CpuRigidBodyPipeline.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e16540b8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e16540b8/b3CpuRigidBodyPipeline.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Dynamics/b3CpuRigidBodyPipeline.cpp

${OBJECTDIR}/_ext/f966a116/b3ConvexHullComputer.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Geometry/b3ConvexHullComputer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f966a116
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f966a116/b3ConvexHullComputer.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Geometry/b3ConvexHullComputer.cpp

${OBJECTDIR}/_ext/f966a116/b3GeometryUtil.o: /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Geometry/b3GeometryUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f966a116
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f966a116/b3GeometryUtil.o /home/build/finjin-3rd-party/cpp/bullet3/src/Bullet3Geometry/b3GeometryUtil.cpp

${OBJECTDIR}/_ext/29cd5d2d/btAxisSweep3.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btAxisSweep3.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btAxisSweep3.cpp

${OBJECTDIR}/_ext/29cd5d2d/btBroadphaseProxy.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btBroadphaseProxy.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btBroadphaseProxy.cpp

${OBJECTDIR}/_ext/29cd5d2d/btCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/29cd5d2d/btDbvt.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btDbvt.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btDbvt.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btDbvt.cpp

${OBJECTDIR}/_ext/29cd5d2d/btDbvtBroadphase.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btDbvtBroadphase.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btDbvtBroadphase.cpp

${OBJECTDIR}/_ext/29cd5d2d/btDispatcher.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btDispatcher.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btDispatcher.cpp

${OBJECTDIR}/_ext/29cd5d2d/btOverlappingPairCache.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btOverlappingPairCache.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btOverlappingPairCache.cpp

${OBJECTDIR}/_ext/29cd5d2d/btQuantizedBvh.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btQuantizedBvh.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btQuantizedBvh.cpp

${OBJECTDIR}/_ext/29cd5d2d/btSimpleBroadphase.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/29cd5d2d
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/29cd5d2d/btSimpleBroadphase.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/BroadphaseCollision/btSimpleBroadphase.cpp

${OBJECTDIR}/_ext/67222d00/SphereTriangleDetector.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/SphereTriangleDetector.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/SphereTriangleDetector.cpp

${OBJECTDIR}/_ext/67222d00/btActivatingCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btActivatingCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btActivatingCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btBox2dBox2dCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btBox2dBox2dCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btBox2dBox2dCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btBoxBoxCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btBoxBoxCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btBoxBoxCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btBoxBoxDetector.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btBoxBoxDetector.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btBoxBoxDetector.cpp

${OBJECTDIR}/_ext/67222d00/btCollisionDispatcher.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btCollisionDispatcher.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCollisionDispatcher.cpp

${OBJECTDIR}/_ext/67222d00/btCollisionObject.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btCollisionObject.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCollisionObject.cpp

${OBJECTDIR}/_ext/67222d00/btCollisionWorld.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btCollisionWorld.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCollisionWorld.cpp

${OBJECTDIR}/_ext/67222d00/btCollisionWorldImporter.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCollisionWorldImporter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btCollisionWorldImporter.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCollisionWorldImporter.cpp

${OBJECTDIR}/_ext/67222d00/btCompoundCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btCompoundCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCompoundCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btCompoundCompoundCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btCompoundCompoundCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btCompoundCompoundCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btConvex2dConvex2dAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btConvex2dConvex2dAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btConvex2dConvex2dAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btConvexConcaveCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btConvexConcaveCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btConvexConcaveCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btConvexConvexAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btConvexConvexAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btConvexConvexAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btConvexPlaneCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btConvexPlaneCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btConvexPlaneCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btDefaultCollisionConfiguration.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btDefaultCollisionConfiguration.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.cpp

${OBJECTDIR}/_ext/67222d00/btEmptyCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btEmptyCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btEmptyCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btGhostObject.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btGhostObject.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btGhostObject.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btGhostObject.cpp

${OBJECTDIR}/_ext/67222d00/btHashedSimplePairCache.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btHashedSimplePairCache.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btHashedSimplePairCache.cpp

${OBJECTDIR}/_ext/67222d00/btInternalEdgeUtility.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btInternalEdgeUtility.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btInternalEdgeUtility.cpp

${OBJECTDIR}/_ext/67222d00/btManifoldResult.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btManifoldResult.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btManifoldResult.cpp

${OBJECTDIR}/_ext/67222d00/btSimulationIslandManager.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btSimulationIslandManager.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btSimulationIslandManager.cpp

${OBJECTDIR}/_ext/67222d00/btSphereBoxCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btSphereBoxCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btSphereBoxCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btSphereSphereCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btSphereSphereCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btSphereSphereCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btSphereTriangleCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btSphereTriangleCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btSphereTriangleCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/67222d00/btUnionFind.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btUnionFind.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/67222d00
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/67222d00/btUnionFind.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionDispatch/btUnionFind.cpp

${OBJECTDIR}/_ext/d859f6f8/btBox2dShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btBox2dShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btBox2dShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btBox2dShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btBoxShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btBoxShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btBoxShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btBoxShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btBvhTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btBvhTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btBvhTriangleMeshShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btCapsuleShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btCapsuleShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btCapsuleShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btCollisionShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btCollisionShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btCollisionShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btCollisionShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btCompoundShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btCompoundShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btCompoundShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btCompoundShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btConcaveShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConcaveShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConcaveShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConcaveShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btConeShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConeShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConeShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConeShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btConvex2dShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConvex2dShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvex2dShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btConvexHullShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConvexHullShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexHullShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btConvexInternalShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConvexInternalShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexInternalShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btConvexPointCloudShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConvexPointCloudShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexPointCloudShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btConvexPolyhedron.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConvexPolyhedron.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexPolyhedron.cpp

${OBJECTDIR}/_ext/d859f6f8/btConvexShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConvexShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btConvexTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btConvexTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btCylinderShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btCylinderShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btCylinderShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btCylinderShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btEmptyShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btEmptyShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btEmptyShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btEmptyShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btHeightfieldTerrainShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btHeightfieldTerrainShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btMinkowskiSumShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btMinkowskiSumShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btMinkowskiSumShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btMultiSphereShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btMultiSphereShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btMultiSphereShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btMultimaterialTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btMultimaterialTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btMultimaterialTriangleMeshShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btOptimizedBvh.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btOptimizedBvh.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btOptimizedBvh.cpp

${OBJECTDIR}/_ext/d859f6f8/btPolyhedralConvexShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btPolyhedralConvexShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btPolyhedralConvexShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btScaledBvhTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btScaledBvhTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btScaledBvhTriangleMeshShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btShapeHull.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btShapeHull.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btShapeHull.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btShapeHull.cpp

${OBJECTDIR}/_ext/d859f6f8/btSphereShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btSphereShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btSphereShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btSphereShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btStaticPlaneShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btStaticPlaneShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btStaticPlaneShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btStridingMeshInterface.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btStridingMeshInterface.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btStridingMeshInterface.cpp

${OBJECTDIR}/_ext/d859f6f8/btTetrahedronShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btTetrahedronShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTetrahedronShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btTriangleBuffer.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btTriangleBuffer.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleBuffer.cpp

${OBJECTDIR}/_ext/d859f6f8/btTriangleCallback.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btTriangleCallback.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleCallback.cpp

${OBJECTDIR}/_ext/d859f6f8/btTriangleIndexVertexArray.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btTriangleIndexVertexArray.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleIndexVertexArray.cpp

${OBJECTDIR}/_ext/d859f6f8/btTriangleIndexVertexMaterialArray.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btTriangleIndexVertexMaterialArray.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleIndexVertexMaterialArray.cpp

${OBJECTDIR}/_ext/d859f6f8/btTriangleMesh.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btTriangleMesh.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleMesh.cpp

${OBJECTDIR}/_ext/d859f6f8/btTriangleMeshShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btTriangleMeshShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btTriangleMeshShape.cpp

${OBJECTDIR}/_ext/d859f6f8/btUniformScalingShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/d859f6f8
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/d859f6f8/btUniformScalingShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/CollisionShapes/btUniformScalingShape.cpp

${OBJECTDIR}/_ext/9b8354a1/btContactProcessing.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btContactProcessing.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/btContactProcessing.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btContactProcessing.cpp

${OBJECTDIR}/_ext/9b8354a1/btGImpactBvh.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGImpactBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/btGImpactBvh.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGImpactBvh.cpp

${OBJECTDIR}/_ext/9b8354a1/btGImpactCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/btGImpactCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGImpactCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/9b8354a1/btGImpactQuantizedBvh.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/btGImpactQuantizedBvh.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGImpactQuantizedBvh.cpp

${OBJECTDIR}/_ext/9b8354a1/btGImpactShape.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGImpactShape.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/btGImpactShape.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGImpactShape.cpp

${OBJECTDIR}/_ext/9b8354a1/btGenericPoolAllocator.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/btGenericPoolAllocator.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btGenericPoolAllocator.cpp

${OBJECTDIR}/_ext/9b8354a1/btTriangleShapeEx.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/btTriangleShapeEx.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/btTriangleShapeEx.cpp

${OBJECTDIR}/_ext/9b8354a1/gim_box_set.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/gim_box_set.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/gim_box_set.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/gim_box_set.cpp

${OBJECTDIR}/_ext/9b8354a1/gim_contact.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/gim_contact.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/gim_contact.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/gim_contact.cpp

${OBJECTDIR}/_ext/9b8354a1/gim_memory.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/gim_memory.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/gim_memory.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/gim_memory.cpp

${OBJECTDIR}/_ext/9b8354a1/gim_tri_collision.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/gim_tri_collision.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/9b8354a1
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/9b8354a1/gim_tri_collision.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/Gimpact/gim_tri_collision.cpp

${OBJECTDIR}/_ext/376ba57e/btContinuousConvexCollision.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btContinuousConvexCollision.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btContinuousConvexCollision.cpp

${OBJECTDIR}/_ext/376ba57e/btConvexCast.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btConvexCast.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btConvexCast.cpp

${OBJECTDIR}/_ext/376ba57e/btGjkConvexCast.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btGjkConvexCast.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btGjkConvexCast.cpp

${OBJECTDIR}/_ext/376ba57e/btGjkEpa2.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btGjkEpa2.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btGjkEpa2.cpp

${OBJECTDIR}/_ext/376ba57e/btGjkEpaPenetrationDepthSolver.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btGjkEpaPenetrationDepthSolver.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.cpp

${OBJECTDIR}/_ext/376ba57e/btGjkPairDetector.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btGjkPairDetector.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btGjkPairDetector.cpp

${OBJECTDIR}/_ext/376ba57e/btMinkowskiPenetrationDepthSolver.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btMinkowskiPenetrationDepthSolver.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.cpp

${OBJECTDIR}/_ext/376ba57e/btPersistentManifold.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btPersistentManifold.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btPersistentManifold.cpp

${OBJECTDIR}/_ext/376ba57e/btPolyhedralContactClipping.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btPolyhedralContactClipping.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btPolyhedralContactClipping.cpp

${OBJECTDIR}/_ext/376ba57e/btRaycastCallback.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btRaycastCallback.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btRaycastCallback.cpp

${OBJECTDIR}/_ext/376ba57e/btSubSimplexConvexCast.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btSubSimplexConvexCast.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.cpp

${OBJECTDIR}/_ext/376ba57e/btVoronoiSimplexSolver.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/376ba57e
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/376ba57e/btVoronoiSimplexSolver.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.cpp

${OBJECTDIR}/_ext/3e648ebd/btKinematicCharacterController.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Character/btKinematicCharacterController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/3e648ebd
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/3e648ebd/btKinematicCharacterController.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Character/btKinematicCharacterController.cpp

${OBJECTDIR}/_ext/64f7863c/btConeTwistConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btConeTwistConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btConeTwistConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btContactConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btContactConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btContactConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btFixedConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btFixedConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btFixedConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btGearConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btGearConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btGearConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btGearConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btGeneric6DofConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btGeneric6DofConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btGeneric6DofSpring2Constraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btGeneric6DofSpring2Constraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpring2Constraint.cpp

${OBJECTDIR}/_ext/64f7863c/btGeneric6DofSpringConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btGeneric6DofSpringConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btGeneric6DofSpringConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btHinge2Constraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btHinge2Constraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btHinge2Constraint.cpp

${OBJECTDIR}/_ext/64f7863c/btHingeConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btHingeConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btHingeConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btNNCGConstraintSolver.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btNNCGConstraintSolver.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.cpp

${OBJECTDIR}/_ext/64f7863c/btPoint2PointConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btPoint2PointConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btSequentialImpulseConstraintSolver.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btSequentialImpulseConstraintSolver.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.cpp

${OBJECTDIR}/_ext/64f7863c/btSliderConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btSliderConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btSliderConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btSolve2LinearConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btSolve2LinearConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btSolve2LinearConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btTypedConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btTypedConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btTypedConstraint.cpp

${OBJECTDIR}/_ext/64f7863c/btUniversalConstraint.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/64f7863c
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/64f7863c/btUniversalConstraint.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/ConstraintSolver/btUniversalConstraint.cpp

${OBJECTDIR}/_ext/e16b1200/btDiscreteDynamicsWorld.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e16b1200
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e16b1200/btDiscreteDynamicsWorld.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.cpp

${OBJECTDIR}/_ext/e16b1200/btDiscreteDynamicsWorldMt.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e16b1200
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e16b1200/btDiscreteDynamicsWorldMt.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.cpp

${OBJECTDIR}/_ext/e16b1200/btRigidBody.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btRigidBody.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e16b1200
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e16b1200/btRigidBody.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btRigidBody.cpp

${OBJECTDIR}/_ext/e16b1200/btSimpleDynamicsWorld.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e16b1200
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e16b1200/btSimpleDynamicsWorld.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btSimpleDynamicsWorld.cpp

${OBJECTDIR}/_ext/e16b1200/btSimulationIslandManagerMt.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btSimulationIslandManagerMt.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/e16b1200
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/e16b1200/btSimulationIslandManagerMt.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Dynamics/btSimulationIslandManagerMt.cpp

${OBJECTDIR}/_ext/53ef3d88/btDantzigLCP.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53ef3d88
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53ef3d88/btDantzigLCP.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/MLCPSolvers/btDantzigLCP.cpp

${OBJECTDIR}/_ext/53ef3d88/btLemkeAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/MLCPSolvers/btLemkeAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53ef3d88
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53ef3d88/btLemkeAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/MLCPSolvers/btLemkeAlgorithm.cpp

${OBJECTDIR}/_ext/53ef3d88/btMLCPSolver.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/53ef3d88
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/53ef3d88/btMLCPSolver.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/MLCPSolvers/btMLCPSolver.cpp

${OBJECTDIR}/_ext/df1437c0/btRaycastVehicle.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/df1437c0
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/df1437c0/btRaycastVehicle.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Vehicle/btRaycastVehicle.cpp

${OBJECTDIR}/_ext/df1437c0/btWheelInfo.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Vehicle/btWheelInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/df1437c0
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/df1437c0/btWheelInfo.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletDynamics/Vehicle/btWheelInfo.cpp

${OBJECTDIR}/_ext/829d05d5/IDMath.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletInverseDynamics/IDMath.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/829d05d5
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/829d05d5/IDMath.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletInverseDynamics/IDMath.cpp

${OBJECTDIR}/_ext/829d05d5/MultiBodyTree.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletInverseDynamics/MultiBodyTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/829d05d5
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/829d05d5/MultiBodyTree.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletInverseDynamics/MultiBodyTree.cpp

${OBJECTDIR}/_ext/b7e89008/MultiBodyTreeImpl.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletInverseDynamics/details/MultiBodyTreeImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b7e89008
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b7e89008/MultiBodyTreeImpl.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletInverseDynamics/details/MultiBodyTreeImpl.cpp

${OBJECTDIR}/_ext/b7e89008/MultiBodyTreeInitCache.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletInverseDynamics/details/MultiBodyTreeInitCache.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b7e89008
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b7e89008/MultiBodyTreeInitCache.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletInverseDynamics/details/MultiBodyTreeInitCache.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btDefaultSoftBodySolver.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btDefaultSoftBodySolver.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btDefaultSoftBodySolver.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btDefaultSoftBodySolver.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btSoftBody.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftBody.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btSoftBody.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftBody.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyConcaveCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyConcaveCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftBodyConcaveCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyHelpers.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftBodyHelpers.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyHelpers.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftBodyHelpers.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyRigidBodyCollisionConfiguration.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btSoftBodyRigidBodyCollisionConfiguration.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btSoftMultiBodyDynamicsWorld.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftMultiBodyDynamicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btSoftMultiBodyDynamicsWorld.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftMultiBodyDynamicsWorld.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btSoftRigidCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btSoftRigidCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftRigidCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btSoftRigidDynamicsWorld.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btSoftRigidDynamicsWorld.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftRigidDynamicsWorld.cpp

${OBJECTDIR}/_ext/ad2b1b9b/btSoftSoftCollisionAlgorithm.o: /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/ad2b1b9b
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/ad2b1b9b/btSoftSoftCollisionAlgorithm.o /home/build/finjin-3rd-party/cpp/bullet3/src/BulletSoftBody/btSoftSoftCollisionAlgorithm.cpp

${OBJECTDIR}/_ext/6f43b5da/btAlignedAllocator.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btAlignedAllocator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btAlignedAllocator.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btAlignedAllocator.cpp

${OBJECTDIR}/_ext/6f43b5da/btConvexHull.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btConvexHull.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btConvexHull.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btConvexHull.cpp

${OBJECTDIR}/_ext/6f43b5da/btConvexHullComputer.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btConvexHullComputer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btConvexHullComputer.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btConvexHullComputer.cpp

${OBJECTDIR}/_ext/6f43b5da/btGeometryUtil.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btGeometryUtil.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btGeometryUtil.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btGeometryUtil.cpp

${OBJECTDIR}/_ext/6f43b5da/btPolarDecomposition.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btPolarDecomposition.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btPolarDecomposition.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btPolarDecomposition.cpp

${OBJECTDIR}/_ext/6f43b5da/btQuickprof.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btQuickprof.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btQuickprof.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btQuickprof.cpp

${OBJECTDIR}/_ext/6f43b5da/btSerializer.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btSerializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btSerializer.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btSerializer.cpp

${OBJECTDIR}/_ext/6f43b5da/btThreads.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btThreads.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btThreads.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btThreads.cpp

${OBJECTDIR}/_ext/6f43b5da/btVector3.o: /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btVector3.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/6f43b5da
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I/home/build/finjin-3rd-party/cpp/bullet3/src -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/6f43b5da/btVector3.o /home/build/finjin-3rd-party/cpp/bullet3/src/LinearMath/btVector3.cpp

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
