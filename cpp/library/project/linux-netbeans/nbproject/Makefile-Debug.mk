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
	${OBJECTDIR}/_ext/b85d39ba/OpenALSystem.o


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
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ASTCReader.o ../../src/finjin/engine/ASTCReader.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationDelegate.o: ../../src/finjin/engine/ApplicationDelegate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationDelegate.o ../../src/finjin/engine/ApplicationDelegate.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationSettings.o: ../../src/finjin/engine/ApplicationSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationSettings.o ../../src/finjin/engine/ApplicationSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewport.o: ../../src/finjin/engine/ApplicationViewport.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewport.o ../../src/finjin/engine/ApplicationViewport.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDelegate.o: ../../src/finjin/engine/ApplicationViewportDelegate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDelegate.o ../../src/finjin/engine/ApplicationViewportDelegate.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDescription.o: ../../src/finjin/engine/ApplicationViewportDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportDescription.o ../../src/finjin/engine/ApplicationViewportDescription.cpp

${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportsController.o: ../../src/finjin/engine/ApplicationViewportsController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ApplicationViewportsController.o ../../src/finjin/engine/ApplicationViewportsController.cpp

${OBJECTDIR}/_ext/8ec51d02/Application_General.o: ../../src/finjin/engine/Application_General.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Application_General.o ../../src/finjin/engine/Application_General.cpp

${OBJECTDIR}/_ext/8ec51d02/Asset.o: ../../src/finjin/engine/Asset.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Asset.o ../../src/finjin/engine/Asset.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetClass.o: ../../src/finjin/engine/AssetClass.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetClass.o ../../src/finjin/engine/AssetClass.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetClassFileReader.o: ../../src/finjin/engine/AssetClassFileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetClassFileReader.o ../../src/finjin/engine/AssetClassFileReader.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetCountsSettings.o: ../../src/finjin/engine/AssetCountsSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetCountsSettings.o ../../src/finjin/engine/AssetCountsSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetFileReader.o: ../../src/finjin/engine/AssetFileReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetFileReader.o ../../src/finjin/engine/AssetFileReader.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetPath.o: ../../src/finjin/engine/AssetPath.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetPath.o ../../src/finjin/engine/AssetPath.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReadHandle.o: ../../src/finjin/engine/AssetReadHandle.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReadHandle.o ../../src/finjin/engine/AssetReadHandle.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReadQueue.o: ../../src/finjin/engine/AssetReadQueue.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReadQueue.o ../../src/finjin/engine/AssetReadQueue.cpp

${OBJECTDIR}/_ext/8ec51d02/AssetReference.o: ../../src/finjin/engine/AssetReference.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/AssetReference.o ../../src/finjin/engine/AssetReference.cpp

${OBJECTDIR}/_ext/8ec51d02/Camera.o: ../../src/finjin/engine/Camera.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/Camera.o ../../src/finjin/engine/Camera.cpp

${OBJECTDIR}/_ext/8ec51d02/CameraState.o: ../../src/finjin/engine/CameraState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/CameraState.o ../../src/finjin/engine/CameraState.cpp

${OBJECTDIR}/_ext/8ec51d02/DDSReader.o: ../../src/finjin/engine/DDSReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/DDSReader.o ../../src/finjin/engine/DDSReader.cpp

${OBJECTDIR}/_ext/8ec51d02/DXGIIncludes.o: ../../src/finjin/engine/DXGIIncludes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/DXGIIncludes.o ../../src/finjin/engine/DXGIIncludes.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinPrecompiled.o: ../../src/finjin/engine/FinjinPrecompiled.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinPrecompiled.o ../../src/finjin/engine/FinjinPrecompiled.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinSceneAssets.o: ../../src/finjin/engine/FinjinSceneAssets.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinSceneAssets.o ../../src/finjin/engine/FinjinSceneAssets.cpp

${OBJECTDIR}/_ext/8ec51d02/FinjinSceneReader.o: ../../src/finjin/engine/FinjinSceneReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FinjinSceneReader.o ../../src/finjin/engine/FinjinSceneReader.cpp

${OBJECTDIR}/_ext/8ec51d02/FlyingCameraInputBindings.o: ../../src/finjin/engine/FlyingCameraInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/FlyingCameraInputBindings.o ../../src/finjin/engine/FlyingCameraInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/GenericGpuNumericStructs.o: ../../src/finjin/engine/GenericGpuNumericStructs.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/GenericGpuNumericStructs.o ../../src/finjin/engine/GenericGpuNumericStructs.cpp

${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonSettings.o: ../../src/finjin/engine/GpuContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/GpuContextCommonSettings.o ../../src/finjin/engine/GpuContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/InputComponents.o: ../../src/finjin/engine/InputComponents.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputComponents.o ../../src/finjin/engine/InputComponents.cpp

${OBJECTDIR}/_ext/8ec51d02/InputConstants.o: ../../src/finjin/engine/InputConstants.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputConstants.o ../../src/finjin/engine/InputConstants.cpp

${OBJECTDIR}/_ext/8ec51d02/InputContextCommonSettings.o: ../../src/finjin/engine/InputContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputContextCommonSettings.o ../../src/finjin/engine/InputContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/InputCoordinate.o: ../../src/finjin/engine/InputCoordinate.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputCoordinate.o ../../src/finjin/engine/InputCoordinate.cpp

${OBJECTDIR}/_ext/8ec51d02/InputSource.o: ../../src/finjin/engine/InputSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/InputSource.o ../../src/finjin/engine/InputSource.cpp

${OBJECTDIR}/_ext/8ec51d02/JobPipelineStage.o: ../../src/finjin/engine/JobPipelineStage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/JobPipelineStage.o ../../src/finjin/engine/JobPipelineStage.cpp

${OBJECTDIR}/_ext/8ec51d02/KTXReader.o: ../../src/finjin/engine/KTXReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/KTXReader.o ../../src/finjin/engine/KTXReader.cpp

${OBJECTDIR}/_ext/8ec51d02/LightState.o: ../../src/finjin/engine/LightState.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/LightState.o ../../src/finjin/engine/LightState.cpp

${OBJECTDIR}/_ext/8ec51d02/MemorySettings.o: ../../src/finjin/engine/MemorySettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/MemorySettings.o ../../src/finjin/engine/MemorySettings.cpp

${OBJECTDIR}/_ext/8ec51d02/PVRReader.o: ../../src/finjin/engine/PVRReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/PVRReader.o ../../src/finjin/engine/PVRReader.cpp

${OBJECTDIR}/_ext/8ec51d02/PlatformCapabilities_General.o: ../../src/finjin/engine/PlatformCapabilities_General.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/PlatformCapabilities_General.o ../../src/finjin/engine/PlatformCapabilities_General.cpp

${OBJECTDIR}/_ext/8ec51d02/ShaderFeatureFlag.o: ../../src/finjin/engine/ShaderFeatureFlag.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ShaderFeatureFlag.o ../../src/finjin/engine/ShaderFeatureFlag.cpp

${OBJECTDIR}/_ext/8ec51d02/ShaderFeatures.o: ../../src/finjin/engine/ShaderFeatures.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/ShaderFeatures.o ../../src/finjin/engine/ShaderFeatures.cpp

${OBJECTDIR}/_ext/8ec51d02/SoundContextCommonSettings.o: ../../src/finjin/engine/SoundContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/SoundContextCommonSettings.o ../../src/finjin/engine/SoundContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/SoundFormat.o: ../../src/finjin/engine/SoundFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/SoundFormat.o ../../src/finjin/engine/SoundFormat.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentChunkNames.o: ../../src/finjin/engine/StandardAssetDocumentChunkNames.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentChunkNames.o ../../src/finjin/engine/StandardAssetDocumentChunkNames.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyNames.o: ../../src/finjin/engine/StandardAssetDocumentPropertyNames.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyNames.o ../../src/finjin/engine/StandardAssetDocumentPropertyNames.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyValues.o: ../../src/finjin/engine/StandardAssetDocumentPropertyValues.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardAssetDocumentPropertyValues.o ../../src/finjin/engine/StandardAssetDocumentPropertyValues.cpp

${OBJECTDIR}/_ext/8ec51d02/StandardGameControllerInputBindings.o: ../../src/finjin/engine/StandardGameControllerInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StandardGameControllerInputBindings.o ../../src/finjin/engine/StandardGameControllerInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/StringTable.o: ../../src/finjin/engine/StringTable.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/StringTable.o ../../src/finjin/engine/StringTable.cpp

${OBJECTDIR}/_ext/8ec51d02/TextureDimension.o: ../../src/finjin/engine/TextureDimension.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/TextureDimension.o ../../src/finjin/engine/TextureDimension.cpp

${OBJECTDIR}/_ext/8ec51d02/UserDataTypes.o: ../../src/finjin/engine/UserDataTypes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/UserDataTypes.o ../../src/finjin/engine/UserDataTypes.cpp

${OBJECTDIR}/_ext/8ec51d02/VRContextCommonSettings.o: ../../src/finjin/engine/VRContextCommonSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/VRContextCommonSettings.o ../../src/finjin/engine/VRContextCommonSettings.cpp

${OBJECTDIR}/_ext/8ec51d02/VRGameControllerInputBindings.o: ../../src/finjin/engine/VRGameControllerInputBindings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/VRGameControllerInputBindings.o ../../src/finjin/engine/VRGameControllerInputBindings.cpp

${OBJECTDIR}/_ext/8ec51d02/WAVReader.o: ../../src/finjin/engine/WAVReader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WAVReader.o ../../src/finjin/engine/WAVReader.cpp

${OBJECTDIR}/_ext/8ec51d02/WindowBounds.o: ../../src/finjin/engine/WindowBounds.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WindowBounds.o ../../src/finjin/engine/WindowBounds.cpp

${OBJECTDIR}/_ext/8ec51d02/WindowSize.o: ../../src/finjin/engine/WindowSize.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/8ec51d02
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/8ec51d02/WindowSize.o ../../src/finjin/engine/WindowSize.cpp

${OBJECTDIR}/_ext/40bb3adf/Application.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/Application.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/Application.o ../../src/finjin/engine/internal/app/linux/finjin/engine/Application.cpp

${OBJECTDIR}/_ext/40bb3adf/ApplicationGlobals.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/ApplicationGlobals.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/ApplicationGlobals.o ../../src/finjin/engine/internal/app/linux/finjin/engine/ApplicationGlobals.cpp

${OBJECTDIR}/_ext/40bb3adf/DisplayInfo.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/DisplayInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/DisplayInfo.o ../../src/finjin/engine/internal/app/linux/finjin/engine/DisplayInfo.cpp

${OBJECTDIR}/_ext/40bb3adf/FinjinMain.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/FinjinMain.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/FinjinMain.o ../../src/finjin/engine/internal/app/linux/finjin/engine/FinjinMain.cpp

${OBJECTDIR}/_ext/40bb3adf/OSWindow.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/OSWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/OSWindow.o ../../src/finjin/engine/internal/app/linux/finjin/engine/OSWindow.cpp

${OBJECTDIR}/_ext/40bb3adf/PlatformCapabilities.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/PlatformCapabilities.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/PlatformCapabilities.o ../../src/finjin/engine/internal/app/linux/finjin/engine/PlatformCapabilities.cpp

${OBJECTDIR}/_ext/40bb3adf/XcbConnection.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbConnection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/XcbConnection.o ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbConnection.cpp

${OBJECTDIR}/_ext/40bb3adf/XcbWindow.o: ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbWindow.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/40bb3adf
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/40bb3adf/XcbWindow.o ../../src/finjin/engine/internal/app/linux/finjin/engine/XcbWindow.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanDescriptorSetLayout.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanDescriptorSetLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanDescriptorSetLayout.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanDescriptorSetLayout.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanFrameBuffer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanFrameBuffer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameBuffer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanFrameStage.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameStage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanFrameStage.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanFrameStage.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuBuffer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuBuffer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuBuffer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContext.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContext.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContext.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextImpl.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextImpl.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextImpl.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextSettings.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuContextSettings.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuContextSettings.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanGpuDescription.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanGpuDescription.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanGpuDescription.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanIncludes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanIncludes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanIncludes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanIncludes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanInputFormat.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanInputFormat.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanInputFormat.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanInputFormat.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanLayerProperties.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanLayerProperties.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanLayerProperties.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanLayerProperties.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanLight.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanLight.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanLight.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanLight.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMaterial.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMaterial.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMaterial.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMaterial.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMemoryResource.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMemoryResource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMemoryResource.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMemoryResource.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanMesh.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanMesh.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanMesh.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanMesh.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPipeline.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipeline.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPipeline.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipeline.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPipelineLayout.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipelineLayout.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPipelineLayout.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPipelineLayout.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPresentModes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPresentModes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPresentModes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPresentModes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanPrimaryQueues.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanPrimaryQueues.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanPrimaryQueues.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanPrimaryQueues.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanQueueFamilyIndexes.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanQueueFamilyIndexes.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanQueueFamilyIndexes.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanQueueFamilyIndexes.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanRenderTarget.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanRenderTarget.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanRenderTarget.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanRenderTarget.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanResources.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanResources.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanResources.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanResources.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanShader.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanShader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanShader.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanShader.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanStaticMeshRenderer.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanStaticMeshRenderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanStaticMeshRenderer.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanStaticMeshRenderer.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSurfaceFormats.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSurfaceFormats.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSurfaceFormats.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSurfaceFormats.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystem.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystem.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystem.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystemImpl.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystemImpl.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemImpl.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanSystemSettings.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemSettings.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanSystemSettings.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanSystemSettings.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanTexture.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanTexture.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanTexture.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanTexture.cpp

${OBJECTDIR}/_ext/f85409f3/VulkanUtilities.o: ../../src/finjin/engine/internal/gpu/vulkan/VulkanUtilities.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/f85409f3
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/f85409f3/VulkanUtilities.o ../../src/finjin/engine/internal/gpu/vulkan/VulkanUtilities.cpp

${OBJECTDIR}/_ext/32e60500/LinuxGameController.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxGameController.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxGameController.o ../../src/finjin/engine/internal/input/linuxinput/LinuxGameController.cpp

${OBJECTDIR}/_ext/32e60500/LinuxInputContext.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxInputContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxInputContext.o ../../src/finjin/engine/internal/input/linuxinput/LinuxInputContext.cpp

${OBJECTDIR}/_ext/32e60500/LinuxInputSystem.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxInputSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxInputSystem.o ../../src/finjin/engine/internal/input/linuxinput/LinuxInputSystem.cpp

${OBJECTDIR}/_ext/32e60500/LinuxKeyboard.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxKeyboard.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxKeyboard.o ../../src/finjin/engine/internal/input/linuxinput/LinuxKeyboard.cpp

${OBJECTDIR}/_ext/32e60500/LinuxMouse.o: ../../src/finjin/engine/internal/input/linuxinput/LinuxMouse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/32e60500
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/32e60500/LinuxMouse.o ../../src/finjin/engine/internal/input/linuxinput/LinuxMouse.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALAdapterDescription.o: ../../src/finjin/engine/internal/sound/openal/OpenALAdapterDescription.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALAdapterDescription.o ../../src/finjin/engine/internal/sound/openal/OpenALAdapterDescription.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALContext.o: ../../src/finjin/engine/internal/sound/openal/OpenALContext.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALContext.o ../../src/finjin/engine/internal/sound/openal/OpenALContext.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALContextImpl.o: ../../src/finjin/engine/internal/sound/openal/OpenALContextImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALContextImpl.o ../../src/finjin/engine/internal/sound/openal/OpenALContextImpl.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALEffect.o: ../../src/finjin/engine/internal/sound/openal/OpenALEffect.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALEffect.o ../../src/finjin/engine/internal/sound/openal/OpenALEffect.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALEffectSlot.o: ../../src/finjin/engine/internal/sound/openal/OpenALEffectSlot.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALEffectSlot.o ../../src/finjin/engine/internal/sound/openal/OpenALEffectSlot.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALExtensions.o: ../../src/finjin/engine/internal/sound/openal/OpenALExtensions.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALExtensions.o ../../src/finjin/engine/internal/sound/openal/OpenALExtensions.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALFilter.o: ../../src/finjin/engine/internal/sound/openal/OpenALFilter.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALFilter.o ../../src/finjin/engine/internal/sound/openal/OpenALFilter.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALListener.o: ../../src/finjin/engine/internal/sound/openal/OpenALListener.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALListener.o ../../src/finjin/engine/internal/sound/openal/OpenALListener.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSoundBuffer.o: ../../src/finjin/engine/internal/sound/openal/OpenALSoundBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSoundBuffer.o ../../src/finjin/engine/internal/sound/openal/OpenALSoundBuffer.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSoundSource.o: ../../src/finjin/engine/internal/sound/openal/OpenALSoundSource.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSoundSource.o ../../src/finjin/engine/internal/sound/openal/OpenALSoundSource.cpp

${OBJECTDIR}/_ext/b85d39ba/OpenALSystem.o: ../../src/finjin/engine/internal/sound/openal/OpenALSystem.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/b85d39ba
	${RM} "$@.d"
	$(COMPILE.cc) -g -DVK_NO_PROTOTYPES -DVK_USE_PLATFORM_XCB_KHR -D_DEBUG -I/home/build/finjin-3rd-party/cpp/boost -I/home/build/finjin-3rd-party/cpp/eigen -I../../../../../finjin-common/cpp/library/include -I../../src -I../../src/finjin/engine -I../../src/finjin/engine/internal/app/linux -I../../src/finjin/engine/internal -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/b85d39ba/OpenALSystem.o ../../src/finjin/engine/internal/sound/openal/OpenALSystem.cpp

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
