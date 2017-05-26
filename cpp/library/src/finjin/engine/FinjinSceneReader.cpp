//Copyright (c) 2017 Finjin
//
//This file is part of Finjin Engine (finjin-engine).
//
//Finjin Engine is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
//This Source Code Form is subject to the terms of the Mozilla Public
//License, v. 2.0. If a copy of the MPL was not distributed with this
//file, You can obtain one at http://mozilla.org/MPL/2.0/.


//Includes----------------------------------------------------------------------
#include "FinjinPrecompiled.hpp"
#include "finjin/engine/FinjinSceneReader.hpp"
#include "finjin/common/DataChunkReaderCallbacks.hpp"
#include "finjin/engine/StandardAssetDocumentChunkNames.hpp"
#include "finjin/engine/StandardAssetDocumentPropertyValues.hpp"

using namespace Finjin::Engine;


//Macros------------------------------------------------------------------------
#if FINJIN_DEBUG
    #define PRINT_DATA 0
#else
    #define PRINT_DATA 0 //Can still be useful to set this to 1 when testing a release build
#endif

#if PRINT_DATA
    #define PRINT_CHUNK(depth, displayName, isStart) std::cout << Indent(depth - 1) << displayName << " " << (isStart ? "Start" : "End") << "-----------" << std::endl;
    #define PRINT_VALUE(depth, displayName, value) std::cout << Indent(depth) << displayName << ": " << value << std::endl;
    #define PRINT_VALUE2(depth, displayName, v0, v1) std::cout << Indent(depth) << displayName << ": " << v0 << " " << v1 << std::endl;
    #define PRINT_VALUE3(depth, displayName, v0, v1, v2) std::cout << Indent(depth) << displayName << ": " << v0 << " " << v1 << " " << v2 << std::endl;
    #define PRINT_VALUE4(depth, displayName, v0, v1, v2, v3) std::cout << Indent(depth) << displayName << ": " << v0 << " " << v1 << " " << v2 << " " << v3 << std::endl;
    #define PRINT_VALUES(depth, displayName, values)\
        {\
        std::cout << Indent(depth) << displayName << ":";\
        for (auto val : values)\
            std::cout << " " << val;\
        std::cout << std::endl;\
        }
    #define PRINT_VALUES_ITEM(depth, displayName, values, item)\
        {\
        std::cout << Indent(depth) << displayName << ":";\
        for (auto val : values)\
            std::cout << " " << val.item;\
        std::cout << std::endl;\
        }
    #define PRINT_CALLBACKS_COUNT(name, collection) std::cout << name << " callbacks using " << collection.GetMappings().size() << " out of " << collection.GetMappings().max_size() << " mapping slots." << std::endl;
#else
    #define PRINT_CHUNK(depth, displayName, isStart) DoesNothingToAvoidCompilerWarning();
    #define PRINT_VALUE(depth, displayName, value) DoesNothingToAvoidCompilerWarning();
    #define PRINT_VALUE2(depth, displayName, v0, v1) DoesNothingToAvoidCompilerWarning();
    #define PRINT_VALUE3(depth, displayName, v0, v1, v2) DoesNothingToAvoidCompilerWarning();
    #define PRINT_VALUE4(depth, displayName, v0, v1, v2, v3) DoesNothingToAvoidCompilerWarning();
    #define PRINT_VALUES(depth, displayName, values) DoesNothingToAvoidCompilerWarning();
    #define PRINT_VALUES_ITEM(depth, displayName, values, item) DoesNothingToAvoidCompilerWarning();
    #define PRINT_CALLBACKS_COUNT(name, collection) DoesNothingToAvoidCompilerWarning();
#endif

#define CHECK_COLLECTION_NOT_EMPTY(obj, objs, msg) \
    if ((objs).empty()) \
    { \
        FINJIN_SET_ERROR(error, msg); \
        return; \
    } \
    auto& obj = (objs).back();

#define CHECK_SUBSCENES_NOT_EMPTY(subscene, subscenes) CHECK_COLLECTION_NOT_EMPTY(subscene, subscenes, "Unexpectedly encountered a subscene chunk. Subscene collection is empty.");
#define CHECK_SKY_NODES_NOT_EMPTY(sky, skyNodes) CHECK_COLLECTION_NOT_EMPTY(sky, skyNodes, "Unexpectedly encountered a sky node chunk. Sky node collection is empty.");
#define CHECK_SCENE_NODES_NOT_EMPTY(sceneNode, sceneNodes) CHECK_COLLECTION_NOT_EMPTY(sceneNode, sceneNodes, "Unexpectedly encountered a scene node chunk. Scene node collection is empty.");
#define CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, sceneNodes) \
    CHECK_SCENE_NODES_NOT_EMPTY(sceneNode, sceneNodes) \
    if (sceneNode == nullptr) \
    {\
        FINJIN_SET_ERROR(error, "Unable to read scene node (or descendant) properties. Scene node is null."); \
        return; \
    }
#define CHECK_NODE_ANIMATIONS_NOT_EMPTY(animation, animations) CHECK_COLLECTION_NOT_EMPTY(animation, animations, "Unexpectedly encountered a node animation chunk. Node animation collection is empty.");
#define CHECK_NODE_ANIMATION_KEYS_NOT_EMPTY(key, keys) CHECK_COLLECTION_NOT_EMPTY(key, keys, "Unexpectedly encountered a node animation key chunk. Node animation key collection is empty.");
#define CHECK_OBJECTS_NOT_EMPTY(obj, objects) CHECK_COLLECTION_NOT_EMPTY(obj, objects, "Unexpectedly encountered an object chunk. Object collection is empty.");
#define CHECK_SUBENTITIES_NOT_EMPTY(subentity, subentities) CHECK_COLLECTION_NOT_EMPTY(subentity, subentities, "Unexpectedly encountered a subentity chunk. Subentity collection is empty.");
#define CHECK_SUBMESHES_NOT_EMPTY(submesh, submeshes) CHECK_COLLECTION_NOT_EMPTY(submesh, submeshes, "Unexpectedly encountered a submesh chunk. Submesh collection is empty.");
#define CHECK_VERTEX_BUFFER_FORMAT_NOT_EMPTY(element, elements) CHECK_COLLECTION_NOT_EMPTY(element, elements, "Unexpectedly encountered a submesh vertex format element chunk. Submesh vertex format collection is empty.");
#define CHECK_VERTEX_BUFFER_CHANNELS_NOT_EMPTY(channel, channels) CHECK_COLLECTION_NOT_EMPTY(channel, channels, "Unexpectedly encountered a submesh vertex channel chunk. Submesh vertex channel collection is empty.");
#define CHECK_MATERIAL_MAPS_NOT_EMPTY(map, maps) CHECK_COLLECTION_NOT_EMPTY(map, maps, "Unexpectedly encountered a material map chunk. Material map collection is empty.");
#define CHECK_BONE_NOT_EMPTY(bone, bones) CHECK_COLLECTION_NOT_EMPTY(bone, bones, "Unexpectedly encountered a bone chunk. Bone collection is empty.");
#define CHECK_SKELETON_ANIMATION_NOT_EMPTY(animation, animations) CHECK_COLLECTION_NOT_EMPTY(animation, animations, "Unexpectedly encountered a skeleton animation chunk. Skeleton animation collection is empty.");
#define CHECK_MORPH_ANIMATIONS_NOT_EMPTY(animation, animations) CHECK_COLLECTION_NOT_EMPTY(animation, animations, "Unexpectedly encountered a morph animation chunk. Morph animation collection is empty.");
#define CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, subanimations) CHECK_COLLECTION_NOT_EMPTY(subanimation, subanimations, "Unexpectedly encountered a morph animation subanimation chunk. Morph animation subanimation collection is empty.");
#define CHECK_MORPH_ANIMATION_SUBANIMATION_FORMAT_NOT_EMPTY(element, elements) CHECK_COLLECTION_NOT_EMPTY(element, elements, "Unexpectedly encountered a morph subanimation format chunk. Morph subanimation vertex format collection is empty.");
#define CHECK_MORPH_ANIMATION_SUBANIMATION_KEYS_NOT_EMPTY(key, keys) CHECK_COLLECTION_NOT_EMPTY(key, keys, "Unexpectedly encountered a morph subanimation keys chunk. Morph subanimation key collection is empty.");
#define CHECK_MORPH_ANIMATION_SUBANIMATION_KEY_CHANNELS_NOT_EMPTY(channel, channels) CHECK_COLLECTION_NOT_EMPTY(channel, channels, "Unexpectedly encountered a morph subanimation key channels chunk. Morph subanimation key channel collection is empty.");
#define CHECK_POSES_NOT_EMPTY(pose, poses) CHECK_COLLECTION_NOT_EMPTY(pose, poses, "Unexpectedly encountered a poses chunk. Pose collection is empty.");
#define CHECK_SUBPOSES_NOT_EMPTY(subpose, subposes) CHECK_COLLECTION_NOT_EMPTY(subpose, subposes, "Unexpectedly encountered a subposes chunk. Subpose collection is empty.");
#define CHECK_SUBPOSE_FORMAT_NOT_EMPTY(element, elements) CHECK_COLLECTION_NOT_EMPTY(element, elements, "Unexpectedly encountered a subpose format element chunk. Subpose format collection is empty.");
#define CHECK_SUBPOSE_CHANNELS_NOT_EMPTY(channel, channels) CHECK_COLLECTION_NOT_EMPTY(channel, channels, "Unexpectedly encountered a subpose channel chunk. Subpose channel collection is empty.");
#define CHECK_POSE_ANIMATIONS_NOT_EMPTY(animation, animations) CHECK_COLLECTION_NOT_EMPTY(animation, animations, "Unexpectedly encountered a pose animation chunk. Pose animation collection is empty.");
#define CHECK_POSE_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, subanimations) CHECK_COLLECTION_NOT_EMPTY(subanimation, subanimations, "Unexpectedly encountered a pose animation subanimation chunk. Pose animation subanimation collection is empty.");
#define CHECK_POSE_ANIMATION_SUBANIMATION_KEY_NOT_EMPTY(key, keys) CHECK_COLLECTION_NOT_EMPTY(key, keys, "Unexpectedly encountered a pose subanimation keys chunk. Pose subanimation key collection is empty.");
#define CHECK_NOTE_TRACKS_NOT_EMPTY(noteTrack, noteTracks) CHECK_COLLECTION_NOT_EMPTY(noteTrack, noteTracks, "Unexpectedly encountered a note track chunk. Note track collection is empty.");
#define CHECK_NOTE_TRACK_KEYS_NOT_EMPTY(key, keys) CHECK_COLLECTION_NOT_EMPTY(key, keys, "Unexpectedly encountered a note keys chunk. Note key collection is empty.");
#define CHECK_MANUAL_LODS_NOT_EMPTY(manualLod, manualLods) CHECK_COLLECTION_NOT_EMPTY(manualLod, manualLods, "Unexpectedly encountered a manual LODs chunk. Manual LOD collection is empty.");
#define CHECK_PREFABS_NOT_EMPTY(prefab, prefabs) CHECK_COLLECTION_NOT_EMPTY(prefab, prefabs, "Unexpectedly encountered a prefab chunk. Prefab collection is empty.");
#define CHECK_MATERIALS_NOT_EMPTY(material, materials) CHECK_COLLECTION_NOT_EMPTY(material, materials, "Unexpectedly encountered a material chunk. Material collection is empty.");
#define CHECK_TEXTURES_NOT_EMPTY(texture, textures) CHECK_COLLECTION_NOT_EMPTY(texture, textures, "Unexpectedly encountered a texture chunk. Texture collection is empty.");
#define CHECK_MESHES_NOT_EMPTY(mesh, meshes) CHECK_COLLECTION_NOT_EMPTY(mesh, meshes, "Unexpectedly encountered a mesh chunk. Mesh collection is empty.");

#define CHECK_OBJECT_OF_TYPE(typedObj, obj, class) \
    if (obj == nullptr) \
    { \
        FINJIN_SET_ERROR(error, "Unable to read object properties. Object is null."); \
        return; \
    } \
    else if (!obj->IsTypeOf(FINJIN_TYPE_DESCRIPTION(class))) \
    { \
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to read object properties. Object is of type '%1%' but needs to be '%2%'.", obj->GetTypeDescription().GetName(), FINJIN_TYPE_DESCRIPTION(class).GetName())); \
        return; \
    } \
    auto typedObj = static_cast<class*>(obj);

#define SORT_USER_DATA_ENUMS_AND_PROPERTIES 0 //Enabling this is helpful for debugging in some scenarios but isn't really necessary in general


//Local types-------------------------------------------------------------------
template <typename T, typename State>
struct NonEmbeddedAssetHandleChunk
{
    void operator () (bool isStart, DataChunkReader& reader, State& state, Error& error) const
    {
        FINJIN_ERROR_METHOD_START(error);

        if (isStart)
        {
            state.template CreateNewAssetHandle<T>(error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
        }
        else
            state.assetHandle = nullptr;
    }
};

template <typename T, typename State>
struct NonEmbeddedAssetGetData
{
    void operator () (AssetHandle<T>*& result, State& state, Error& error) const
    {
        FINJIN_ERROR_METHOD_START(error);

        if (state.assetHandle == nullptr)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unexpectedly encountered a %1% property - the %1% is null.", AssetClassUtilities::ToString(TypeToAssetInfo<T>::GetAssetClass())));
            return;
        }

        result = state.assetHandle;
    }
};


//Local functions---------------------------------------------------------------
#if PRINT_DATA
static Utf8String Indent(size_t depth)
{
    return Utf8String(depth, ' ');
}

static void PRINT_TRANSFORM(size_t depth, const MathMatrix4& value)
{
    std::cout << Indent(depth) << "transform: " << value(0, 0) << " " << value(0, 1) << " " << value(0, 2) << " " << value(0, 3) << std::endl
        << Indent(depth) << value(1, 0) << " " << value(1, 1) << " " << value(1, 2) << " " << value(1, 3) << std::endl
        << Indent(depth) << value(2, 0) << " " << value(2, 1) << " " << value(2, 2) << " " << value(2, 3) << std::endl
        << Indent(depth) << value(3, 0) << " " << value(3, 1) << " " << value(3, 2) << " " << value(3, 3) << std::endl
        ;
}
#else
inline void PRINT_TRANSFORM(size_t depth, const MathMatrix4& value)
{
}
#endif

template <typename State>
bool ReadStringWithLengthHint(DataChunkReader& reader, DataHeader& dataHeader, State& state, Utf8String& value, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (state.propertyLengthHint > 0)
    {
        if (value.reserve(state.propertyLengthHint).HasError())
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to reserve '%1%' characters for hinted string.", state.propertyLengthHint));

            state.propertyLengthHint = 0;

            return false;
        }

        state.propertyLengthHint = 0;
    }

    reader.ReadString(dataHeader, value, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to read hinted string.");
        return false;
    }

    return dataHeader.IsOnlyOrLastOccurrence();
}

template <typename Settings, typename State>
void PrepareToReadChannelValues
    (
    DynamicVector<FinjinVertexElementChannel>& channels,
    size_t vertexCount,
    DynamicVector<FinjinVertexElementFormat>& formatElements,
    Settings& settings,
    State& state,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    auto channelIndex = channels.size();
    channels.push_back();
    auto& channel = channels.back();

    if (vertexCount == 0)
    {
        FINJIN_SET_ERROR(error, "Vertex count is 0. Unable to allocate vertex element channel.");
        return;
    }

    if (channelIndex >= formatElements.size())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Channel index '%1%' exceeds the number of format elements '%2%'", channelIndex, formatElements.size()));
        return;
    }

    auto& formatElement = formatElements[channelIndex];

    auto formatElementValueCount = NumericStructElementTypeUtilities::GetSimpleTypeSizeInElements(formatElement.type);
    if (formatElementValueCount == 0)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Channel index '%1%' has a zero size element type '%2%'", channelIndex, NumericStructElementTypeUtilities::ToString(formatElement.type)));
        return;
    }

    if (NumericStructElementTypeUtilities::IsFloat(formatElement.type))
    {
        if (!channel.floatValues.Create(vertexCount * formatElementValueCount, state.allocator))
            FINJIN_SET_ERROR(error, "Failed to allocate channel float values.");
    }
    else if (NumericStructElementTypeUtilities::IsInt32(formatElement.type))
    {
        if (!channel.int32Values.Create(vertexCount * formatElementValueCount, state.allocator))
            FINJIN_SET_ERROR(error, "Failed to allocate channel int values.");
    }
    else
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Channel index '%1%' has a non-simple element type '%2%'", channelIndex, NumericStructElementTypeUtilities::ToString(formatElement.type)));
    }
}

template <typename State, typename Settings>
void ReadChannelValues
    (
    FinjinVertexElementChannel& channel,
    DataChunkReader& reader,
    DataHeader& dataHeader,
    State& state,
    Settings& settings,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    if (dataHeader.IsOnlyOrFirstOccurrence())
        state.bufferValuesRead = 0;

    if (!channel.floatValues.empty())
    {
        state.bufferValuesRead += reader.ReadFloats(dataHeader, channel.floatValues.data() + state.bufferValuesRead, channel.floatValues.size() - state.bufferValuesRead, error);
        if (error)
            FINJIN_SET_ERROR(error, "Failed to read channel float values.");
        else
        {
            if (state.bufferValuesRead == channel.floatValues.size())
                PRINT_VALUES(state.GetDepth(), "channel values", channel.floatValues);
        }
    }
    else if (!channel.int32Values.empty())
    {
        state.bufferValuesRead += reader.ReadInt32s(dataHeader, channel.int32Values.data() + state.bufferValuesRead, channel.int32Values.size() - state.bufferValuesRead, error);
        if (error)
            FINJIN_SET_ERROR(error, "Failed to read channel int32 values.");
    }
}

template <typename State, typename Callbacks>
static void ReadFormatElement
    (
    FinjinVertexElementFormat& formatElement,
    ParsedChunkPropertyName& propertyName,
    DataChunkReader& reader,
    DataHeader& dataHeader,
    Callbacks* callbacks,
    State& state,
    Error& error
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //auto& settings = callbacks->GetSettings();

    if (propertyName == StandardAssetDocumentPropertyNames::ID)
    {
        if (dataHeader.IsOnlyOrFirstOccurrence())
            callbacks->tempString.clear();
        auto& id = callbacks->tempString;
        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, id, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::ID.name));
        else if (isValueDone)
        {
            GpuInputFormatStructMetadata::ParseElementID(formatElement.id, id, error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
            else
                PRINT_VALUE(state.GetDepth(), "id", id);
        }
    }
    else if (propertyName == StandardAssetDocumentPropertyNames::TYPE)
    {
        if (dataHeader.IsOnlyOrFirstOccurrence())
            callbacks->tempString.clear();
        auto& type = callbacks->tempString;
        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, type, error);
        if (error)
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TYPE.name));
        else if (isValueDone)
        {
            NumericStructElementTypeUtilities::ParseSimpleType(formatElement.type, type, error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
            else
                PRINT_VALUE(state.GetDepth(), "type", type);
        }
    }
}

template <typename State, typename Callbacks, typename GetData, typename... BasePattern>
void AddUserDataMappings
    (
    Callbacks* callbacks,
    GetData getData, //std::function<void(UserDataClassInstance*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern/[user-data]
    DataChunkReaderCallbacksChunkMapping<State> mapping;
    if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
    {
        FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
        return;
    }
    if (mapping.pattern.Add(StandardAssetDocumentChunkNames::USER_DATA).HasErrorOrValue(false))
    {
        FINJIN_SET_ERROR(error, "Failed to add user data pattern to mapping patterns.");
        return;
    }
    mapping.chunkCallback = [callbacks](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        PRINT_CHUNK(state.GetDepth(), "USER_DATA", isStart);
    };
    mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
    {
        FINJIN_ERROR_METHOD_START(error);

        UserDataClassInstance* userData = nullptr;
        getData(userData, state, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to get user data.");
            return;
        }

        //auto& settings = callbacks->GetSettings();
        auto& userDataClassPropertiesLookup = callbacks->GetUserDataClassPropertiesLookup();

        if (propertyName == StandardAssetDocumentPropertyNames::USER_DATA_STRING_PROPERTY_COUNT)
        {
            reader.ReadCount(dataHeader, state.userDataStringPropertyCount, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::USER_DATA_STRING_PROPERTY_COUNT.name));
                return;
            }

            PRINT_VALUE(state.GetDepth(), "user data string property property count", state.userDataStringPropertyCount);
        }
        else if (propertyName == StandardAssetDocumentPropertyNames::USER_DATA_CLASS_NAME)
        {
            auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, userData->userDataClassName, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::USER_DATA_CLASS_NAME.name));
                return;
            }
            else if (isValueDone && state.userDataStringPropertyCount == 0)
            {
                userData->userDataClass = state.GetUserDataClass(userData->userDataClassName);
                if (userData->userDataClass == nullptr)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get user data class '%1%'.", userData->userDataClassName));
                    return;
                }

                userData->userDataClass->GetProperties(userDataClassPropertiesLookup, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to get properties for data class '%1%'.", userData->userDataClassName));
                    return;
                }

                if (!userDataClassPropertiesLookup.empty())
                {
                    if (!userData->propertyInstances.CreateEmpty(userDataClassPropertiesLookup.size(), state.allocator))
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' user data property instances.", userDataClassPropertiesLookup.size()));
                        return;
                    }

                    for (auto prop : userDataClassPropertiesLookup)
                    {
                        switch (prop->type.type)
                        {
                            case UserDataPrimitiveType::STRING_DATA_TYPE:
                            {
                                auto propertyInstance = AllocatedClass::New<UserDataClassInstance::StringPropertyInstance>(state.allocator, FINJIN_CALLER_ARGUMENTS, prop);
                                if (propertyInstance == nullptr)
                                {
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate string property instance '%1%'.", prop->name));
                                    return;
                                }

                                userData->propertyInstances.push_back(propertyInstance);

                                break;
                            }
                            case UserDataPrimitiveType::INT_DATA_TYPE:
                            {
                                auto propertyInstance = AllocatedClass::New<UserDataClassInstance::IntPropertyInstance>(state.allocator, FINJIN_CALLER_ARGUMENTS, prop);
                                if (propertyInstance == nullptr)
                                {
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate int property instance '%1%'.", prop->name));
                                    return;
                                }

                                userData->propertyInstances.push_back(propertyInstance);

                                break;
                            }
                            case UserDataPrimitiveType::FLOAT_DATA_TYPE:
                            {
                                auto propertyInstance = AllocatedClass::New<UserDataClassInstance::FloatPropertyInstance>(state.allocator, FINJIN_CALLER_ARGUMENTS, prop);
                                if (propertyInstance == nullptr)
                                {
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate float property instance '%1%'.", prop->name));
                                    return;
                                }

                                userData->propertyInstances.push_back(propertyInstance);

                                break;
                            }
                            case UserDataPrimitiveType::BOOL_DATA_TYPE:
                            {
                                auto propertyInstance = AllocatedClass::New<UserDataClassInstance::BoolPropertyInstance>(state.allocator, FINJIN_CALLER_ARGUMENTS, prop);
                                if (propertyInstance == nullptr)
                                {
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate boolean property instance '%1%'.", prop->name));
                                    return;
                                }

                                userData->propertyInstances.push_back(propertyInstance);

                                break;
                            }
                            case UserDataPrimitiveType::RGB_DATA_TYPE:
                            {
                                auto propertyInstance = AllocatedClass::New<UserDataClassInstance::RGBAPropertyInstance>(state.allocator, FINJIN_CALLER_ARGUMENTS, prop);
                                if (propertyInstance == nullptr)
                                {
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate RGB property instance '%1%'.", prop->name));
                                    return;
                                }

                                userData->propertyInstances.push_back(propertyInstance);

                                break;
                            }
                            case UserDataPrimitiveType::RGBA_DATA_TYPE:
                            {
                                auto propertyInstance = AllocatedClass::New<UserDataClassInstance::RGBAPropertyInstance>(state.allocator, FINJIN_CALLER_ARGUMENTS, prop);
                                if (propertyInstance == nullptr)
                                {
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate RGBA property instance '%1%'.", prop->name));
                                    return;
                                }

                                userData->propertyInstances.push_back(propertyInstance);

                                break;
                            }
                        }
                    }
                }
            }
            else if (isValueDone && state.userDataStringPropertyCount > 0)
            {
                if (!userData->propertyInstances.CreateEmpty(state.userDataStringPropertyCount, state.allocator))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate '%1%' user data property instances.", state.userDataStringPropertyCount));
                    return;
                }
            }
        }
        else
        {
            if (userData->userDataClass != nullptr)
            {
                //It's a property instance to be updated
                auto userDatapropertyInstanceIter = std::find_if(userData->propertyInstances.begin(), userData->propertyInstances.end(), [&propertyName](UserDataClassInstance::PropertyInstance* userDatapropertyInstance)
                {
                    return propertyName.id == userDatapropertyInstance->chunkPropertyNameID;
                });
                if (userDatapropertyInstanceIter != userData->propertyInstances.end())
                {
                    auto userDatapropertyInstance = *userDatapropertyInstanceIter;
                    switch (userDatapropertyInstance->userDataProperty->type.type)
                    {
                        case UserDataPrimitiveType::STRING_DATA_TYPE:
                        {
                            auto propertyInstance = static_cast<UserDataClassInstance::StringPropertyInstance*>(userDatapropertyInstance);

                            auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, propertyInstance->value, error);
                            if (error)
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", userDatapropertyInstance->userDataProperty->name));
                                return;
                            }
                            else if (isValueDone)
                                PRINT_VALUE(state.GetDepth(), userDatapropertyInstance->userDataProperty->name, propertyInstance->value);

                            break;
                        }
                        case UserDataPrimitiveType::INT_DATA_TYPE:
                        {
                            auto propertyInstance = static_cast<UserDataClassInstance::IntPropertyInstance*>(userDatapropertyInstance);
                            reader.ReadInt32(dataHeader, propertyInstance->value, error);
                            if (error)
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read int32 property '%1%'.", userDatapropertyInstance->userDataProperty->name));
                                return;
                            }

                            PRINT_VALUE(state.GetDepth(), userDatapropertyInstance->userDataProperty->name, propertyInstance->value);

                            break;
                        }
                        case UserDataPrimitiveType::FLOAT_DATA_TYPE:
                        {
                            auto propertyInstance = static_cast<UserDataClassInstance::FloatPropertyInstance*>(userDatapropertyInstance);
                            reader.ReadFloat(dataHeader, propertyInstance->value, error);
                            if (error)
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", userDatapropertyInstance->userDataProperty->name));
                                return;
                            }

                            PRINT_VALUE(state.GetDepth(), userDatapropertyInstance->userDataProperty->name, propertyInstance->value);

                            break;
                        }
                        case UserDataPrimitiveType::BOOL_DATA_TYPE:
                        {
                            auto propertyInstance = static_cast<UserDataClassInstance::BoolPropertyInstance*>(userDatapropertyInstance);
                            reader.ReadBool(dataHeader, propertyInstance->value, error);
                            if (error)
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read bool property '%1%'.", userDatapropertyInstance->userDataProperty->name));
                                return;
                            }

                            PRINT_VALUE(state.GetDepth(), userDatapropertyInstance->userDataProperty->name, (int)propertyInstance->value);

                            break;
                        }
                        case UserDataPrimitiveType::RGB_DATA_TYPE:
                        {
                            auto propertyInstance = static_cast<UserDataClassInstance::RGBAPropertyInstance*>(userDatapropertyInstance);
                            auto isValueDone = reader.ReadFloats(dataHeader, propertyInstance->value.data(), 3, error) == 3;
                            if (error)
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read RGB property '%1%'.", userDatapropertyInstance->userDataProperty->name));
                                return;
                            }
                            else if (!isValueDone)
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", userDatapropertyInstance->userDataProperty->name));

                            PRINT_VALUE3(state.GetDepth(), userDatapropertyInstance->userDataProperty->name, propertyInstance->value[0], propertyInstance->value[1], propertyInstance->value[2]);

                            break;
                        }
                        case UserDataPrimitiveType::RGBA_DATA_TYPE:
                        {
                            auto propertyInstance = static_cast<UserDataClassInstance::RGBAPropertyInstance*>(userDatapropertyInstance);
                            auto isValueDone = reader.ReadFloats(dataHeader, propertyInstance->value.data(), 4, error) == 4;
                            if (error)
                            {
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read RGBA property '%1%'.", userDatapropertyInstance->userDataProperty->name));
                                return;
                            }
                            else if (!isValueDone)
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", userDatapropertyInstance->userDataProperty->name));

                            PRINT_VALUE4(state.GetDepth(), userDatapropertyInstance->userDataProperty->name, propertyInstance->value[0], propertyInstance->value[1], propertyInstance->value[2], propertyInstance->value[3]);

                            break;
                        }
                    }
                }
            }
            else
            {
                //It's a property instance to be updated or added

                //If there is an existing instance, it will be the last one in the collection
                UserDataClassInstance::StringPropertyInstance* propertyInstance = nullptr;
                if (!userData->propertyInstances.empty() && userData->propertyInstances.back()->chunkPropertyNameID == propertyName.id)
                {
                    //Existing instance found
                    propertyInstance = static_cast<UserDataClassInstance::StringPropertyInstance*>(userData->propertyInstances.back());
                }
                else
                {
                    //Existing instance not found. Create new one
                    propertyInstance = AllocatedClass::New<UserDataClassInstance::StringPropertyInstance>(state.allocator, FINJIN_CALLER_ARGUMENTS, nullptr);
                    if (propertyInstance == nullptr)
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to allocate string property instance '%1%'.", propertyName.ToString()));
                        return;
                    }

                    propertyInstance->chunkPropertyNameID = propertyName.id;

                    if (userData->propertyInstances.push_back(propertyInstance).HasErrorOrValue(false))
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add string property instance '%1%'.", propertyName.ToString()));
                        return;
                    }
                }

                //Read/update the string value
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, propertyInstance->value, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", propertyName.ToString()));
                    return;
                }
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), Convert::ToString(propertyInstance->chunkPropertyNameID), propertyInstance->value);
            }
        }
    };
    callbacks->AddMapping(mapping, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for user data.");
        return;
    }
}

template <typename State, typename Callbacks, typename GetData, typename... BasePattern>
void AddNoteTracksMappings
    (
    Callbacks* callbacks,
    GetData getData, //std::function<void(DynamicVector<FinjinSceneObjectBase::NoteTrack>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern/[note-tracks]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::NOTE_TRACKS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add note tracks pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "NOTE TRACKS", isStart);
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneObjectBase::NoteTrack>* noteTracks = nullptr;
            getData(noteTracks, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get note tracks.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!noteTracks->CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate note tracks.");
                    else
                        PRINT_VALUE(state.GetDepth(), "note track count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for note tracks.");
            return;
        }
    }

    //basePattern/[note-tracks]/[note-track-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::NOTE_TRACKS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add note track pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "NOTE TRACK", isStart);

            if (isStart)
            {
                DynamicVector<FinjinSceneObjectBase::NoteTrack>* noteTracks = nullptr;
                getData(noteTracks, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get note tracks.");
                    return;
                }
                noteTracks->push_back();
            }
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneObjectBase::NoteTrack>* noteTracks = nullptr;
            getData(noteTracks, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get note tracks.");
                return;
            }

            CHECK_NOTE_TRACKS_NOT_EMPTY(noteTrack, *noteTracks)

            if (propertyName == StandardAssetDocumentPropertyNames::NAME)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, noteTrack.name, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "name", noteTrack.name);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for note track.");
            return;
        }
    }

    //basePattern/[note-tracks]/[note-track-index]/[keys]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::NOTE_TRACKS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add note track keys pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "NOTE TRACK KEYS", isStart);
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneObjectBase::NoteTrack>* noteTracks = nullptr;
            getData(noteTracks, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get note tracks.");
                return;
            }

            CHECK_NOTE_TRACKS_NOT_EMPTY(noteTrack, *noteTracks)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!noteTrack.keys.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate note track keys.");
                    else
                        PRINT_VALUE(state.GetDepth(), "note track key count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for note track keys.");
            return;
        }
    }

    //basePattern/[note-tracks]/[note-track-index]/[keys]/[key-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::NOTE_TRACKS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add note track key pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "NOTE TRACK KEY", isStart);

            if (isStart)
            {
                DynamicVector<FinjinSceneObjectBase::NoteTrack>* noteTracks = nullptr;
                getData(noteTracks, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get note tracks.");
                    return;
                }

                CHECK_NOTE_TRACKS_NOT_EMPTY(noteTrack, *noteTracks)
                noteTrack.keys.push_back();
            }
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneObjectBase::NoteTrack>* noteTracks = nullptr;
            getData(noteTracks, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get note tracks.");
                return;
            }

            CHECK_NOTE_TRACKS_NOT_EMPTY(noteTrack, *noteTracks)
            CHECK_NOTE_TRACK_KEYS_NOT_EMPTY(key, noteTrack.keys)

            if (propertyName == StandardAssetDocumentPropertyNames::TIME)
            {
                reader.ReadTimeDuration(dataHeader, key.time, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read time duration property '%1%'.", StandardAssetDocumentPropertyNames::TIME.name));
                else
                    PRINT_VALUE(state.GetDepth(), "time", key.time.ToString());
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::CONTENT)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, key.content, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::CONTENT.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "content", key.content);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for note track key.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename GetData, typename... BasePattern>
void AddFlagsMappings
    (
    Callbacks* callbacks,
    GetData getData, //std::function<void(DynamicVector<uint8_t>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern/[flags]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::FLAGS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add flags pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "FLAGS", isStart);
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<uint8_t>* flags = nullptr;
            getData(flags, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get flags.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!flags->Create(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate object flags.");
                    else
                        PRINT_VALUE(state.GetDepth(), "object flag count", value);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::VALUES)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                state.bufferValuesRead += reader.ReadUInt8s(dataHeader, flags->data() + state.bufferValuesRead, flags->size() - state.bufferValuesRead, error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read flags uint8 values.");
                else
                    PRINT_VALUES(state.GetDepth(), "flag 8bit values", *flags);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for flags.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddNodeAnimationMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(AssetHandle<FinjinNodeAnimation>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "NODE ANIMATION", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinNodeAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get node animation animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::NODE_ANIMATION_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& animationRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, animationRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NODE_ANIMATION_REF.name));
                else if (isValueDone)
                {
                    animationHandle->assetRef.ForUriString(animationRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse node animation reference '%1%'.", animationRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "node animation ref", animationRef);

                        state.RecordAssetHandle(animationHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record node animation asset.");
                    }
                }
            }
            else
            {
                if (animation == nullptr)
                {
                    animationHandle->isOwner = true;

                    animation = AllocatedClass::New<FinjinNodeAnimation>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                    if (animation == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to allocate node animation.");
                        return;
                    }
                }

                if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                {
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, animation->name, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                    else if (isValueDone)
                    {
                        PRINT_VALUE(state.GetDepth(), "animation name", animation->name);

                        animationHandle->assetRef.objectName = animation->name;

                        state.RecordAssetHandle(animationHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record node animation asset.");
                    }
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::ENABLE)
                {
                    reader.ReadBool(dataHeader, animation->enable, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read boolean property '%1%'.", StandardAssetDocumentPropertyNames::ENABLE.name));
                    else
                        PRINT_VALUE(state.GetDepth(), "animation enable", (int)animation->enable);
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::LOOP)
                {
                    reader.ReadBool(dataHeader, animation->loop, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read boolean property '%1%'.", StandardAssetDocumentPropertyNames::LOOP.name));
                    else
                        PRINT_VALUE(state.GetDepth(), "animation loop", (int)animation->loop);
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::LENGTH)
                {
                    reader.ReadTimeDuration(dataHeader, animation->length, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read time duration property '%1%'.", StandardAssetDocumentPropertyNames::LENGTH.name));
                    else
                        PRINT_VALUE(state.GetDepth(), "animation time duration", animation->length.ToString());
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for node animation.");
            return;
        }
    }

    //basePattern/[node-animation-keys]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::KEYS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add node animation keys pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "NODE ANIMATION KEYS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinNodeAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get node animation animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!animation->keys.Create(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate node animation keys.");
                    else
                        PRINT_VALUE(state.GetDepth(), "node animation keys count", value);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TIME)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                auto& key = animation->keys[state.bufferValuesRead];

                state.bufferValuesRead += reader.ReadStridedTimeDurations(dataHeader, &key.time, animation->keys.size() - state.bufferValuesRead, DataChunkReadStride(1, sizeof(FinjinNodeAnimation::Key)), error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read node animation key times.");
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TRANSFORM)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                auto isValueDone = false;
                auto offsetInKeyValue = state.bufferValuesRead % 16;
                if (offsetInKeyValue != 0)
                {
                    auto& key = animation->keys[state.bufferValuesRead / 16];

                    state.bufferValuesRead += reader.ReadFloats(dataHeader, key.m44.data() + offsetInKeyValue, 16 - offsetInKeyValue, error);
                    if (error)
                        FINJIN_SET_ERROR(error, "Failed to read node animation key transforms.");
                    else
                    {
                        if (state.bufferValuesRead == animation->keys.size() * 16)
                            isValueDone = true;
                    }
                }
                if (!isValueDone && !error)
                {
                    auto& key = animation->keys[state.bufferValuesRead / 16];

                    state.bufferValuesRead += reader.ReadStridedFloats(dataHeader, key.m44.data(), animation->keys.size() * 16 - state.bufferValuesRead, DataChunkReadStride(16, sizeof(FinjinNodeAnimation::Key)), error);
                    if (error)
                        FINJIN_SET_ERROR(error, "Failed to read node animation key transforms.");
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for node animation keys.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename GetData, typename... BasePattern>
void AddNodeAnimationsMappings
    (
    Callbacks* callbacks,
    GetData getData, //std::function<void(DynamicVector<AssetHandle<FinjinNodeAnimation> >*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern/[node-animations]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::NODE_ANIMATIONS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add node animations pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "NODE ANIMATIONS", isStart);
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<AssetHandle<FinjinNodeAnimation> >* nodeAnimationHandles = nullptr;
            getData(nodeAnimationHandles, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get node animations.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!nodeAnimationHandles->CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate node animations.");
                    else
                        PRINT_VALUE(state.GetDepth(), "node animation count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for node animations.");
            return;
        }
    }

    //basePattern/[node-animations]/[node-animation-index]
    AddNodeAnimationMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (isStart)
            {
                DynamicVector<AssetHandle<FinjinNodeAnimation> >* nodeAnimationHandles = nullptr;
                getData(nodeAnimationHandles, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get node animations.");
                    return;
                }

                nodeAnimationHandles->push_back();
            }
        },
        [getData, callbacks](AssetHandle<FinjinNodeAnimation>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<AssetHandle<FinjinNodeAnimation> >* nodeAnimationHandles = nullptr;
            getData(nodeAnimationHandles, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get node animations.");
                return;
            }

            result = &nodeAnimationHandles->back();
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::NODE_ANIMATIONS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for node animations.");
        return;
    }
}

template <typename State, typename Callbacks, typename GetData, typename... BasePattern>
void AddSceneNodesMappings
    (
    Callbacks* callbacks,
    GetData getData, //std::function<void(DynamicVector<FinjinSceneNode*>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern/[scene-nodes]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE_NODES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene nodes pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SCENE NODES", isStart);
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!sceneNodes->CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate scene nodes.");
                    else
                        PRINT_VALUE(state.GetDepth(), "scene node count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for scene nodes.");
            return;
        }
    }

    //basePattern/[scene-nodes]/[scene-node-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene node pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            if (isStart)
                sceneNodes->push_back(nullptr);
            else
            {
                if (sceneNodes->back() == nullptr)
                    FINJIN_SET_ERROR(error, "Failed to instantiate scene node at the end of a scene node chunk.");
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY(sceneNode, *sceneNodes)

            if (propertyName == StandardAssetDocumentPropertyNames::TYPE)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& typeName = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, typeName, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TYPE.name));
                else if (isValueDone)
                {
                    if (callbacks->GetSettings().TranslateType(typeName).HasError())
                        FINJIN_SET_ERROR(error, "Failed to translate type.");
                    else
                    {
                        sceneNode = CreateInstanceOf<FinjinSceneNode>(typeName, state.allocator, FINJIN_CALLER_ARGUMENTS, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create scene node of type '%1%'.", typeName));
                        else
                            PRINT_VALUE(state.GetDepth(), "type", typeName);
                    }
                }
            }
            else
            {
                if (sceneNode == nullptr)
                    FINJIN_SET_ERROR(error, "Unable to read scene node properties. Scene node is null.");
                else if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                {
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, sceneNode->name, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                    else if (isValueDone)
                        PRINT_VALUE(state.GetDepth(), "name", sceneNode->name);
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::ID)
                {
                    reader.ReadUuid(dataHeader, sceneNode->id, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read UUID property '%1%'.", StandardAssetDocumentPropertyNames::ID.name));
                    else
                        PRINT_VALUE(state.GetDepth(), "id", sceneNode->id);
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::VISIBILITY)
                {
                    if (dataHeader.IsOnlyOrFirstOccurrence())
                        callbacks->tempString.clear();
                    auto& visibility = callbacks->tempString;
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, visibility, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::VISIBILITY.name));
                    else if (isValueDone)
                    {
                        if (visibility == StandardAssetDocumentPropertyValues::NodeVisibility::VISIBLE)
                            sceneNode->visibility = FinjinSceneNodeVisibility::VISIBLE;
                        else if (visibility == StandardAssetDocumentPropertyValues::NodeVisibility::HIDDEN)
                            sceneNode->visibility = FinjinSceneNodeVisibility::HIDDEN;
                        else if (visibility == StandardAssetDocumentPropertyValues::NodeVisibility::TREE_VISIBLE)
                            sceneNode->visibility = FinjinSceneNodeVisibility::TREE_VISIBLE;
                        else if (visibility == StandardAssetDocumentPropertyValues::NodeVisibility::TREE_HIDDEN)
                            sceneNode->visibility = FinjinSceneNodeVisibility::TREE_HIDDEN;

                        PRINT_VALUE(state.GetDepth(), "visibility", visibility);
                    }
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::PREFAB_REF)
                {
                    if (dataHeader.IsOnlyOrFirstOccurrence())
                        callbacks->tempString.clear();
                    auto& prefabRef = callbacks->tempString;
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, prefabRef, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::PREFAB_REF.name));
                    else if (isValueDone)
                    {
                        sceneNode->prefabHandle.assetRef.ForUriString(prefabRef, callbacks->tempSimpleUri, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse prefab reference '%1%'.", prefabRef));
                        else
                        {
                            PRINT_VALUE(state.GetDepth(), "prefab ref", prefabRef);

                            state.RecordAssetHandle(&sceneNode->prefabHandle, error);
                            if (error)
                                FINJIN_SET_ERROR(error, "Failed to prefab asset.");
                        }
                    }
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::TRANSFORM)
                {
                    auto isValueDone = reader.ReadFloats(dataHeader, sceneNode->transform.data(), 16, error) == 16;
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::TRANSFORM.name));
                    else if (!isValueDone)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::TRANSFORM.name));
                    else
                        PRINT_TRANSFORM(state.GetDepth(), sceneNode->transform);
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::PARENT)
                {
                    size_t value;
                    reader.ReadCount(dataHeader, value, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::PARENT.name));
                    else if (value >= sceneNodes->size())
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid parent index '%1%'. Must be between [0-%2%].", value, sceneNodes->size() - 1));
                    else
                    {
                        sceneNode->parentNodeIndex = value;
                        sceneNode->parentNodePointer = (*sceneNodes)[value];
                        if (sceneNode->parentNodePointer->childNodePointers.push_back(sceneNode).HasErrorOrValue(false))
                            FINJIN_SET_ERROR(error, "Failed to add child node pointer to parent node.");
                        else
                            PRINT_VALUE(state.GetDepth(), "parent index", value);
                    }
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::CHILD_COUNT)
                {
                    size_t value;
                    reader.ReadCount(dataHeader, value, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::CHILD_COUNT.name));
                    else
                    {
                        if (!sceneNode->childNodePointers.CreateEmpty(value, state.allocator))
                            FINJIN_SET_ERROR(error, "Failed to allocate child pointers.");
                        else
                            PRINT_VALUE(state.GetDepth(), "child count", value);
                    }
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for scene node.");
            return;
        }
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[user-data]
    AddUserDataMappings<State>
        (
        callbacks,
        [getData](UserDataClassInstance*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)

            result = &sceneNode->userData;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for scene node user data.");
        return;
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[note-tracks]
    AddNoteTracksMappings<State>
        (
        callbacks,
        [getData](DynamicVector<FinjinSceneObjectBase::NoteTrack>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)

            result = &sceneNode->noteTracks;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for scene node note tracks.");
        return;
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[node-animations]
    AddNodeAnimationsMappings<State>
        (
        callbacks,
        [getData](DynamicVector<AssetHandle<FinjinNodeAnimation> >*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)

            result = &sceneNode->nodeAnimationHandles;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for scene node animations.");
        return;
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[objects]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::OBJECTS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene node objects pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SCENE NODE OBJECTS", isStart);
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!sceneNode->objects.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate scene node objects collection.");
                    else
                        PRINT_VALUE(state.GetDepth(), "scene node objects count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for scene node objects.");
            return;
        }
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[objects]/[object-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::OBJECTS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene node object pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SCENE NODE OBJECT", isStart);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)

            if (isStart)
                sceneNode->objects.push_back(nullptr);
            else
            {
                if (sceneNode->objects.back() == nullptr)
                    FINJIN_SET_ERROR(error, "Failed to instantiate scene node object at the end of a scene node object chunk.");
            }
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)
            CHECK_OBJECTS_NOT_EMPTY(obj, sceneNode->objects)

            if (propertyName == StandardAssetDocumentPropertyNames::TYPE)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& typeName = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, typeName, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TYPE.name));
                else if (isValueDone)
                {
                    if (callbacks->GetSettings().TranslateType(typeName).HasError())
                        FINJIN_SET_ERROR(error, "Failed to translate type.");
                    else
                    {
                        obj = CreateInstanceOf<FinjinSceneObjectBase>(typeName, state.allocator, FINJIN_CALLER_ARGUMENTS, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create object of type '%1%'.", typeName));
                        else
                        {
                            if (obj->IsTypeOf(FINJIN_TYPE_DESCRIPTION(FinjinSceneMovableObject)))
                            {
                                auto movable = static_cast<FinjinSceneMovableObject*>(obj);
                                movable->parentNodePointer = sceneNode;
                            }

                            PRINT_VALUE(state.GetDepth(), "type", typeName);
                        }
                    }
                }
            }
            else
            {
                if (obj == nullptr)
                {
                    FINJIN_SET_ERROR(error, "Unable to read object properties. Object is null.");
                    return;
                }

                auto propertyHandled = false;

                if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                {
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, obj->name, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                    else if (isValueDone)
                        PRINT_VALUE(state.GetDepth(), "name", obj->name);

                    propertyHandled = true;
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::ID)
                {
                    reader.ReadUuid(dataHeader, obj->id, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read UUID property '%1%'.", StandardAssetDocumentPropertyNames::ID.name));
                    else
                        PRINT_VALUE(state.GetDepth(), "id", obj->id);

                    propertyHandled = true;
                }

                if (!propertyHandled && obj->IsTypeOf(FINJIN_TYPE_DESCRIPTION(FinjinSceneMovableObject)))
                {
                    auto movable = static_cast<FinjinSceneMovableObject*>(obj);

                    if (propertyName == StandardAssetDocumentPropertyNames::TRANSFORM)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, movable->transform.data(), 16, error) == 16;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::TRANSFORM.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::TRANSFORM.name));
                        else
                            PRINT_TRANSFORM(state.GetDepth(), movable->transform);

                        propertyHandled = true;
                    }
                }

                if (!propertyHandled && obj->IsTypeOf(FINJIN_TYPE_DESCRIPTION(FinjinSceneRenderableMovableObject)))
                {
                    auto renderableMovable = static_cast<FinjinSceneRenderableMovableObject*>(obj);

                    if (propertyName == StandardAssetDocumentPropertyNames::RENDER_QUEUE)
                    {
                        if (dataHeader.IsOnlyOrFirstOccurrence())
                            callbacks->tempString.clear();
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, callbacks->tempString, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::RENDER_QUEUE.name));
                        else if (isValueDone)
                        {
                            if (callbacks->GetSettings().parseRenderQueue != nullptr)
                            {
                                auto parsedRenderQueue = callbacks->GetSettings().parseRenderQueue(callbacks->tempString);
                                if (parsedRenderQueue.HasError())
                                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse render queue value '%1%'.", callbacks->tempString));
                                else
                                {
                                    renderableMovable->renderQueue = parsedRenderQueue.value;
                                    PRINT_VALUE(state.GetDepth(), "render queue (string)", callbacks->tempString);
                                    PRINT_VALUE(state.GetDepth(), "render queue (parsed)", renderableMovable->renderQueue);
                                }
                            }
                            else
                                PRINT_VALUE(state.GetDepth(), "render queue (string)", callbacks->tempString);
                        }

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::RENDER_PRIORITY)
                    {
                        reader.ReadInt32(dataHeader, renderableMovable->renderPriority, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read int32 property '%1%'.", StandardAssetDocumentPropertyNames::RENDER_PRIORITY.name));
                        else
                            PRINT_VALUE(state.GetDepth(), "render priority", renderableMovable->renderPriority);

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::RENDER_DISTANCE)
                    {
                        reader.ReadFloat(dataHeader, renderableMovable->renderDistance, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::RENDER_DISTANCE.name));
                        else
                            PRINT_VALUE(state.GetDepth(), "render distance", renderableMovable->renderDistance);

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::CAST_SHADOWS)
                    {
                        reader.ReadBool(dataHeader, renderableMovable->castShadows, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read boolean property '%1%'.", StandardAssetDocumentPropertyNames::CAST_SHADOWS.name));
                        else
                            PRINT_VALUE(state.GetDepth(), "cast shadows", (int)renderableMovable->castShadows);

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::RECEIVE_SHADOWS)
                    {
                        reader.ReadBool(dataHeader, renderableMovable->receiveShadows, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read boolean property '%1%'.", StandardAssetDocumentPropertyNames::RECEIVE_SHADOWS.name));
                        else
                            PRINT_VALUE(state.GetDepth(), "receive shadows", (int)renderableMovable->receiveShadows);

                        propertyHandled = true;
                    }
                }

                if (!propertyHandled && obj->IsTypeOf(FINJIN_TYPE_DESCRIPTION(FinjinSceneObjectEntity)))
                {
                    auto entity = static_cast<FinjinSceneObjectEntity*>(obj);

                    if (propertyName == StandardAssetDocumentPropertyNames::MESH_REF)
                    {
                        if (dataHeader.IsOnlyOrFirstOccurrence())
                            callbacks->tempString.clear();
                        auto& meshRef = callbacks->tempString;
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, meshRef, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MESH_REF.name));
                        else if (isValueDone)
                        {
                            entity->meshHandle.assetRef.ForUriString(meshRef, callbacks->tempSimpleUri, error);
                            if (error)
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse mesh reference '%1%'.", meshRef));
                            else
                            {
                                PRINT_VALUE(state.GetDepth(), "mesh ref", meshRef);
                                state.RecordAssetHandle(&entity->meshHandle, error);
                                if (error)
                                    FINJIN_SET_ERROR(error, "Failed to record mesh asset.");
                            }
                        }

                        propertyHandled = true;
                    }
                }

                if (!propertyHandled && obj->IsTypeOf(FINJIN_TYPE_DESCRIPTION(FinjinSceneObjectCamera)))
                {
                    auto camera = static_cast<FinjinSceneObjectCamera*>(obj);

                    if (propertyName == StandardAssetDocumentPropertyNames::FOV)
                    {
                        float fov;
                        reader.ReadFloat(dataHeader, fov, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::FOV.name));
                        else
                        {
                            PRINT_VALUE(state.GetDepth(), "fov", fov);

                            camera->fovY = Radians(fov);
                        }

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::PROJECTION_TYPE)
                    {
                        if (dataHeader.IsOnlyOrFirstOccurrence())
                            callbacks->tempString.clear();
                        auto& projectionType = callbacks->tempString;
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, projectionType, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::PROJECTION_TYPE.name));
                        else if (isValueDone)
                        {
                            if (projectionType == StandardAssetDocumentPropertyValues::CameraProjection::ORTHOGRAPHIC)
                                camera->projectionType = CameraProjectionType::ORTHOGRAPHIC;
                            else if (projectionType == StandardAssetDocumentPropertyValues::CameraProjection::PERSPECTIVE)
                                camera->projectionType = CameraProjectionType::PERSPECTIVE;

                            PRINT_VALUE(state.GetDepth(), "projection", projectionType);
                        }

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::ORTHO_SIZE)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, camera->orthoSize, 2, error) == 2;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::ORTHO_SIZE.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::ORTHO_SIZE.name));
                        else
                            PRINT_VALUE2(state.GetDepth(), "ortho size", camera->orthoSize[0], camera->orthoSize[1]);

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::CLIPPING)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, camera->range, 2, error) == 2;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::CLIPPING.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::CLIPPING.name));
                        else
                            PRINT_VALUE2(state.GetDepth(), "clipping", camera->range[0], camera->range[1]);

                        propertyHandled = true;
                    }
                }

                if (!propertyHandled && obj->IsTypeOf(FINJIN_TYPE_DESCRIPTION(FinjinSceneObjectLight)))
                {
                    auto light = static_cast<FinjinSceneObjectLight*>(obj);

                    if (propertyName == StandardAssetDocumentPropertyNames::LIGHT_TYPE)
                    {
                        if (dataHeader.IsOnlyOrFirstOccurrence())
                            callbacks->tempString.clear();
                        auto& lightType = callbacks->tempString;
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, lightType, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::LIGHT_TYPE.name));
                        else if (isValueDone)
                        {
                            if (lightType == StandardAssetDocumentPropertyValues::LightType::DIRECTIONAL)
                                light->lightType = LightType::DIRECTIONAL;
                            else if (lightType == StandardAssetDocumentPropertyValues::LightType::POINT)
                                light->lightType = LightType::POINT;
                            else if (lightType == StandardAssetDocumentPropertyValues::LightType::SPOT)
                                light->lightType = LightType::SPOT;

                            PRINT_VALUE(state.GetDepth(), "light type", lightType);

                            propertyHandled = true;
                        }
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::ACTIVE)
                    {
                        reader.ReadBool(dataHeader, light->isActive, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read boolean property '%1%'.", StandardAssetDocumentPropertyNames::ACTIVE.name));
                        else
                            PRINT_VALUE(state.GetDepth(), "is active", (int)light->isActive);

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::POWER)
                    {
                        reader.ReadFloat(dataHeader, light->power, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::POWER.name));
                        else
                            PRINT_VALUE(state.GetDepth(), "power", light->power);

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::COLOR)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, light->color.data(), 4, error) == 4;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::COLOR.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::COLOR.name));
                        else
                            PRINT_VALUE4(state.GetDepth(), "color", light->color(0), light->color(1), light->color(2), light->color(3));

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::RANGE)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, light->range, 2, error) == 2;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::RANGE.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::RANGE.name));
                        else
                            PRINT_VALUE2(state.GetDepth(), "range", light->range[0], light->range[1]);

                        propertyHandled = true;
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::CONE_RANGE)
                    {
                        float coneRange[2];
                        auto isValueDone = reader.ReadFloats(dataHeader, coneRange, 2, error) == 2;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::CONE_RANGE.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::CONE_RANGE.name));
                        else
                        {
                            light->coneRange[0] = Radians(coneRange[0]);
                            light->coneRange[1] = Radians(coneRange[1]);

                            PRINT_VALUE2(state.GetDepth(), "cone range", coneRange[0], coneRange[1]);
                        }

                        propertyHandled = true;
                    }
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for scene node object.");
            return;
        }
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[objects]/[object-index]/[user-data]
    AddUserDataMappings<State>
        (
        callbacks,
        [getData](UserDataClassInstance*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)
            CHECK_OBJECTS_NOT_EMPTY(obj, sceneNode->objects)

            result = &obj->userData;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::OBJECTS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for scene node object user data.");
        return;
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[objects]/[object-index]/[note-tracks]
    AddNoteTracksMappings<State>
        (
        callbacks,
        [getData](DynamicVector<FinjinSceneObjectBase::NoteTrack>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)
            CHECK_OBJECTS_NOT_EMPTY(obj, sceneNode->objects)

            result = &obj->noteTracks;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::OBJECTS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for scene node object note tracks.");
        return;
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[objects]/[object-index]/[flags]
    AddFlagsMappings<State>
        (
        callbacks,
        [getData](DynamicVector<uint8_t>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)
            CHECK_OBJECTS_NOT_EMPTY(obj, sceneNode->objects)

            result = &obj->flags;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::OBJECTS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for scene node object note tracks.");
        return;
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[objects]/[object-index (entity)]/[subentities]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::OBJECTS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::SUBENTITIES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene node object subentities pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBENTITIES", isStart);
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)
            CHECK_OBJECTS_NOT_EMPTY(obj, sceneNode->objects)
            CHECK_OBJECT_OF_TYPE(entity, obj, FinjinSceneObjectEntity)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!entity->subentities.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate entity subentities.");
                    else
                        PRINT_VALUE(state.GetDepth(), "subentities count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for subentities.");
            return;
        }
    }

    //basePattern/[scene-nodes]/[scene-node-index]/[objects]/[object-index (entity)]/[subentities]/[subentity-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE_NODES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::OBJECTS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::SUBENTITIES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene node object subentity pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBENTITY", isStart);

            if (isStart)
            {
                DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
                getData(sceneNodes, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                    return;
                }

                CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)
                CHECK_OBJECTS_NOT_EMPTY(obj, sceneNode->objects)
                CHECK_OBJECT_OF_TYPE(entity, obj, FinjinSceneObjectEntity)
                entity->subentities.push_back();
            }
        };
        mapping.propertyCallback = [callbacks, getData](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            DynamicVector<FinjinSceneNode*>* sceneNodes = nullptr;
            getData(sceneNodes, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get scene nodes.");
                return;
            }

            CHECK_SCENE_NODES_NOT_EMPTY_NOT_NULL(sceneNode, *sceneNodes)
            CHECK_OBJECTS_NOT_EMPTY(obj, sceneNode->objects)
            CHECK_OBJECT_OF_TYPE(entity, obj, FinjinSceneObjectEntity)
            CHECK_SUBENTITIES_NOT_EMPTY(subentity, entity->subentities)

            if (propertyName == StandardAssetDocumentPropertyNames::MATERIAL_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& materialRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, materialRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MATERIAL_REF.name));
                else if (isValueDone)
                {
                    subentity.materialHandle.assetRef.ForUriString(materialRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse material reference '%1%'.", materialRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "material ref", materialRef);

                        state.RecordAssetHandle(&subentity.materialHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record material asset.");
                    }
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for subentity.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddSkeletonAnimationMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(AssetHandle<FinjinMeshSkeleton::Animation>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON BONE ANIMATION", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton::Animation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::SKELETON_ANIMATION_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& animationRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, animationRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::SKELETON_ANIMATION_REF.name));
                else if (isValueDone)
                {
                    animationHandle->assetRef.ForUriString(animationRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse skeleton animation reference '%1%'.", animationRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "skeleton animation ref", animationRef);

                        state.RecordAssetHandle(animationHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record skeleton animation asset.");
                    }
                }
            }
            else
            {
                if (animation == nullptr)
                {
                    animationHandle->isOwner = true;

                    animation = AllocatedClass::New<FinjinMeshSkeleton::Animation>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                    if (animation == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to allocate skeleton animation.");
                        return;
                    }
                }

                if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                {
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, animation->name, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                    else if (isValueDone)
                    {
                        PRINT_VALUE(state.GetDepth(), "animation name", animation->name);

                        animationHandle->assetRef.objectName = animation->name;

                        state.RecordAssetHandle(animationHandle, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to record skeleton animation asset.");
                            return;
                        }
                    }
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::LENGTH)
                {
                    reader.ReadTimeDuration(dataHeader, animation->length, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read time duration property '%1%'.", StandardAssetDocumentPropertyNames::LENGTH.name));
                    else
                        PRINT_VALUE(state.GetDepth(), "animation time duration", animation->length.ToString());
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton animation.");
            return;
        }
    }

    //basePattern/[bones]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::BONES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add skeleton animation bones pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON ANIMATION BONES", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton::Animation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!animation->bones.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate animation bones.");
                    else
                        PRINT_VALUE(state.GetDepth(), "skeleton animation bone count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton animation bones.");
            return;
        }
    }

    //basePattern/[bones]/[bone-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::BONES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add skeleton animation bone pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON ANIMATION BONE", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton::Animation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_BONE_NOT_EMPTY(bone, animation->bones)

            if (propertyName == StandardAssetDocumentPropertyNames::BONE)
            {
                reader.ReadCount(dataHeader, bone.index, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::BONE.name));
                else
                    PRINT_VALUE(state.GetDepth(), "skeleton animation bone index", bone.index);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton animation bone.");
            return;
        }
    }

    //basePattern/[bones]/[bone-index]/[keys]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SKELETON, StandardAssetDocumentChunkNames::SKELETON_ANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::BONES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add skeleton animation bone keys pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON ANIMATION BONE KEYS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton::Animation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_BONE_NOT_EMPTY(boneAnimation, animation->bones)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!boneAnimation.keys.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate bone animation keys.");
                    else
                        PRINT_VALUE(state.GetDepth(), "skeleton animation bone keys count", value);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TIME)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                auto& key = boneAnimation.keys[state.bufferValuesRead];

                state.bufferValuesRead += reader.ReadStridedTimeDurations(dataHeader, &key.time, boneAnimation.keys.size() - state.bufferValuesRead, DataChunkReadStride(1, sizeof(FinjinMeshSkeleton::Animation::Bone::Key)), error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read skeleton bone animation key times.");
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TRANSFORM)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                auto isValueDone = false;
                auto offsetInKeyValue = state.bufferValuesRead % 16;
                if (offsetInKeyValue != 0)
                {
                    auto& key = boneAnimation.keys[state.bufferValuesRead / 16];

                    state.bufferValuesRead += reader.ReadFloats(dataHeader, key.m44.data() + offsetInKeyValue, 16 - offsetInKeyValue, error);
                    if (error)
                        FINJIN_SET_ERROR(error, "Failed to read bone animation key transforms.");
                    else
                    {
                        if (state.bufferValuesRead == boneAnimation.keys.size() * 16)
                            isValueDone = true;
                    }
                }
                if (!isValueDone && !error)
                {
                    auto& key = boneAnimation.keys[state.bufferValuesRead / 16];

                    state.bufferValuesRead += reader.ReadStridedFloats(dataHeader, key.m44.data(), boneAnimation.keys.size() * 16 - state.bufferValuesRead, DataChunkReadStride(16, sizeof(FinjinNodeAnimation::Key)), error);
                    if (error)
                        FINJIN_SET_ERROR(error, "Failed to read bone animation key transforms.");
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton animation bone keys.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddIndexBufferMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(FinjinMesh::IndexBuffer*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "INDEX BUFFER", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            FinjinMesh::IndexBuffer* indexBuffer = nullptr;
            getData(indexBuffer, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get index buffer.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::TYPE)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& type = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, type, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TYPE.name));
                else if (isValueDone)
                {
                    if (type == StandardAssetDocumentPropertyValues::IndexBufferType::UINT16)
                        indexBuffer->type = FinjinIndexBufferType::UINT16;
                    else if (type == StandardAssetDocumentPropertyValues::IndexBufferType::UINT32)
                        indexBuffer->type = FinjinIndexBufferType::UINT32;

                    PRINT_VALUE(state.GetDepth(), "type", type);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (indexBuffer->type == FinjinIndexBufferType::UINT16)
                    {
                        if (!indexBuffer->uint16s.Create(value, state.allocator))
                            FINJIN_SET_ERROR(error, "Failed to allocate index buffer uint16 values.");
                    }
                    else
                    {
                        if (!indexBuffer->uint32s.Create(value, state.allocator))
                            FINJIN_SET_ERROR(error, "Failed to allocate index buffer uint32 values.");
                    }

                    PRINT_VALUE(state.GetDepth(), "index buffer count", value);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::VALUES)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                if (indexBuffer->type == FinjinIndexBufferType::UINT16)
                {
                    state.bufferValuesRead += reader.ReadUInt16s(dataHeader, indexBuffer->uint16s.data() + state.bufferValuesRead, indexBuffer->uint16s.size() - state.bufferValuesRead, error);
                    if (error)
                        FINJIN_SET_ERROR(error, "Failed to read index buffer uint16 values.");
                    else
                    {
                        if (state.bufferValuesRead == indexBuffer->uint16s.size())
                            PRINT_VALUES(state.GetDepth(), "index buffer 16bit values", indexBuffer->uint16s);
                    }
                }
                else
                {
                    state.bufferValuesRead += reader.ReadUInt32s(dataHeader, indexBuffer->uint32s.data() + state.bufferValuesRead, indexBuffer->uint32s.size() - state.bufferValuesRead, error);
                    if (error)
                        FINJIN_SET_ERROR(error, "Failed to read index buffer uint32 values.");
                    else
                    {
                        if (state.bufferValuesRead == indexBuffer->uint32s.size())
                            PRINT_VALUES(state.GetDepth(), "index buffer 32bit values", indexBuffer->uint32s);
                    }
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for index buffer.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddVertexBufferMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(FinjinMesh::VertexBuffer*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "VERTEX BUFFER", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            FinjinMesh::VertexBuffer* vertexBuffer = nullptr;
            getData(vertexBuffer, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get vertex buffer.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::FORMAT)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, vertexBuffer->formatName, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::FORMAT.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "format name", vertexBuffer->formatName);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::VERTEX_COUNT)
            {
                reader.ReadCount(dataHeader, vertexBuffer->vertexCount, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::VERTEX_COUNT.name));
                else
                    PRINT_VALUE(state.GetDepth(), "vertex count", vertexBuffer->vertexCount);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for vertex buffer.");
            return;
        }
    }

    //basePattern/[format]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::FORMAT).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to vertex buffer format data pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "VERTEX BUFFER FORMAT", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            FinjinMesh::VertexBuffer* vertexBuffer = nullptr;
            getData(vertexBuffer, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get vertex buffer.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!vertexBuffer->formatElements.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate vertex buffer format elements.");
                    else
                        PRINT_VALUE(state.GetDepth(), "vertex buffer format elements count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for submesh vertex buffer format.");
            return;
        }
    }

    //basePattern/[format]/[element-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::FORMAT, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add vertex buffer format element pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "VERTEX BUFFER FORMAT ELEMENT", isStart);

            if (isStart)
            {
                FinjinMesh::VertexBuffer* vertexBuffer = nullptr;
                getData(vertexBuffer, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get vertex buffer.");
                    return;
                }

                vertexBuffer->formatElements.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            FinjinMesh::VertexBuffer* vertexBuffer = nullptr;
            getData(vertexBuffer, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get vertex buffer.");
                return;
            }

            CHECK_VERTEX_BUFFER_FORMAT_NOT_EMPTY(formatElement, vertexBuffer->formatElements)

            ReadFormatElement(formatElement, propertyName, reader, dataHeader, callbacks, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read vertex format element.");
                return;
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for vertex buffer format element.");
            return;
        }
    }

    //basePattern/[channels]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::CHANNELS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add vertex buffer channels pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "VERTEX BUFFER CHANNELS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            FinjinMesh::VertexBuffer* vertexBuffer = nullptr;
            getData(vertexBuffer, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get vertex buffer.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!vertexBuffer->channels.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate vertex buffer channels.");
                    else
                        PRINT_VALUE(state.GetDepth(), "vertex buffer channels count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for vertex buffer channels.");
            return;
        }
    }

    //basePattern/[channels]/[channel-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::CHANNELS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add vertex buffer channel pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData, callbacks](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "VERTEX BUFFER CHANNEL", isStart);

            if (isStart)
            {
                FinjinMesh::VertexBuffer* vertexBuffer = nullptr;
                getData(vertexBuffer, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get vertex buffer.");
                    return;
                }

                PrepareToReadChannelValues(vertexBuffer->channels, vertexBuffer->vertexCount, vertexBuffer->formatElements, callbacks->GetSettings(), state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to prepare to read vertex buffer channel.");
                    return;
                }
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            FinjinMesh::VertexBuffer* vertexBuffer = nullptr;
            getData(vertexBuffer, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get vertex buffer.");
                return;
            }

            CHECK_VERTEX_BUFFER_CHANNELS_NOT_EMPTY(channel, vertexBuffer->channels)

            if (propertyName == StandardAssetDocumentPropertyNames::VALUES)
            {
                ReadChannelValues(channel, reader, dataHeader, state, callbacks->GetSettings(), error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to read vertex buffer channel.");
                    return;
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for submesh vertex buffer channel.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddSkeletonMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(FinjinMeshSkeleton*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton>* skeletonHandle = nullptr;
            getData(skeletonHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton.");
                return;
            }

            auto& skeleton = skeletonHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::SKELETON_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& skeletonRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, skeletonRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::SKELETON_REF.name));
                else if (isValueDone)
                {
                    skeletonHandle->assetRef.ForUriString(skeletonRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse skeleton reference '%1%'.", skeletonRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "skeleton ref", skeletonRef);

                        state.RecordAssetHandle(skeletonHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record skeleton asset.");
                    }
                }
            }
            else
            {
                if (skeleton == nullptr)
                {
                    skeletonHandle->isOwner = true;

                    skeleton = AllocatedClass::New<FinjinMeshSkeleton>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                    if (skeleton == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to allocate skeleton.");
                        return;
                    }
                }

                if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                {
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, skeleton->name, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                    else if (isValueDone)
                    {
                        PRINT_VALUE(state.GetDepth(), "name", skeleton->name);

                        skeletonHandle->assetRef.objectName = skeleton->name;

                        state.RecordAssetHandle(skeletonHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record skeleton asset.");
                    }
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton.");
            return;
        }
    }

    //basePattern/[bones]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::BONES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add skeleton bones pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON BONES", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton>* skeletonHandle = nullptr;
            getData(skeletonHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton.");
                return;
            }

            auto& skeleton = skeletonHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!skeleton->bones.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate skeleton bones.");
                    else
                        PRINT_VALUE(state.GetDepth(), "skeleton bone count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton bones.");
            return;
        }
    }

    //basePattern/[bones]/[bone-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::BONES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add skeleton bone pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON BONE", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMeshSkeleton>* skeletonHandle = nullptr;
                getData(skeletonHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get skeleton.");
                    return;
                }

                auto& skeleton = skeletonHandle->asset;

                skeleton->bones.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton>* skeletonHandle = nullptr;
            getData(skeletonHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton.");
                return;
            }

            auto& skeleton = skeletonHandle->asset;

            CHECK_BONE_NOT_EMPTY(bone, skeleton->bones)

            if (propertyName == StandardAssetDocumentPropertyNames::NAME)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, bone.name, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "skeleton bone name", bone.name);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::PARENT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::PARENT.name));
                else if (value >= skeleton->bones.size())
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Invalid parent index '%1%'. Must be between [0-%2%].", value, skeleton->bones.size() - 1));
                else
                {
                    auto& parentBone = skeleton->bones[value];
                    bone.parentBonePointer = &parentBone;
                    if (parentBone.childBonePointers.push_back(&parentBone).HasErrorOrValue(false))
                        FINJIN_SET_ERROR(error, "Failed to add child bone pointer to parent bone.");
                    else
                        PRINT_VALUE(state.GetDepth(), "parent index", value);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::CHILD_COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::CHILD_COUNT.name));
                else
                {
                    if (!bone.childBonePointers.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate child pointers.");
                    else
                        PRINT_VALUE(state.GetDepth(), "child count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton bone.");
            return;
        }
    }

    //basePattern/[animations]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SKELETON_ANIMATIONS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add skeleton animations pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON ANIMATIONS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton>* skeletonHandle = nullptr;
            getData(skeletonHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton.");
                return;
            }

            auto& skeleton = skeletonHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!skeleton->animationHandles.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate skeleton animations.");
                    else
                        PRINT_VALUE(state.GetDepth(), "skeleton animation count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton animations.");
            return;
        }
    }

    //basePattern/[animations]/[animation-index]
    AddSkeletonAnimationMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (isStart)
            {
                AssetHandle<FinjinMeshSkeleton>* skeletonHandle = nullptr;
                getData(skeletonHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get skeleton.");
                    return;
                }

                auto& skeleton = skeletonHandle->asset;

                skeleton->animationHandles.push_back();
            }
        },
        [getData, callbacks](AssetHandle<FinjinMeshSkeleton::Animation>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMeshSkeleton>* skeletonHandle = nullptr;
            getData(skeletonHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get skeleton.");
                return;
            }

            auto& skeleton = skeletonHandle->asset;

            result = &skeleton->animationHandles.back();
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SKELETON_ANIMATIONS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for skeleton animation.");
        return;
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddMorphAnimationMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(FinjinMesh::MorphAnimation*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MORPH ANIMATION", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::MORPH_ANIMATION_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& animationRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, animationRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MORPH_ANIMATION_REF.name));
                else if (isValueDone)
                {
                    animationHandle->assetRef.ForUriString(animationRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse morph animation reference '%1%'.", animationRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "morph animation ref", animationRef);
                        state.RecordAssetHandle(animationHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record morph animation asset.");
                    }
                }
            }
            else
            {
                if (animation == nullptr)
                {
                    animationHandle->isOwner = true;

                    animation = AllocatedClass::New<FinjinMesh::MorphAnimation>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                    if (animation == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to allocate morph animation.");
                        return;
                    }
                }

                if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                {
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, animation->name, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                    else if (isValueDone)
                    {
                        PRINT_VALUE(state.GetDepth(), "morph animation name", animation->name);

                        animationHandle->assetRef.objectName = animation->name;

                        state.RecordAssetHandle(animationHandle, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to record morph animation asset.");
                            return;
                        }
                    }
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::LENGTH)
                {
                    reader.ReadTimeDuration(dataHeader, animation->length, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read time duration property '%1%'.", StandardAssetDocumentPropertyNames::LENGTH.name));
                    else
                        PRINT_VALUE(state.GetDepth(), "morph animation time duration", animation->length.ToString());
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation.");
            return;
        }
    }

    //basePattern/[subanimations]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add morph animation subanimations pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MORPH SUBANIMATIONS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!animation->subanimations.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate morph animation subanimations.");
                    else
                        PRINT_VALUE(state.GetDepth(), "morph subanimation count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation subanimations.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add morph animation subanimation pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MORPH SUBANIMATION", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
                getData(animationHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                    return;
                }

                auto& animation = animationHandle->asset;

                animation->subanimations.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)

            if (propertyName == StandardAssetDocumentPropertyNames::SUBMESH)
            {
                reader.ReadCount(dataHeader, subanimation.submeshIndex, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                    PRINT_VALUE(state.GetDepth(), "submesh index", subanimation.submeshIndex);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation subanimation.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]/[format]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::FORMAT).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add morph animation subanimation format pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MORPH ANIMATION SUBANIMATION FORMAT", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!subanimation.formatElements.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate subanimation format elements.");
                    else
                        PRINT_VALUE(state.GetDepth(), "submesh vertex buffer format elements count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation subanimation format.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]/[format]/[element-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::FORMAT, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add morph animation subanimation format element pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MORPH ANIMATION SUBANIMATION FORMAT ELEMENT", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
                getData(animationHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                    return;
                }

                auto& animation = animationHandle->asset;

                CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
                subanimation.formatElements.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
            CHECK_MORPH_ANIMATION_SUBANIMATION_FORMAT_NOT_EMPTY(formatElement, subanimation.formatElements)

            ReadFormatElement(formatElement, propertyName, reader, dataHeader, callbacks, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read morph animation subanimation vertex format element.");
                return;
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation subanimation format element.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]/[keys]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add morph animation subanimation keys pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON MORPH SUBANIMATION KEYS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!subanimation.keys.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate subanimation keys.");
                    else
                        PRINT_VALUE(state.GetDepth(), "key count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation subanimation keys.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]/[keys]/[key-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add morph animation subanimation key pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON MORPH SUBANIMATION KEY", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
                getData(animationHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                    return;
                }

                auto& animation = animationHandle->asset;

                CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
                subanimation.keys.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
            CHECK_MORPH_ANIMATION_SUBANIMATION_KEYS_NOT_EMPTY(key, subanimation.keys)

            if (propertyName == StandardAssetDocumentPropertyNames::TIME)
            {
                reader.ReadTimeDuration(dataHeader, key.time, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read time duration property '%1%'.", StandardAssetDocumentPropertyNames::TIME.name));
                else
                    PRINT_VALUE(state.GetDepth(), "time", key.time.ToString());
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::VERTEX_COUNT)
            {
                reader.ReadCount(dataHeader, key.vertexCount, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::VERTEX_COUNT.name));
                else
                    PRINT_VALUE(state.GetDepth(), "vertex count", key.vertexCount);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation subanimation key.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]/[keys]/[key-index]/[channels]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::CHANNELS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add morph animation subanimation key channels pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MORPH SUBANIMATION KEY CHANNELS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
            CHECK_MORPH_ANIMATION_SUBANIMATION_KEYS_NOT_EMPTY(key, subanimation.keys)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!key.channels.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate key channels.");
                    else
                        PRINT_VALUE(state.GetDepth(), "count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation subanimation key channels.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]/[keys]/[key-index]/[channels]/[channel-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::CHANNELS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add user data pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData, callbacks](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MORPH SUBANIMATION KEY CHANNEL", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
                getData(animationHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                    return;
                }

                auto& animation = animationHandle->asset;

                CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
                CHECK_MORPH_ANIMATION_SUBANIMATION_KEYS_NOT_EMPTY(key, subanimation.keys)

                PrepareToReadChannelValues(key.channels, key.vertexCount, subanimation.formatElements, callbacks->GetSettings(), state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to prepare to read morph subanimation key channel.");
                    return;
                }
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::MorphAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get morph animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::VALUES)
            {
                CHECK_MORPH_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
                CHECK_MORPH_ANIMATION_SUBANIMATION_KEYS_NOT_EMPTY(key, subanimation.keys)
                CHECK_MORPH_ANIMATION_SUBANIMATION_KEY_CHANNELS_NOT_EMPTY(channel, key.channels)

                ReadChannelValues(channel, reader, dataHeader, state, callbacks->GetSettings(), error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to read morph subanimation key channel.");
                    return;
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation subanimation key channel.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddPoseAnimationMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(AssetHandle<FinjinMesh::PoseAnimation>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "POSE ANIMATION", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::PoseAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get pose animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::POSE_ANIMATION_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& animationRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, animationRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::POSE_ANIMATION_REF.name));
                else if (isValueDone)
                {
                    animationHandle->assetRef.ForUriString(animationRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse pose animation reference '%1%'.", animationRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "pose animation ref", animationRef);
                        state.RecordAssetHandle(animationHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record pose animation asset.");
                    }
                }
            }
            else
            {
                if (animation == nullptr)
                {
                    animationHandle->isOwner = true;

                    animation = AllocatedClass::New<FinjinMesh::PoseAnimation>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                    if (animation == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to allocate pose animation.");
                        return;
                    }
                }

                if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                {
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, animation->name, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                    else if (isValueDone)
                    {
                        PRINT_VALUE(state.GetDepth(), "animation name", animation->name);

                        animationHandle->assetRef.objectName = animation->name;

                        state.RecordAssetHandle(animationHandle, error);
                        if (error)
                        {
                            FINJIN_SET_ERROR(error, "Failed to record pose animation asset.");
                            return;
                        }
                    }
                }
                else if (propertyName == StandardAssetDocumentPropertyNames::LENGTH)
                {
                    reader.ReadTimeDuration(dataHeader, animation->length, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read time duration property '%1%'.", StandardAssetDocumentPropertyNames::LENGTH.name));
                    else
                        PRINT_VALUE(state.GetDepth(), "animation time duration", animation->length.ToString());
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton animation.");
            return;
        }
    }

    //basePattern/[subanimations]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose animation subanimations pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "POSE SUBANIMATIONS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::PoseAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get pose animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!animation->subanimations.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate subanimations.");
                    else
                        PRINT_VALUE(state.GetDepth(), "pose subanimation count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for pose subanimations.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subanimation pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "POSE SUBANIMATION", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh::PoseAnimation>* animationHandle = nullptr;
                getData(animationHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get pose animation.");
                    return;
                }

                auto& animation = animationHandle->asset;

                animation->subanimations.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::PoseAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get pose animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_POSE_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)

            if (propertyName == StandardAssetDocumentPropertyNames::SUBMESH)
            {
                reader.ReadCount(dataHeader, subanimation.submeshIndex, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::SUBMESH.name));
                else
                    PRINT_VALUE(state.GetDepth(), "pose subanimation submesh index", subanimation.submeshIndex);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for pose subanimation.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]/[keys]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subanimation keys pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "POSE SUBANIMATION KEYS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::PoseAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get pose animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_POSE_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!subanimation.keys.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate subanimation keys.");
                    else
                        PRINT_VALUE(state.GetDepth(), "pose subanimation keys count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for pose subanimation keys.");
            return;
        }
    }

    //basePattern/[subanimations]/[subanimation-index]/[keys]/[key-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBANIMATIONS, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::KEYS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subanimation key pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "POSE SUBANIMATION KEY", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh::PoseAnimation>* animationHandle = nullptr;
                getData(animationHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get pose animation.");
                    return;
                }

                auto& animation = animationHandle->asset;

                CHECK_POSE_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
                subanimation.keys.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh::PoseAnimation>* animationHandle = nullptr;
            getData(animationHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get pose animation.");
                return;
            }

            auto& animation = animationHandle->asset;

            CHECK_POSE_ANIMATION_SUBANIMATIONS_NOT_EMPTY(subanimation, animation->subanimations)
            CHECK_POSE_ANIMATION_SUBANIMATION_KEY_NOT_EMPTY(key, subanimation.keys)

            if (propertyName == StandardAssetDocumentPropertyNames::TIME)
            {
                reader.ReadTimeDuration(dataHeader, key.time, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read time duration property '%1%'.", StandardAssetDocumentPropertyNames::TIME.name));
                else
                    PRINT_VALUE(state.GetDepth(), "time duration", key.time.ToString());
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::INFLUENCE_COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::INFLUENCE_COUNT.name));
                else
                {
                    if (!key.influences.Create(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate subpose influence values.");
                    else if (!key.submeshIndexes.Create(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate subpose submesh index values.");
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::INFLUENCE)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                state.bufferValuesRead += reader.ReadFloats(dataHeader, key.influences.data() + state.bufferValuesRead, key.influences.size() - state.bufferValuesRead, error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read influences.");
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::SUBMESH)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                state.bufferValuesRead += reader.ReadCounts(dataHeader, key.submeshIndexes.data() + state.bufferValuesRead, key.submeshIndexes.size() - state.bufferValuesRead, error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read submesh indexes.");
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for pose subanimation key.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddMeshMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(AssetHandle<FinjinMesh>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MESH", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::MESH_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& meshRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, meshRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MESH_REF.name));
                else if (isValueDone)
                {
                    meshHandle->assetRef.ForUriString(meshRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse mesh reference '%1%'.", meshRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "mesh ref", meshRef);
                        state.RecordAssetHandle(meshHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record mesh asset.");
                    }
                }
            }
            else
            {
                auto& mesh = meshHandle->asset;

                if (propertyName == StandardAssetDocumentPropertyNames::TYPE)
                {
                    if (dataHeader.IsOnlyOrFirstOccurrence())
                        callbacks->tempString.clear();
                    auto& typeName = callbacks->tempString;
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, typeName, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TYPE.name));
                    else if (isValueDone)
                    {
                        if (callbacks->GetSettings().TranslateType(typeName).HasError())
                            FINJIN_SET_ERROR(error, "Failed to translate type.");
                        else
                        {
                            meshHandle->isOwner = true;

                            mesh = CreateInstanceOf<FinjinMesh>(typeName, state.allocator, FINJIN_CALLER_ARGUMENTS, error);
                            if (error)
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create object of type '%1%'.", typeName));
                            else
                                PRINT_VALUE(state.GetDepth(), "type", typeName);
                        }
                    }
                }
                else
                {
                    if (mesh == nullptr)
                        FINJIN_SET_ERROR(error, "Unable to read mesh properties. Mesh is null.");
                    else if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                    {
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, mesh->name, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                        else if (isValueDone)
                        {
                            PRINT_VALUE(state.GetDepth(), "name", mesh->name);

                            meshHandle->assetRef.objectName = mesh->name;

                            state.RecordAssetHandle(meshHandle, error);
                            if (error)
                                FINJIN_SET_ERROR(error, "Failed to record mesh asset.");
                        }
                    }
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for mesh.");
            return;
        }
    }

    //basePattern/[bounding-volume]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::BOUNDING_VOLUME).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add bounding volume pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "BOUNDING VOLUME", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::CUBE_CENTER)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, mesh->boundingVolume.cubeCenter.data(), 3, error) == 3;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::CUBE_CENTER.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::CUBE_CENTER.name));
                else
                    PRINT_VALUE3(state.GetDepth(), "cube center", mesh->boundingVolume.cubeCenter[0], mesh->boundingVolume.cubeCenter[1], mesh->boundingVolume.cubeCenter[2]);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::CUBE_SIZE)
            {
                reader.ReadFloat(dataHeader, mesh->boundingVolume.cubeSize, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::CUBE_SIZE.name));
                else
                    PRINT_VALUE(state.GetDepth(), "cube size", mesh->boundingVolume.cubeSize);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::BOX_CENTER)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, mesh->boundingVolume.boxCenter.data(), 3, error) == 3;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::BOX_CENTER.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::BOX_CENTER.name));
                else
                    PRINT_VALUE3(state.GetDepth(), "box center", mesh->boundingVolume.boxCenter[0], mesh->boundingVolume.boxCenter[1], mesh->boundingVolume.boxCenter[2]);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::BOX_SIZE)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, mesh->boundingVolume.boxSize.data(), 3, error) == 3;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::BOX_SIZE.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::BOX_SIZE.name));
                else
                    PRINT_VALUE3(state.GetDepth(), "box size", mesh->boundingVolume.boxSize[0], mesh->boundingVolume.boxSize[1], mesh->boundingVolume.boxSize[2]);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::RADIUS)
            {
                reader.ReadFloat(dataHeader, mesh->boundingVolume.radius, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::RADIUS.name));
                else
                    PRINT_VALUE(state.GetDepth(), "radius", mesh->boundingVolume.radius);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for bounding volume.");
            return;
        }
    }

    //basePattern/[index-buffer]
    AddIndexBufferMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }
        },
        [getData](FinjinMesh::IndexBuffer*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            result = &mesh->indexBuffer;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::INDEX_BUFFER
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for mesh index buffer.");
        return;
    }

    //basePattern/[vertex-buffers]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::VERTEX_BUFFERS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add vertex buffers pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "VERTEX BUFFERS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!mesh->vertexBuffers.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate vertex buffers.");
                    else
                        PRINT_VALUE(state.GetDepth(), "vertex buffer count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for mesh.");
            return;
        }
    }

    //basePattern/[vertex-buffers]/[vertex-buffer-index]
    AddVertexBufferMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                mesh->vertexBuffers.push_back();
            }
        },
        [getData, callbacks](FinjinMesh::VertexBuffer*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            result = &mesh->vertexBuffers.back();
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::VERTEX_BUFFERS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for morph animation.");
        return;
    }

    //basePattern/[submeshes]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBMESHES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add submeshes pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBMESHES", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!mesh->submeshes.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate submeshes.");
                    else
                        PRINT_VALUE(state.GetDepth(), "submesh count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for mesh.");
            return;
        }
    }

    //basePattern/[submeshes]/[submesh-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBMESHES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add submesh pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBMESH", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                mesh->submeshes.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_SUBMESHES_NOT_EMPTY(submesh, mesh->submeshes)

            if (propertyName == StandardAssetDocumentPropertyNames::NAME)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, submesh.name, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "name", submesh.name);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::MATERIAL_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& materialRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, materialRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MATERIAL_REF.name));
                else if (isValueDone)
                {
                    submesh.materialHandle.assetRef.ForUriString(materialRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse material reference '%1%'.", materialRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "material ref", materialRef);
                        state.RecordAssetHandle(&submesh.materialHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record material asset.");
                    }
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::PRIMITIVE_TYPE)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& primitiveType = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, primitiveType, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::PRIMITIVE_TYPE.name));
                else if (isValueDone)
                {
                    if (primitiveType == StandardAssetDocumentPropertyValues::PrimitiveType::TRIANGLE_LIST)
                        submesh.primitiveType = FinjinPrimitiveType::TRIANGLE_LIST;
                    else if (primitiveType == StandardAssetDocumentPropertyValues::PrimitiveType::LINE_LIST)
                        submesh.primitiveType = FinjinPrimitiveType::LINE_LIST;
                    else if (primitiveType == StandardAssetDocumentPropertyValues::PrimitiveType::POINT_LIST)
                        submesh.primitiveType = FinjinPrimitiveType::POINT_LIST;

                    PRINT_VALUE(state.GetDepth(), "primitive type", primitiveType);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for submesh.");
            return;
        }
    }

    //basePattern/[submeshes]/[submesh-index]/[index-buffer]
    AddIndexBufferMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_SUBMESHES_NOT_EMPTY(submesh, mesh->submeshes)
        },
        [getData](FinjinMesh::IndexBuffer*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_SUBMESHES_NOT_EMPTY(submesh, mesh->submeshes)

            result = &submesh.indexBuffer;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SUBMESHES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::INDEX_BUFFER
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for submesh index buffer.");
        return;
    }

    //basePattern/[submeshes]/[submesh-index]/[index-buffer-range]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBMESHES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::INDEX_BUFFER_RANGE).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add submesh index buffer range pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBMESH INDEX BUFFER RANGE", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_SUBMESHES_NOT_EMPTY(submesh, mesh->submeshes)

            if (propertyName == StandardAssetDocumentPropertyNames::BUFFER)
            {
                reader.ReadCount(dataHeader, submesh.indexBufferRange.bufferIndex, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::BUFFER.name));
                else
                    PRINT_VALUE(state.GetDepth(), "submesh index range buffer index", submesh.indexBufferRange.bufferIndex);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::START)
            {
                reader.ReadCount(dataHeader, submesh.indexBufferRange.startIndex, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::START.name));
                else
                    PRINT_VALUE(state.GetDepth(), "submesh index range start index", submesh.indexBufferRange.startIndex);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                reader.ReadCount(dataHeader, submesh.indexBufferRange.count, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                    PRINT_VALUE(state.GetDepth(), "submesh index range count", submesh.indexBufferRange.count);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for submesh index range.");
            return;
        }
    }

    //basePattern/[submeshes]/[submesh-index]/[vertex-buffer]
    AddVertexBufferMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_SUBMESHES_NOT_EMPTY(submesh, mesh->submeshes)
        },
        [getData](FinjinMesh::VertexBuffer*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_SUBMESHES_NOT_EMPTY(submesh, mesh->submeshes)

            result = &submesh.vertexBuffer;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SUBMESHES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::VERTEX_BUFFER
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for submesh vertex buffer.");
        return;
    }

    //basePattern/[submeshes]/[submesh-index]/[vertex-buffer-range]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBMESHES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::VERTEX_BUFFER_RANGE).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add submesh vertex buffer range pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBMESH VERTEX BUFFER RANGE", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_SUBMESHES_NOT_EMPTY(submesh, mesh->submeshes)

            if (propertyName == StandardAssetDocumentPropertyNames::BUFFER)
            {
                reader.ReadCount(dataHeader, submesh.vertexBufferRange.bufferIndex, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::BUFFER.name));
                else
                    PRINT_VALUE(state.GetDepth(), "submesh vertex range buffer index", submesh.vertexBufferRange.bufferIndex);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::START)
            {
                reader.ReadCount(dataHeader, submesh.vertexBufferRange.startIndex, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::START.name));
                else
                    PRINT_VALUE(state.GetDepth(), "submesh vertex range start index", submesh.vertexBufferRange.startIndex);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                reader.ReadCount(dataHeader, submesh.vertexBufferRange.count, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                    PRINT_VALUE(state.GetDepth(), "submesh vertex range count", submesh.vertexBufferRange.count);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for submesh vertex range.");
            return;
        }
    }

    //basePattern/[submeshes]/[submesh-index]/[vertex-bone-assignments]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SUBMESHES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::VERTEX_BONE_ASSIGNMENTS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add submesh vertex buffer vertex bone assignments pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBMESH VERTEX BONE ASSIGNMENTS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_SUBMESHES_NOT_EMPTY(submesh, mesh->submeshes)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!submesh.vertexBoneAssignments.Create(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate submesh vertex bone assignments.");
                    else
                        PRINT_VALUE(state.GetDepth(), "submesh vertex bone assignment count", value);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::VERTEX)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                state.bufferValuesRead += reader.ReadStridedUInt32s(dataHeader, &submesh.vertexBoneAssignments[state.bufferValuesRead].vertexIndex, submesh.vertexBoneAssignments.size() - state.bufferValuesRead, DataChunkReadStride(1, sizeof(FinjinMesh::Submesh::VertexBoneAssignment)), error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read vertex bone assignment vertex uint32 values.");
                else
                {
                    if (state.bufferValuesRead == submesh.vertexBoneAssignments.size())
                        PRINT_VALUES_ITEM(state.GetDepth(), "vertex bone assignment vertex values", submesh.vertexBoneAssignments, vertexIndex);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::BONE)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                state.bufferValuesRead += reader.ReadStridedUInt32s(dataHeader, &submesh.vertexBoneAssignments[state.bufferValuesRead].boneIndex, submesh.vertexBoneAssignments.size() - state.bufferValuesRead, DataChunkReadStride(1, sizeof(FinjinMesh::Submesh::VertexBoneAssignment)), error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read vertex bone assignment bone uint32 values.");
                else
                {
                    if (state.bufferValuesRead == submesh.vertexBoneAssignments.size())
                        PRINT_VALUES_ITEM(state.GetDepth(), "vertex bone assignment bone values", submesh.vertexBoneAssignments, boneIndex);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::WEIGHT)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    state.bufferValuesRead = 0;

                state.bufferValuesRead += reader.ReadStridedFloats(dataHeader, &submesh.vertexBoneAssignments[state.bufferValuesRead].weight, submesh.vertexBoneAssignments.size() - state.bufferValuesRead, DataChunkReadStride(1, sizeof(FinjinMesh::Submesh::VertexBoneAssignment)), error);
                if (error)
                    FINJIN_SET_ERROR(error, "Failed to read vertex bone assignment weight float values.");
                else
                {
                    if (state.bufferValuesRead == submesh.vertexBoneAssignments.size())
                        PRINT_VALUES_ITEM(state.GetDepth(), "vertex bone assignment weight values", submesh.vertexBoneAssignments, weight);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for submesh vertex bone assignments.");
            return;
        }
    }

    //basePattern/[skeleton]
    AddSkeletonMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }
        },
        [getData](AssetHandle<FinjinMeshSkeleton>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            result = &mesh->skeletonHandle;
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::SKELETON
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for skeleton.");
        return;
    }

    //basePattern/[morph-animations]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::MORPH_ANIMATIONS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add morph animations pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKELETON MORPH ANIMATIONS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!mesh->morphAnimationHandles.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate morph animations.");
                    else
                        PRINT_VALUE(state.GetDepth(), "skeleton morph animation count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for morph animations.");
            return;
        }
    }

    //basePattern/[morph-animations]/[animation-index]
    AddMorphAnimationMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                mesh->morphAnimationHandles.push_back();
            }
        },
        [getData, callbacks](AssetHandle<FinjinMesh::MorphAnimation>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            result = &mesh->morphAnimationHandles.back();
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::MORPH_ANIMATIONS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for morph animation.");
        return;
    }

    //basePattern/[poses]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add poses pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "POSES", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!mesh->poses.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate poses.");
                    else
                        PRINT_VALUE(state.GetDepth(), "pose count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for poses.");
            return;
        }
    }

    //basePattern/[poses]/[pose-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "POSE", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                mesh->poses.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            //auto& mesh = meshHandle->asset;
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for pose.");
            return;
        }
    }

    //basePattern/[poses]/[pose-index]/[subposes]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::SUBPOSES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subposes pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBPOSES", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_POSES_NOT_EMPTY(pose, mesh->poses)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!pose.subposes.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to pose subposes.");
                    else
                        PRINT_VALUE(state.GetDepth(), "subpose count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for poses.");
            return;
        }
    }

    //basePattern/[poses]/[pose-index]/[subposes]/[subpose-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::SUBPOSES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subpose pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBPOSE", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                CHECK_POSES_NOT_EMPTY(pose, mesh->poses)
                pose.subposes.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_POSES_NOT_EMPTY(pose, mesh->poses)
            CHECK_SUBPOSES_NOT_EMPTY(subpose, pose.subposes)

            if (propertyName == StandardAssetDocumentPropertyNames::SUBMESH)
            {
                reader.ReadCount(dataHeader, subpose.submeshIndex, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                    PRINT_VALUE(state.GetDepth(), "submesh index", subpose.submeshIndex);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for pose.");
            return;
        }
    }

    //basePattern/[poses]/[pose-index]/[subposes]/[subpose-index]/[format]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::SUBPOSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::FORMAT).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subpose format pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBPOSE FORMAT", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_POSES_NOT_EMPTY(pose, mesh->poses)
            CHECK_SUBPOSES_NOT_EMPTY(subpose, pose.subposes)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!subpose.formatElements.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate subpose format elements.");
                    else
                        PRINT_VALUE(state.GetDepth(), "format count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for subpose format.");
            return;
        }
    }

    //basePattern/[poses]/[pose-index]/[subposes]/[subpose-index]/[format]/[element-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::SUBPOSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::FORMAT, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subpose format element pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBPOSE FORMAT ELEMENT", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                CHECK_POSES_NOT_EMPTY(pose, mesh->poses)
                CHECK_SUBPOSES_NOT_EMPTY(subpose, pose.subposes)
                subpose.formatElements.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_POSES_NOT_EMPTY(pose, mesh->poses)
            CHECK_SUBPOSES_NOT_EMPTY(subpose, pose.subposes)
            CHECK_SUBPOSE_FORMAT_NOT_EMPTY(formatElement, subpose.formatElements)

            ReadFormatElement(formatElement, propertyName, reader, dataHeader, callbacks, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to read subpose format element.");
                return;
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for subpose format element.");
            return;
        }
    }

    //basePattern/[poses]/[pose-index]/[subposes]/[subpose-index]/[channels]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::SUBPOSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::CHANNELS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subpose channels pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBPOSE CHANNELS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_POSES_NOT_EMPTY(pose, mesh->poses)
            CHECK_SUBPOSES_NOT_EMPTY(subpose, pose.subposes)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!subpose.channels.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate subpose channels.");
                    else
                        PRINT_VALUE(state.GetDepth(), "subpose channels count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for subpose channels.");
            return;
        }
    }

    //basePattern/[poses]/[pose-index]/[subposes]/[subpose-index]/[channels]/[channel-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::SUBPOSES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::CHANNELS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose subpose channel pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBPOSE CHANNELS", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                CHECK_POSES_NOT_EMPTY(pose, mesh->poses)
                CHECK_SUBPOSES_NOT_EMPTY(subpose, pose.subposes)
                subpose.channels.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_POSES_NOT_EMPTY(pose, mesh->poses)
            CHECK_SUBPOSES_NOT_EMPTY(subpose, pose.subposes)
            CHECK_SUBPOSE_CHANNELS_NOT_EMPTY(channel, subpose.channels)

            if (propertyName == StandardAssetDocumentPropertyNames::VALUES)
            {
                ReadChannelValues(channel, reader, dataHeader, state, callbacks->GetSettings(), error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to read subpose channel.");
                    return;
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for subpose channel.");
            return;
        }
    }

    //basePattern/[pose-animations]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::POSE_ANIMATIONS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add pose animations pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "POSE ANIMATIONS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!mesh->poseAnimationHandles.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate pose animations.");
                    else
                        PRINT_VALUE(state.GetDepth(), "pose animation count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for pose animations.");
            return;
        }
    }

    //basePattern/[pose-animations]/[animation-index]
    AddPoseAnimationMappings<State>
        (
        callbacks,
        [getData](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                mesh->poseAnimationHandles.push_back();
            }
        },
        [getData, callbacks](AssetHandle<FinjinMesh::PoseAnimation>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            result = &mesh->poseAnimationHandles.back();
        },
        error,
        basePattern..., StandardAssetDocumentChunkNames::POSE_ANIMATIONS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for pose animation.");
        return;
    }

    //basePattern/[manual-lods]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::MANUAL_LODS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add manual LODs pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MANUAL LODS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!mesh->manualLods.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate manual LODs.");
                    else
                        PRINT_VALUE(state.GetDepth(), "manual LOD count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for manual LODs.");
            return;
        }
    }

    //basePattern/[manual-lods]/[manual-lod-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::MANUAL_LODS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add manual LOD pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MANUAL LOD", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMesh>* meshHandle = nullptr;
                getData(meshHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get mesh.");
                    return;
                }

                auto& mesh = meshHandle->asset;

                mesh->manualLods.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMesh>* meshHandle = nullptr;
            getData(meshHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get mesh.");
                return;
            }

            auto& mesh = meshHandle->asset;

            CHECK_MANUAL_LODS_NOT_EMPTY(manualLod, mesh->manualLods)

            if (propertyName == StandardAssetDocumentPropertyNames::MESH_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& meshRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, meshRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MESH_REF.name));
                else if (isValueDone)
                {
                    manualLod.meshHandle.assetRef.ForUriString(meshRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse mesh reference '%1%'.", meshRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "mesh ref", meshRef);
                        state.RecordAssetHandle(meshHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record mesh asset.");
                    }
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::DISTANCE)
            {
                reader.ReadFloat(dataHeader, manualLod.distance, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::DISTANCE.name));
                else
                    PRINT_VALUE(state.GetDepth(), "distance", manualLod.distance);
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for manual LOD.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddMaterialMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(AssetHandle<FinjinMaterial>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MATERIAL", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMaterial>* materialHandle = nullptr;
            getData(materialHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get material.");
                return;
            }

            if (propertyName == StandardAssetDocumentPropertyNames::MATERIAL_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& materialRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, materialRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MATERIAL_REF.name));
                else if (isValueDone)
                {
                    materialHandle->assetRef.ForUriString(materialRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse material reference '%1%'.", materialRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "material ref", materialRef);
                        state.RecordAssetHandle(materialHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to material asset.");
                    }
                }
            }
            else
            {
                auto& material = materialHandle->asset;

                if (propertyName == StandardAssetDocumentPropertyNames::TYPE)
                {
                    if (dataHeader.IsOnlyOrFirstOccurrence())
                        callbacks->tempString.clear();
                    auto& typeName = callbacks->tempString;
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, typeName, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TYPE.name));
                    else if (isValueDone)
                    {
                        if (callbacks->GetSettings().TranslateType(typeName).HasError())
                            FINJIN_SET_ERROR(error, "Failed to translate type.");
                        else
                        {
                            materialHandle->isOwner = true;

                            material = CreateInstanceOf<FinjinMaterial>(typeName, state.allocator, FINJIN_CALLER_ARGUMENTS, error);
                            if (error)
                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Unable to create object of type '%1%'.", typeName));
                            else
                                PRINT_VALUE(state.GetDepth(), "type", typeName);
                        }
                    }
                }
                else
                {
                    if (material == nullptr)
                        FINJIN_SET_ERROR(error, "Unable to read material properties. Material is null.");
                    else if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                    {
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, material->name, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                        else if (isValueDone)
                        {
                            PRINT_VALUE(state.GetDepth(), "name", material->name);

                            materialHandle->assetRef.objectName = material->name;

                            state.RecordAssetHandle(materialHandle, error);
                            if (error)
                                FINJIN_SET_ERROR(error, "Failed to record material asset.");
                        }
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::AMBIENT_COLOR)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, material->ambientColor.data(), 4, error) == 4;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::AMBIENT_COLOR.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::AMBIENT_COLOR.name));
                        else
                            PRINT_VALUE4(state.GetDepth(), "ambient color", material->ambientColor(0), material->ambientColor(1), material->ambientColor(2), material->ambientColor(3));
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::DIFFUSE_COLOR)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, material->diffuseColor.data(), 4, error) == 4;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::DIFFUSE_COLOR.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::DIFFUSE_COLOR.name));
                        else
                            PRINT_VALUE4(state.GetDepth(), "diffuse color", material->diffuseColor(0), material->diffuseColor(1), material->diffuseColor(2), material->diffuseColor(3));
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::SPECULAR_COLOR)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, material->specularColor.data(), 4, error) == 4;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::SPECULAR_COLOR.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::SPECULAR_COLOR.name));
                        else
                            PRINT_VALUE4(state.GetDepth(), "specular color", material->specularColor(0), material->specularColor(1), material->specularColor(2), material->specularColor(3));
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::SELF_ILLUMINATION_COLOR)
                    {
                        auto isValueDone = reader.ReadFloats(dataHeader, material->selfIlluminationColor.data(), 4, error) == 4;
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::SELF_ILLUMINATION_COLOR.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::SELF_ILLUMINATION_COLOR.name));
                        else
                            PRINT_VALUE4(state.GetDepth(), "self illum color", material->selfIlluminationColor(0), material->selfIlluminationColor(1), material->selfIlluminationColor(2), material->selfIlluminationColor(3));
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::SHININESS)
                    {
                        reader.ReadFloat(dataHeader, material->shininess, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::SHININESS.name));
                        else
                            PRINT_VALUE(state.GetDepth(), "shininess", material->shininess);
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::OPACITY)
                    {
                        reader.ReadFloat(dataHeader, material->opacity, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::OPACITY.name));
                        else
                            PRINT_VALUE(state.GetDepth(), "opacity", material->opacity);
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::CULL_MODE)
                    {
                        if (dataHeader.IsOnlyOrFirstOccurrence())
                            callbacks->tempString.clear();
                        auto& cullMode = callbacks->tempString;
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, cullMode, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::CULL_MODE.name));
                        else if (!isValueDone)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%' in a single read.", StandardAssetDocumentPropertyNames::CULL_MODE.name));
                        else
                        {
                            if (cullMode == StandardAssetDocumentPropertyValues::CullMode::NORMAL)
                                material->cullMode = FinjinCullMode::NORMAL;
                            else if (cullMode == StandardAssetDocumentPropertyValues::CullMode::NONE)
                                material->cullMode = FinjinCullMode::NONE;

                            PRINT_VALUE(state.GetDepth(), "cull mode", cullMode);
                        }
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::POLYGON_MODE)
                    {
                        if (dataHeader.IsOnlyOrFirstOccurrence())
                            callbacks->tempString.clear();
                        auto& polygonMode = callbacks->tempString;
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, polygonMode, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::POLYGON_MODE.name));
                        else if (isValueDone)
                        {
                            if (polygonMode == StandardAssetDocumentPropertyValues::PolygonMode::SOLID)
                                material->polygonMode = FinjinPolygonMode::SOLID;
                            else if (polygonMode == StandardAssetDocumentPropertyValues::PolygonMode::WIREFRAME)
                                material->polygonMode = FinjinPolygonMode::WIREFRAME;

                            PRINT_VALUE(state.GetDepth(), "polygon mode", polygonMode);
                        }
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::SHADING_MODE)
                    {
                        if (dataHeader.IsOnlyOrFirstOccurrence())
                            callbacks->tempString.clear();
                        auto& shadingMode = callbacks->tempString;
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, shadingMode, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::SHADING_MODE.name));
                        else if (isValueDone)
                        {
                            if (shadingMode == StandardAssetDocumentPropertyValues::ShadingMode::FLAT)
                                material->shadingMode = FinjinShadingMode::FLAT;
                            else if (shadingMode == StandardAssetDocumentPropertyValues::ShadingMode::SMOOTH)
                                material->shadingMode = FinjinShadingMode::SMOOTH;

                            PRINT_VALUE(state.GetDepth(), "shading mode", shadingMode);
                        }
                    }
                    else if (propertyName == StandardAssetDocumentPropertyNames::TRANSPARENCY)
                    {
                        if (dataHeader.IsOnlyOrFirstOccurrence())
                            callbacks->tempString.clear();
                        auto& transparency = callbacks->tempString;
                        auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, transparency, error);
                        if (error)
                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TRANSPARENCY.name));
                        else if (isValueDone)
                        {
                            if (transparency == StandardAssetDocumentPropertyValues::Transparency::NONE)
                                material->transparency = FinjinTransparency::NONE;
                            else if (transparency == StandardAssetDocumentPropertyValues::Transparency::ADDITIVE)
                                material->transparency = FinjinTransparency::ADDITIVE;
                            else if (transparency == StandardAssetDocumentPropertyValues::Transparency::SUBTRACTIVE)
                                material->transparency = FinjinTransparency::SUBTRACTIVE;
                            else if (transparency == StandardAssetDocumentPropertyValues::Transparency::FILTER)
                                material->transparency = FinjinTransparency::FILTER;

                            PRINT_VALUE(state.GetDepth(), "transparency", transparency);
                        }
                    }
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for material.");
            return;
        }
    }

    //basePattern/[maps]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::MAPS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add maps pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MATERIAL MAPS", isStart);
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMaterial>* materialHandle = nullptr;
            getData(materialHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get material.");
                return;
            }

            auto& material = materialHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!material->maps.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate maps.");
                    else
                        PRINT_VALUE(state.GetDepth(), "material map count", value);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for material maps.");
            return;
        }
    }

    //basePattern/[maps]/[map-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::MAPS, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add map pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [getData](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MATERIAL MAP", isStart);

            if (isStart)
            {
                AssetHandle<FinjinMaterial>* materialHandle = nullptr;
                getData(materialHandle, state, error);
                if (error)
                {
                    FINJIN_SET_ERROR(error, "Failed to get material.");
                    return;
                }

                auto& material = materialHandle->asset;

                material->maps.push_back();
            }
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinMaterial>* materialHandle = nullptr;
            getData(materialHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get material.");
                return;
            }

            auto& material = materialHandle->asset;

            CHECK_MATERIAL_MAPS_NOT_EMPTY(map, material->maps);

            if (propertyName == StandardAssetDocumentPropertyNames::TYPE)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, map.typeName, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TYPE.name));
                else if (isValueDone)
                {
                    if (callbacks->GetSettings().TranslateType(map.typeName).HasError())
                        FINJIN_SET_ERROR(error, "Failed to translate type.");
                    else
                        PRINT_VALUE(state.GetDepth(), "type", map.typeName);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::NAME)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, map.name, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "name", map.name);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::AMOUNT)
            {
                reader.ReadFloat(dataHeader, map.amount, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::AMOUNT.name));
                else
                    PRINT_VALUE(state.GetDepth(), "amount", textureRotation);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TEXTURE_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& textureRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, textureRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TEXTURE_REF.name));
                else if (isValueDone)
                {
                    map.textureHandle.assetRef.ForUriString(textureRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse texture reference '%1%'.", textureRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "texture ref", textureRef);

                        state.RecordAssetHandle(&map.textureHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record texture asset.");
                    }
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TEXTURE_COORDINATE_SET)
            {
                reader.ReadUInt8(dataHeader, map.textureCoordinateSet, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TEXTURE_COORDINATE_SET.name));
                else
                    PRINT_VALUE(state.GetDepth(), "texture coordinate set", map.textureCoordinateSet);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TEXTURE_ALPHA)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& textureAlpha = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, textureAlpha, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TEXTURE_ALPHA.name));
                else if (isValueDone)
                {
                    if (textureAlpha == StandardAssetDocumentPropertyValues::TextureAlpha::PREMULTIPLIED)
                        map.textureAlpha = FinjinTextureAlpha::PREMULTIPLIED;
                    else if (textureAlpha == StandardAssetDocumentPropertyValues::TextureAlpha::STANDARD)
                        map.textureAlpha = FinjinTextureAlpha::STANDARD;

                    PRINT_VALUE(state.GetDepth(), "texture alpha", textureAlpha);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TEXTURE_SCALE)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, map.textureScale.data(), 3, error) == 3;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::TEXTURE_SCALE.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::TEXTURE_SCALE.name));
                else
                    PRINT_VALUE3(state.GetDepth(), "texture scale", map.textureScale[0], map.textureScale[1], map.textureScale[2]);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TEXTURE_ROTATION)
            {
                float textureRotation;
                reader.ReadFloat(dataHeader, textureRotation, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::TEXTURE_ROTATION.name));
                else
                {
                    map.textureRotation = Radians(textureRotation);
                    PRINT_VALUE(state.GetDepth(), "texture rotation", textureRotation);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TEXTURE_OFFSET)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, map.textureOffset.data(), map.textureOffset.size(), error) == map.textureOffset.size();
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::TEXTURE_OFFSET.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::TEXTURE_OFFSET.name));
                else
                    PRINT_VALUE3(state.GetDepth(), "texture offset", map.textureOffset[0], map.textureOffset[1], map.textureOffset[2]);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TEXTURE_ADDRESS_MODE)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                {
                    for (auto& mode : callbacks->textureAddressMode)
                        mode.clear();
                }
                auto isValueDone = reader.ReadStrings(dataHeader, callbacks->textureAddressMode.data(), callbacks->textureAddressMode.size(), error) == callbacks->textureAddressMode.size();
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string array property '%1%'.", StandardAssetDocumentPropertyNames::TEXTURE_ADDRESS_MODE.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::TEXTURE_ADDRESS_MODE.name));
                else
                {
                    for (size_t modeIndex = 0; modeIndex < callbacks->textureAddressMode.size(); modeIndex++)
                    {
                        if (callbacks->textureAddressMode[modeIndex] == StandardAssetDocumentPropertyValues::TextureAddressMode::CLAMP)
                            map.textureAddressMode[modeIndex] = FinjinTextureAddressMode::CLAMP;
                        else if (callbacks->textureAddressMode[modeIndex] == StandardAssetDocumentPropertyValues::TextureAddressMode::MIRROR)
                            map.textureAddressMode[modeIndex] = FinjinTextureAddressMode::MIRROR;
                        else if (callbacks->textureAddressMode[modeIndex] == StandardAssetDocumentPropertyValues::TextureAddressMode::WRAP)
                            map.textureAddressMode[modeIndex] = FinjinTextureAddressMode::WRAP;
                    }

                    PRINT_VALUE3(state.GetDepth(), "texture address mode", callbacks->textureAddressMode[0], callbacks->textureAddressMode[1], callbacks->textureAddressMode[2]);
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for material map.");
            return;
        }
    }
}

template <typename State, typename Callbacks, typename HandleChunk, typename GetData, typename... BasePattern>
void AddPrefabMappings
    (
    Callbacks* callbacks,
    HandleChunk handleChunk, //std::function<void(bool, DataChunkReader&, State&, Error&)> handleChunk,
    GetData getData, //std::function<void(AssetHandle<FinjinPrefab>*&, State&, Error& error)> getData,
    Error& error,
    const BasePattern&... basePattern
    )
{
    FINJIN_ERROR_METHOD_START(error);

    //basePattern
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(basePattern...).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add base pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [handleChunk](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "PREFAB", isStart);

            handleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR(error, "Chunk handler failed.");
        };
        mapping.propertyCallback = [getData, callbacks](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinPrefab>* prefabHandle = nullptr;
            getData(prefabHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get prefab.");
                return;
            }

            auto& prefab = prefabHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::PREFAB_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    callbacks->tempString.clear();
                auto& prefabRef = callbacks->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, prefabRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::PREFAB_REF.name));
                else if (isValueDone)
                {
                    prefabHandle->assetRef.ForUriString(prefabRef, callbacks->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse prefab reference '%1%'.", prefabRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "prefab ref", prefabRef);

                        state.RecordAssetHandle(prefabHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record prefab asset.");
                    }
                }
            }
            else
            {
                if (prefab == nullptr)
                {
                    prefabHandle->isOwner = true;

                    prefab = AllocatedClass::New<FinjinPrefab>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                    if (prefab == nullptr)
                    {
                        FINJIN_SET_ERROR(error, "Failed to allocate prefab.");
                        return;
                    }
                }

                if (propertyName == StandardAssetDocumentPropertyNames::NAME)
                {
                    auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, prefab->name, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                    else if (isValueDone)
                    {
                        PRINT_VALUE(state.GetDepth(), "prefab name", prefab->name);

                        prefabHandle->assetRef.objectName = prefab->name;

                        state.RecordAssetHandle(prefabHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record prefab asset.");
                    }
                }
            }
        };
        callbacks->AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for prefab.");
            return;
        }
    }

    //basePattern/[scene-nodes]
    AddSceneNodesMappings<State>
        (
        callbacks,
        [getData, callbacks](DynamicVector<FinjinSceneNode*>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            AssetHandle<FinjinPrefab>* prefabHandle = nullptr;
            getData(prefabHandle, state, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, "Failed to get prefab.");
                return;
            }

            auto& prefab = prefabHandle->asset;

            result = &prefab->sceneNodes;
        },
        error,
        basePattern...
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for prefab scene nodes.");
        return;
    }
}


//Implementation----------------------------------------------------------------

//FinjinCommonDataChunkReaderCallbacksState
FinjinCommonDataChunkReaderCallbacksState::FinjinCommonDataChunkReaderCallbacksState(FinjinCommonDataChunkReaderCallbacksState* parentState)
{
    this->parentState = parentState;

    this->allocator = nullptr;

    this->controller = nullptr;

    this->createOptimizedLookups = true;
}

FinjinCommonDataChunkReaderCallbacksState::~FinjinCommonDataChunkReaderCallbacksState()
{
    Destroy();
}

void FinjinCommonDataChunkReaderCallbacksState::Create(const FinjinCommonDataChunkReaderCallbacksStateSettings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Destroy();

    if (this->parentState == nullptr)
    {
        if (settings.maxFileCount == 0)
        {
            FINJIN_SET_ERROR(error, "The maximum file count must be greater than 0.");
            return;
        }

        auto allZero = true;
        for (size_t assetClass = 0; assetClass < settings.assetCounts.size(); assetClass++)
        {
            if (settings.assetCounts[assetClass] > 0)
            {
                allZero = false;

                if (!this->assetBucketsByClass[assetClass].assetHandlesByName.Create(settings.assetCounts[assetClass], settings.assetCounts[assetClass], settings.allocator, settings.allocator))
                {
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create assets by name lookup for '%1%'.", AssetClassUtilities::ToString(assetClass)));
                    return;
                }
            }
        }

        if (allZero)
        {
            FINJIN_SET_ERROR(error, "Failed to create any asset lookups. All counts were 0.");
            return;
        }

        if (!this->encounteredFilePaths.Create(settings.cacheFullFilePaths, settings.maxFileCount, settings.allocator))
        {
            FINJIN_SET_ERROR(error, "Failed to create encountered paths set.");
            return;
        }

        if (settings.createFilesToLoadQueue)
        {
            if (!this->filesToLoad.Create(settings.maxFileCount, settings.allocator))
            {
                FINJIN_SET_ERROR(error, "Failed to create files to load array.");
                return;
            }
        }
    }

    this->allocator = settings.allocator;

    this->createOptimizedLookups = settings.createOptimizedLookups;
}

void FinjinCommonDataChunkReaderCallbacksState::Destroy()
{
    for (size_t assetClass = 0; assetClass < this->assetBucketsByClass.size(); assetClass++)
        this->assetBucketsByClass[assetClass].assetHandlesByName.Destroy();

    this->encounteredFilePaths.Destroy();

    this->filesToLoad.Destroy();

    for (size_t assetClass = 0; assetClass < this->assetBucketsByClass.size(); assetClass++)
    {
        auto& allocatedHandleHead = this->assetBucketsByClass[assetClass].allocatedHead;

        for (auto allocatedHandle = allocatedHandleHead; allocatedHandle != nullptr; )
        {
            auto next = allocatedHandle->allocatedNext;

            switch (static_cast<AssetClass>(assetClass))
            {
                case AssetClass::SCENE:
                {
                    reinterpret_cast<AssetHandle<FinjinScene>*>(allocatedHandle)->Destroy();
                    break;
                }
                case AssetClass::MESH:
                {
                    reinterpret_cast<AssetHandle<FinjinMesh>*>(allocatedHandle)->Destroy();
                    break;
                }
                case AssetClass::SKELETON:
                {
                    reinterpret_cast<AssetHandle<FinjinMeshSkeleton>*>(allocatedHandle)->Destroy();
                    break;
                }
                case AssetClass::MATERIAL:
                {
                    reinterpret_cast<AssetHandle<FinjinMaterial>*>(allocatedHandle)->Destroy();
                    break;
                }
                case AssetClass::PREFAB:
                {
                    reinterpret_cast<AssetHandle<FinjinPrefab>*>(allocatedHandle)->Destroy();
                    break;
                }
                case AssetClass::NODE_ANIMATION:
                {
                    reinterpret_cast<AssetHandle<FinjinNodeAnimation>*>(allocatedHandle)->Destroy();
                    break;
                }
                case AssetClass::SKELETON_ANIMATION:
                {
                    reinterpret_cast<AssetHandle<FinjinMeshSkeleton>*>(allocatedHandle)->Destroy();
                    break;
                }
                case AssetClass::MORPH_ANIMATION:
                {
                    reinterpret_cast<AssetHandle<FinjinMeshMorphAnimation>*>(allocatedHandle)->Destroy();
                    break;
                }
                case AssetClass::POSE_ANIMATION:
                {
                    reinterpret_cast<AssetHandle<FinjinMeshPoseAnimation>*>(allocatedHandle)->Destroy();
                    break;
                }
                default: break;
            }

            this->allocator->Deallocate(allocatedHandle);

            allocatedHandle->allocatedNext = nullptr;
            allocatedHandle = next;
        }

        allocatedHandleHead = nullptr;
    }

    this->createOptimizedLookups = true;
}

FinjinCommonDataChunkReaderCallbacksState* FinjinCommonDataChunkReaderCallbacksState::GetRootState()
{
    auto rootState = this;
    while (rootState->parentState != nullptr)
        rootState = rootState->parentState;
    return rootState;
}

const FinjinCommonDataChunkReaderCallbacksState* FinjinCommonDataChunkReaderCallbacksState::GetRootState() const
{
    auto rootState = this;
    while (rootState->parentState != nullptr)
        rootState = rootState->parentState;
    return rootState;
}

void FinjinCommonDataChunkReaderCallbacksState::HandleEncounteredAssetFilePath(const AssetReference& assetRef, AssetClass assetClass, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    auto rootState = GetRootState();

    auto addResult = rootState->encounteredFilePaths.insert(assetRef.filePath);
    if (addResult.HasError())
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add asset file '%1%' (%2%) to encountered set.", assetRef.filePath, AssetClassUtilities::ToString(assetClass)));
        return;
    }

    if (addResult.value)
    {
        //File was added (it hasn't been encountered)

        auto handledAsset = false;
        if (rootState->controller != nullptr)
        {
            handledAsset = rootState->controller->HandleNewAsset(assetRef, assetClass, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Controller failed to handle asset file '%1%' (%2%).", assetRef.filePath, AssetClassUtilities::ToString(assetClass)));
                return;
            }
        }

        //This should probably add the file to the asset reader queue, not this generic list
        if (!handledAsset && rootState->filesToLoad.push(assetRef.filePath).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add asset file '%1%' (%2%) to queue.", assetRef.filePath, AssetClassUtilities::ToString(assetClass)));
            return;
        }
    }
}

ValueOrError<bool> FinjinCommonDataChunkReaderCallbacksState::AddEncounteredFilePath(const Path& filePath)
{
    return GetRootState()->encounteredFilePaths.insert(filePath);
}

size_t FinjinCommonDataChunkReaderCallbacksState::GetEncounteredFileCount() const
{
    return GetRootState()->encounteredFilePaths.size();
}

UserDataClass* FinjinCommonDataChunkReaderCallbacksState::GetUserDataClass(const Utf8String& name)
{
    auto& assetBucket = GetRootState()->assetBucketsByClass[AssetClass::USER_DATA_TYPES];
    for (auto& assetNameAndHandle : assetBucket.assetHandlesByName)
    {
        if (assetNameAndHandle.second->isOwner)
        {
            auto assetHandle = (AssetHandle<UserDataTypes>*)assetNameAndHandle.second;
            auto userDataClass = assetHandle->asset->GetClass(name);
            if (userDataClass != nullptr)
                return userDataClass;
        }
    }

    return nullptr;
}

void FinjinCommonDataChunkReaderCallbacksState::RecordAssetHandle(AssetHandle<void>* assetHandle, AssetClass assetClass, bool requiresInternalDependencyResolution, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    assert(!assetHandle->isOwner || assetHandle->asset != nullptr);
    if (assetHandle->isOwner && assetHandle->asset == nullptr)
    {
        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Asset '%1%' (%2%) is in an unexpected state. It is as an owner but doesn't have an actual asset.", assetHandle->assetRef.objectName, AssetClassUtilities::ToString(assetClass)));
        return;
    }

    auto rootState = GetRootState();

    auto& assetBucket = rootState->assetBucketsByClass[assetClass];

    auto assetByNameFoundAt = assetBucket.assetHandlesByName.find(assetHandle->assetRef.objectName);
    if (assetByNameFoundAt != assetBucket.assetHandlesByName.end())
    {
        //Asset reference already exists
        auto foundAssetHandle = assetByNameFoundAt->second;

        if (assetHandle->isOwner && foundAssetHandle->isOwner)
        {
            //Both are owners. This should probably be an error or reported back to the caller
            if (rootState->controller != nullptr)
            {
                switch (rootState->controller->ResolveAssetCollision(foundAssetHandle, assetClass, assetHandle))
                {
                    case AssetCollisionResolution::GENERATE_ERROR:
                    {
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Encountered a duplicate asset name '%1%' (%2%).", assetHandle->assetRef.objectName, AssetClassUtilities::ToString(assetClass)));
                        return;
                    }
                    case AssetCollisionResolution::KEEP_EXISTING:
                    {
                        break;
                    }
                    case AssetCollisionResolution::KEEP_NEW:
                    {
                        foundAssetHandle->asset = assetHandle->asset;
                        break;
                    }
                }
            }
        }
        else if (assetHandle->isOwner)
        {
            //The asset being passed in is an owner asset

            //Update and remove all other references from linked list
            for (auto dependentHandle = foundAssetHandle; dependentHandle != nullptr; )
            {
                dependentHandle->asset = assetHandle->asset;

                auto next = dependentHandle->referenceNext;
                dependentHandle->referenceNext = nullptr;
                dependentHandle = next;
            }

            //Replace the looked up asset handle's asset with the one passed in
            assetByNameFoundAt->second = assetHandle;
        }
        else if (foundAssetHandle->isOwner)
        {
            //The looked up asset is an owner. Update passed in asset
            assetHandle->asset = foundAssetHandle->asset;
        }
        else
        {
            //Neither the passed in asset or the ones in the lookup are owner assets. Add to beginning of reference list
            assetHandle->referenceNext = foundAssetHandle;
            assetByNameFoundAt->second = assetHandle;
        }
    }
    else
    {
        //Asset does not exist

        //Add it to lookup
        if (assetBucket.assetHandlesByName.insert(assetHandle->assetRef.objectName, assetHandle, false).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to insert asset '%1%' (%2%) into lookup.", assetHandle->assetRef.objectName, AssetClassUtilities::ToString(assetClass)));
            return;
        }

        //Add file to queue if it hasn't already been encountered
        if (!assetHandle->assetRef.filePath.empty())
        {
            HandleEncounteredAssetFilePath(assetHandle->assetRef, assetClass, error);
            if (error)
            {
                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to process asset reference '%1%' (%2%).", assetHandle->assetRef.objectName, AssetClassUtilities::ToString(assetClass)));
                return;
            }
        }
    }

    if (assetHandle->isOwner)
    {
        assetBucket.newAssetHandles.push_back(assetHandle);

        if (requiresInternalDependencyResolution)
        {
            assetHandle->internalDependencyNext = assetBucket.internalDependencyHead;
            assetBucket.internalDependencyHead = assetHandle;
        }
    }
}

//FinjinSceneReaderState
FinjinSceneReaderState::FinjinSceneReaderState(FinjinCommonDataChunkReaderCallbacksState* parentState) :
    Super(parentState),
    sceneCallbacksState(this),
    meshCallbacksState(this),
    skeletonCallbacksState(this),
    materialCallbacksState(this),
    prefabCallbacksState(this),
    nodeAnimationCallbacksState(this),
    skeletonAnimationCallbacksState(this),
    morphAnimationCallbacksState(this),
    poseAnimationCallbacksState(this),
    handwrittenUserDataTypesReaderState(this),
    handwrittenStringsReaderState(this),
    textureReaderState(this),
    soundReaderState(this)
{
}

void FinjinSceneReaderState::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    this->sceneCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create scene callback state.");
        return;
    }

    this->meshCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create mesh callback state.");
        return;
    }

    this->skeletonCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create skeleton callback state.");
        return;
    }

    this->materialCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create material callback state.");
        return;
    }

    this->prefabCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create prefab callback state.");
        return;
    }

    this->nodeAnimationCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create node animation callback state.");
        return;
    }

    this->skeletonAnimationCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create skeleton animation callback state.");
        return;
    }

    this->morphAnimationCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create morph animation callback state.");
        return;
    }

    this->poseAnimationCallbacksState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create pose animation callback state.");
        return;
    }

    this->textureReaderState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create texture reader state.");
        return;
    }

    this->soundReaderState.Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to create sound reader state.");
        return;
    }
}

void FinjinSceneReaderState::Destroy()
{
    Super::Destroy();
    this->sceneCallbacksState.Destroy();
    this->meshCallbacksState.Destroy();
    this->skeletonCallbacksState.Destroy();
    this->materialCallbacksState.Destroy();
    this->prefabCallbacksState.Destroy();
    this->nodeAnimationCallbacksState.Destroy();
    this->skeletonAnimationCallbacksState.Destroy();
    this->morphAnimationCallbacksState.Destroy();
    this->poseAnimationCallbacksState.Destroy();
    this->textureReaderState.Destroy();
    this->soundReaderState.Destroy();
}

//FinjinMeshDataChunkReaderCallbacks
void FinjinMeshDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinMesh, State> meshHandleChunk;
    static NonEmbeddedAssetGetData<FinjinMesh, State> meshHandleProperty;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[mesh]
    AddMeshMappings<State>(this, meshHandleChunk, meshHandleProperty, error, StandardAssetDocumentChunkNames::MESH);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for mesh.");
        return;
    }
}

//FinjinMeshSkeletonDataChunkReaderCallbacks
void FinjinMeshSkeletonDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinMeshSkeleton, State> skeletonHandleChunk;
    static NonEmbeddedAssetGetData<FinjinMeshSkeleton, State> skeletonHandleProperty;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[skeleton]
    AddSkeletonMappings<State>(this, skeletonHandleChunk, skeletonHandleProperty, error, StandardAssetDocumentChunkNames::SKELETON);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mappings for skeleton.");
        return;
    }
}

//FinjinMaterialDataChunkReaderCallbacks
void FinjinMaterialDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinMaterial, State> materialHandleChunk;
    static NonEmbeddedAssetGetData<FinjinMaterial, State> materialHandleProperty;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[material]
    AddMaterialMappings<State>(this, materialHandleChunk, materialHandleProperty, error, StandardAssetDocumentChunkNames::MATERIAL);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for material.");
        return;
    }
}

//FinjinNodeAnimationDataChunkReaderCallbacks
void FinjinNodeAnimationDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinNodeAnimation, State> nodeAnimationHandleChunk;
    static NonEmbeddedAssetGetData<FinjinNodeAnimation, State> nodeAnimationHandleProperty;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[node-animation]
    AddNodeAnimationMappings<State>(this, nodeAnimationHandleChunk, nodeAnimationHandleProperty, error, StandardAssetDocumentChunkNames::NODE_ANIMATION);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for node animation.");
        return;
    }
}

//FinjinSkeletonAnimationDataChunkReaderCallbacks
void FinjinSkeletonAnimationDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinMeshSkeletonAnimation, State> skeletonAnimationHandleChunk;
    static NonEmbeddedAssetGetData<FinjinMeshSkeletonAnimation, State> skeletonAnimationHandleProperty;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[skeleton-animation]
    AddSkeletonAnimationMappings<State>(this, skeletonAnimationHandleChunk, skeletonAnimationHandleProperty, error, StandardAssetDocumentChunkNames::SKELETON_ANIMATION);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for skeleton animation.");
        return;
    }
}

//FinjinMorphAnimationDataChunkReaderCallbacks
void FinjinMorphAnimationDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinMeshMorphAnimation, State> morphAnimationHandleChunk;
    static NonEmbeddedAssetGetData<FinjinMeshMorphAnimation, State> morphAnimationHandleProperty;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[morph-animation]
    AddMorphAnimationMappings<State>(this, morphAnimationHandleChunk, morphAnimationHandleProperty, error, StandardAssetDocumentChunkNames::MORPH_ANIMATION);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for morph animation.");
        return;
    }
}

//FinjinPoseAnimationDataChunkReaderCallbacks
void FinjinPoseAnimationDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinMeshPoseAnimation, State> poseAnimationHandleChunk;
    static NonEmbeddedAssetGetData<FinjinMeshPoseAnimation, State> poseAnimationHandleProperty;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[pose-animation]
    AddPoseAnimationMappings<State>(this, poseAnimationHandleChunk, poseAnimationHandleProperty, error, StandardAssetDocumentChunkNames::POSE_ANIMATION);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for pose animation.");
        return;
    }
}

//FinjinPrefabDataChunkReaderCallbacks
void FinjinPrefabDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinPrefab, State> prefabHandleChunk;
    static NonEmbeddedAssetGetData<FinjinPrefab, State> prefabHandleProperty;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[prefab]
    AddPrefabMappings<State>(this, prefabHandleChunk, prefabHandleProperty, error, StandardAssetDocumentChunkNames::PREFAB);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for prefab.");
        return;
    }
}

FinjinPrefabDataChunkReaderCallbacks::Settings& FinjinPrefabDataChunkReaderCallbacks::GetSettings()
{
    return this->settings;
}

//FinjinSceneDataChunkReaderCallbacks
void FinjinSceneDataChunkReaderCallbacks::Create(const Settings& settings, Error& error)
{
    static NonEmbeddedAssetHandleChunk<FinjinScene, State> sceneHandleChunk;

    FINJIN_ERROR_METHOD_START(error);

    this->settings = settings;

    Super::Create(settings, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    //[scene]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SCENE", isStart);

            sceneHandleChunk(isStart, reader, state, error);
            if (error)
                FINJIN_SET_ERROR_NO_MESSAGE(error);
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (scene == nullptr)
            {
                sceneHandle->isOwner = true;

                scene = AllocatedClass::New<FinjinScene>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                if (scene == nullptr)
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate scene.");
                    return;
                }
            }

            if (propertyName == StandardAssetDocumentPropertyNames::NAME)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, scene->name, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                else if (isValueDone)
                {
                    PRINT_VALUE(state.GetDepth(), "scene name", scene->name);

                    sceneHandle->assetRef.objectName = scene->name;

                    state.RecordAssetHandle(sceneHandle, error);
                    if (error)
                        FINJIN_SET_ERROR(error, "Failed to record scene asset.");
                }
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for scene.");
            return;
        }
    }

    //[scene]/[prefabs]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::PREFABS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add prefabs pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "PREFABS", isStart);
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!scene->prefabHandles.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate prefabs.");
                    else
                        PRINT_VALUE(state.GetDepth(), "prefab count", value);
                }
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for prefabs.");
            return;
        }
    }

    //[scene]/[prefabs]/[prefab-index]
    AddPrefabMappings<State>
        (
        this,
        [](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (isStart)
                scene->prefabHandles.push_back();
        },
        [](AssetHandle<FinjinPrefab>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_PREFABS_NOT_EMPTY(prefab, scene->prefabHandles)

            result = &prefab;
        },
        error,
        StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::PREFABS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for prefab.");
        return;
    }

    //[scene]/[subscenes]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add subscenes pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBSCENES", isStart);
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::UNITS_PER_METER)
            {
                reader.ReadFloat(dataHeader, scene->unitsPerMeter, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::UNITS_PER_METER.name));
                else
                    PRINT_VALUE(state.GetDepth(), "units per meter", scene->unitsPerMeter);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!scene->subscenes.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate scenes.");
                    else
                        PRINT_VALUE(state.GetDepth(), "scene count", value);
                }
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for subscenes.");
            return;
        }
    }

    //[scene]/[subscenes]/[subscene-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add subscene pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SUBSCENE", isStart);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (isStart)
                scene->subscenes.push_back();
        };
        mapping.propertyCallback = [](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)

            if (propertyName == StandardAssetDocumentPropertyNames::ID)
            {
                reader.ReadUuid(dataHeader, subscene.id, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read UUID property '%1%'.", StandardAssetDocumentPropertyNames::ID.name));
                else
                    PRINT_VALUE(state.GetDepth(), "id", subscene.id);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::SCENE_MANAGER)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, subscene.sceneManager, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::SCENE_MANAGER.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "scene manager", subscene.sceneManager);
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for subscene.");
            return;
        }
    }

    //[scene]/[subscenes]/[subscene-index]/[environment]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::ENVIRONMENT).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene environment pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SCENE ENVIRONMENT", isStart);
        };
        mapping.propertyCallback = [](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)

            if (propertyName == StandardAssetDocumentPropertyNames::AMBIENT_COLOR)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, subscene.environment.ambientLight.color.data(), 4, error) == 4;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::AMBIENT_COLOR.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::AMBIENT_COLOR.name));
                else
                    PRINT_VALUE4(state.GetDepth(), "ambient color", subscene.environment.ambientColor(0), subscene.environment.ambientColor(1), subscene.environment.ambientColor(2), subscene.environment.ambientColor(3));
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::BACKGROUND_COLOR)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, subscene.environment.backgroundColor.data(), 4, error) == 4;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::BACKGROUND_COLOR.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::BACKGROUND_COLOR.name));
                else
                    PRINT_VALUE4(state.GetDepth(), "background color", subscene.environment.backgroundColor(0), subscene.environment.backgroundColor(1), subscene.environment.backgroundColor(2), subscene.environment.backgroundColor(3));
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::RANGE)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, subscene.environment.range, 2, error) == 2;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::RANGE.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::RANGE.name));
                else
                    PRINT_VALUE2(state.GetDepth(), "range", subscene.environment.range[0], subscene.environment.range[1]);
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for environment.");
            return;
        }
    }

    //[scene]/[subscenes]/[subscene-index]/[environment]/[fog]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::ENVIRONMENT, StandardAssetDocumentChunkNames::FOG).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene environment fog pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SCENE ENVIRONMENT FOG", isStart);
        };
        mapping.propertyCallback = [](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)

            if (propertyName == StandardAssetDocumentPropertyNames::MODE)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, subscene.environment.fog.mode, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MODE.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "mode", subscene.environment.fog.mode);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::EXP_DENSITY)
            {
                reader.ReadFloat(dataHeader, subscene.environment.fog.expDensity, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::EXP_DENSITY.name));
                else
                    PRINT_VALUE(state.GetDepth(), "exp density", subscene.environment.fog.expDensity);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::RANGE)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, subscene.environment.fog.range, 2, error) == 2;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::RANGE.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::RANGE.name));
                else
                    PRINT_VALUE2(state.GetDepth(), "linear range", subscene.environment.fog.range[0], subscene.environment.fog.range[1]);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::COLOR)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, subscene.environment.fog.color.data(), 4, error) == 4;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::COLOR.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::COLOR.name));
                else
                    PRINT_VALUE4(state.GetDepth(), "color", subscene.environment.fog.color(0), subscene.environment.fog.color(1), subscene.environment.fog.color(2), subscene.environment.fog.color(3));
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for fog.");
            return;
        }
    }

    //[scene]/[subscenes]/[subscene-index]/[environment]/[shadows]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::ENVIRONMENT, StandardAssetDocumentChunkNames::SHADOWS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene environment shadows pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SCENE ENVIRONMENT SHADOWS", isStart);
        };
        mapping.propertyCallback = [](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)

            if (propertyName == StandardAssetDocumentPropertyNames::TECHNIQUE)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, subscene.environment.shadows.technique, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TECHNIQUE.name));
                else if (isValueDone)
                    PRINT_VALUE(state.GetDepth(), "technique", subscene.environment.shadows.technique);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::FAR_DISTANCE)
            {
                reader.ReadFloat(dataHeader, subscene.environment.shadows.farDistance, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float property '%1%'.", StandardAssetDocumentPropertyNames::FAR_DISTANCE.name));
                else
                    PRINT_VALUE(state.GetDepth(), "far distance", subscene.environment.shadows.farDistance);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::COLOR)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, subscene.environment.shadows.color.data(), 4, error) == 4;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::COLOR.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::COLOR.name));
                else
                    PRINT_VALUE4(state.GetDepth(), "color", subscene.environment.shadows.color(0), subscene.environment.shadows.color(1), subscene.environment.shadows.color(2), subscene.environment.shadows.color(3));
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for shadows.");
            return;
        }
    }

    //[scene]/[subscenes]/[subscene-index]/[environment]/[sky-nodes]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::ENVIRONMENT, StandardAssetDocumentChunkNames::SKY_NODES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene environment sky nodes pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SKY NODES", isStart);
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!subscene.environment.skyNodes.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate scene environment sky nodes.");
                    else
                        PRINT_VALUE(state.GetDepth(), "sky node count", value);
                }
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for sky nodes.");
            return;
        }
    }

    //[scene]/[subscenes]/[subscene-index]/[environment]/[sky-nodes]/[sky-node-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::ENVIRONMENT, StandardAssetDocumentChunkNames::SKY_NODES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add scene environment sky node pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "SCENE ENVIRONMENT SKY", isStart);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (isStart)
            {
                CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)
                subscene.environment.skyNodes.push_back();
            }
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)
            CHECK_SKY_NODES_NOT_EMPTY(skyNode, subscene.environment.skyNodes)

            if (propertyName == StandardAssetDocumentPropertyNames::TYPE)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, skyNode.typeName, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::TYPE.name));
                else if (isValueDone)
                {
                    if (this->settings.TranslateType(skyNode.typeName).HasError())
                        FINJIN_SET_ERROR(error, "Failed to translate type.");
                    else
                        PRINT_VALUE(state.GetDepth(), "type", skyNode.typeName);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::TRANSFORM)
            {
                auto isValueDone = reader.ReadFloats(dataHeader, skyNode.transform.data(), 16, error) == 16;
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%'.", StandardAssetDocumentPropertyNames::TRANSFORM.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read float array property '%1%' in a single read.", StandardAssetDocumentPropertyNames::TRANSFORM.name));
                else
                    PRINT_TRANSFORM(state.GetDepth(), skyNode.transform);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::ENABLE)
            {
                reader.ReadBool(dataHeader, skyNode.enable, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read boolean property '%1%'.", StandardAssetDocumentPropertyNames::ENABLE.name));
                else
                    PRINT_VALUE(state.GetDepth(), "enable", (int)skyNode.enable);
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::MESH_REF)
            {
                if (dataHeader.IsOnlyOrFirstOccurrence())
                    this->tempString.clear();
                auto& meshRef = this->tempString;
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, meshRef, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::MESH_REF.name));
                else if (!isValueDone)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%' in a single read.", StandardAssetDocumentPropertyNames::MESH_REF.name));
                else
                {
                    skyNode.meshHandle.assetRef.ForUriString(meshRef, this->tempSimpleUri, error);
                    if (error)
                        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to parse mesh reference '%1%'.", meshRef));
                    else
                    {
                        PRINT_VALUE(state.GetDepth(), "mesh ref", meshRef);

                        state.RecordAssetHandle(&skyNode.meshHandle, error);
                        if (error)
                            FINJIN_SET_ERROR(error, "Failed to record mesh asset.");
                    }
                }
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for sky node.");
            return;
        }
    }

    //[scene]/[subscenes]/[subscene-index]/[environment]/[sky-nodes]/[sky-node-index]/[node-animations]
    AddNodeAnimationsMappings<State>
        (
        this,
        [](DynamicVector<AssetHandle<FinjinNodeAnimation> >*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)
            CHECK_SKY_NODES_NOT_EMPTY(skyNode, subscene.environment.skyNodes)

            result = &skyNode.nodeAnimationHandles;
        },
        error,
        StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES, (ChunkName::Index)-1, StandardAssetDocumentChunkNames::ENVIRONMENT, StandardAssetDocumentChunkNames::SKY_NODES, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for sky node animations.");
        return;
    }

    //[scene]/[subscenes]/[subscene-index]/[scene-nodes]
    AddSceneNodesMappings<State>
        (
        this,
        [](DynamicVector<FinjinSceneNode*>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_SUBSCENES_NOT_EMPTY(subscene, scene->subscenes)

            result = &subscene.sceneNodes;
        },
        error,
        StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::SUBSCENES, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for scene nodes.");
        return;
    }

    //[scene]/[materials]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::MATERIALS).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add materials pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MATERIALS", isStart);
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!scene->materialHandles.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate materials.");
                    else
                        PRINT_VALUE(state.GetDepth(), "material count", value);
                }
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for materials.");
            return;
        }
    }

    //[scene]/[materials]/[material-index]
    AddMaterialMappings<State>
        (
        this,
        [](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (isStart)
                scene->materialHandles.push_back();
        },
        [](AssetHandle<FinjinMaterial>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_MATERIALS_NOT_EMPTY(material, scene->materialHandles)

            result = &material;
        },
        error,
        StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::MATERIALS, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for material.");
        return;
    }

    //[scene]/[textures]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::TEXTURES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add textures pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "TEXTURES", isStart);
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!scene->textureHandles.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate textures.");
                    else
                        PRINT_VALUE(state.GetDepth(), "texture count", value);
                }
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for textures.");
            return;
        }
    }

    //[scene]/[textures]/[texture-index]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::TEXTURES, (ChunkName::Index)-1).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add texture pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "TEXTURE", isStart);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (isStart)
            {
                scene->textureHandles.push_back();
            }
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_TEXTURES_NOT_EMPTY(textureHandle, scene->textureHandles)
            auto& texture = textureHandle.asset;

            if (texture == nullptr)
            {
                textureHandle.isOwner = true;

                texture = AllocatedClass::New<FinjinTexture>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                if (texture == nullptr)
                {
                    FINJIN_SET_ERROR(error, "Failed to allocate texture.");
                    return;
                }
            }

            if (propertyName == StandardAssetDocumentPropertyNames::NAME)
            {
                auto isValueDone = ReadStringWithLengthHint(reader, dataHeader, state, texture->name, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read string property '%1%'.", StandardAssetDocumentPropertyNames::NAME.name));
                else if (isValueDone)
                {
                    PRINT_VALUE(state.GetDepth(), "name", textureHandle.assetRef.objectName);

                    textureHandle.assetRef.objectName = texture->name;

                    state.RecordAssetHandle(&textureHandle, error);
                    if (error)
                        FINJIN_SET_ERROR(error, "Failed to record texture asset.");
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::CONTENT_SIZE)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!texture->fileBytes.CreateEmpty(value, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate texture bytes.");
                    else
                        PRINT_VALUE(state.GetDepth(), "byte count", value);
                }
            }
            else if (propertyName == StandardAssetDocumentPropertyNames::CONTENT)
            {
                auto bytesRead = reader.ReadBlob(dataHeader, texture->fileBytes.data_left(), texture->fileBytes.size_left(), error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read blob property '%1%'.", StandardAssetDocumentPropertyNames::CONTENT.name));
                else
                    texture->fileBytes.push_back_count(bytesRead);
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for sky node.");
            return;
        }
    }

    //[scene]/[meshes]
    {
        DataChunkReaderCallbacksChunkMapping<State> mapping;
        if (mapping.pattern.Add(StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::MESHES).HasErrorOrValue(false))
        {
            FINJIN_SET_ERROR(error, "Failed to add meshes pattern to mapping patterns.");
            return;
        }
        mapping.chunkCallback = [](bool isStart, const ChunkName* chunkName, const ParsedChunkName& parsedChunkName, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            PRINT_CHUNK(state.GetDepth(), "MESHES", isStart);
        };
        mapping.propertyCallback = [this](const ChunkName* chunkName, const ParsedChunkName* parsedChunkName, ParsedChunkPropertyName& propertyName, DataChunkReader& reader, DataHeader& dataHeader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (propertyName == StandardAssetDocumentPropertyNames::COUNT)
            {
                size_t value;
                reader.ReadCount(dataHeader, value, error);
                if (error)
                    FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to read count property '%1%'.", StandardAssetDocumentPropertyNames::COUNT.name));
                else
                {
                    if (!scene->meshHandles.CreateEmpty(value, state.allocator, state.allocator))
                        FINJIN_SET_ERROR(error, "Failed to allocate meshes.");
                    else
                        PRINT_VALUE(state.GetDepth(), "mesh count", value);
                }
            }
        };
        AddMapping(mapping, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to add mapping for meshes.");
            return;
        }
    }

    //[scene]/[meshes]/[mesh-index]
    AddMeshMappings<State>
        (
        this,
        [](bool isStart, DataChunkReader& reader, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            if (isStart)
                scene->meshHandles.push_back();
        },
        [](AssetHandle<FinjinMesh>*& result, State& state, Error& error)
        {
            FINJIN_ERROR_METHOD_START(error);

            auto& sceneHandle = state.assetHandle;
            auto& scene = sceneHandle->asset;

            CHECK_MESHES_NOT_EMPTY(mesh, scene->meshHandles)

            result = &mesh;
        },
        error,
        StandardAssetDocumentChunkNames::SCENE, StandardAssetDocumentChunkNames::MESHES, (ChunkName::Index)-1
        );
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add mapping for mesh.");
        return;
    }
}

FinjinSceneDataChunkReaderCallbacks::Settings& FinjinSceneDataChunkReaderCallbacks::GetSettings()
{
    return this->settings;
}


//HandwrittenUserDataTypesReader
HandwrittenUserDataTypesReader::HandwrittenUserDataTypesReader()
{
}

HandwrittenUserDataTypesReader::~HandwrittenUserDataTypesReader()
{
    Destroy();
}

void HandwrittenUserDataTypesReader::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Destroy();

    this->settings = settings;

    if (!this->workingAssetRef.Create(this->settings.setupAllocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create working asset reference.");
        return;
    }

    if (!this->workingPath.Create(this->settings.setupAllocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create working path.");
        return;
    }
}

void HandwrittenUserDataTypesReader::Destroy()
{
    this->settings = Settings();

    this->workingAssetRef.Destroy();
    this->workingPath.Destroy();
}

HandwrittenUserDataTypesReader::Settings& HandwrittenUserDataTypesReader::GetSettings()
{
    return this->settings;
}

void HandwrittenUserDataTypesReader::Parse(const AssetReference& assetRef, const Path& filePath, State& state, const ByteBufferReader& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    state.CreateNewAssetHandleAndAsset<UserDataTypes>(this->workingPath, filePath, error, true);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    auto userDataTypes = state.assetHandle->asset;

    ConfigDocumentReader reader;
    for (auto line = reader.Start(bytes); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                auto value = line->GetSectionName();
                if (value == "include")
                {
                    ParseInclude(state, reader, value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to parse include.");
                        return;
                    }
                }
                else if (value == "enum")
                {
                    ParseEnum(state, userDataTypes, reader, value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to parse enum.");
                        return;
                    }
                }
                else if (value == "class")
                {
                    ParseClass(state, userDataTypes, reader, value, error);
                    if (error)
                    {
                        FINJIN_SET_ERROR(error, "Failed to parse class.");
                        return;
                    }
                }
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_START:
            {
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_END:
            {
                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                break;
            }
            default: break;
        }
    }

    state.assetHandle = nullptr;
}

void HandwrittenUserDataTypesReader::ParseInclude(State& state, ConfigDocumentReader& reader, const Utf8String& initialSectionName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    //auto userDataTypes = state.assetHandle->asset;

    StaticVector<Utf8String, UserDataTypesConstants::MAX_SECTION_DEPTH> sections;
    auto currentSectionName = initialSectionName;

    for (auto line = reader.Next(); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                currentSectionName = line->GetSectionName();
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_START:
            {
                if (sections.full())
                {
                    //This is an error condition
                    return;
                }
                else
                    sections.push_back(currentSectionName);
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_END:
            {
                sections.pop_back();
                if (sections.empty())
                    return;
                else
                    currentSectionName = sections.back();
                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                Utf8String key, value;
                line->GetKeyAndValue(key, value);
                if (currentSectionName == initialSectionName)
                {
                    if (key == "file")
                    {
                        if (!value.empty())
                        {
                            this->workingPath = value;
                            if (!this->workingPath.HasExtension())
                            {
                                auto assetTypeExtension = AssetClassUtilities::ToHandwrittenString(AssetClass::USER_DATA_TYPES);
                                auto assetTypeAndFileTypeExtension = StreamingFileFormatUtilities::MakeExtension(assetTypeExtension, StreamingFileFormat::STREAMING_CONFIG);
                                this->workingPath += ".";
                                this->workingPath += assetTypeAndFileTypeExtension;
                            }

                            this->workingAssetRef.ForLocalFile(this->workingPath);
                            state.HandleEncounteredAssetFilePath(this->workingAssetRef, AssetClass::USER_DATA_TYPES, error);
                            if (error)
                            {
                                FINJIN_SET_ERROR_NO_MESSAGE(error);
                                return;
                            }
                        }
                    }
                }

                break;
            }
            default: break;
        }
    }
}

void HandwrittenUserDataTypesReader::ParseEnum(State& state, UserDataTypes* userDataTypes, ConfigDocumentReader& reader, const Utf8String& initialSectionName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    StaticVector<Utf8String, UserDataTypesConstants::MAX_SECTION_DEPTH> sections;
    auto currentSectionName = initialSectionName;

    UserDataEnum* enumType = nullptr;
    UserDataEnum::Item* enumValue = nullptr;

    for (auto line = reader.Next(); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                currentSectionName = line->GetSectionName();
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_START:
            {
                if (sections.full())
                {
                    //This is an error condition
                    return;
                }
                else
                {
                    if (currentSectionName == "enum-items")
                    {
                        if (enumType == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'enum-items'. No enum type exists.");
                            return;
                        }

                        auto enumValueCount = reader.GetSectionCount("enum-value");
                        if (enumValueCount == 0)
                        {
                            FINJIN_SET_ERROR(error, "No 'enum-value' elements found for enum.");
                            return;
                        }

                        if (!enumType->items.CreateEmpty(enumValueCount, state.allocator, state.allocator))
                        {
                            FINJIN_SET_ERROR(error, "Failed to create vector of enum values.");
                            return;
                        }

                        enumType->items.push_back();
                        enumValue = &enumType->items.back();
                    }

                    sections.push_back(currentSectionName);
                }
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_END:
            {
                sections.pop_back();
                if (sections.empty())
                    return;
                else
                {
                    if (currentSectionName == "enum-value")
                    {
                        if (enumType->items.full())
                        {
                            enumValue = nullptr;
                        #if SORT_USER_DATA_ENUMS_AND_PROPERTIES
                            std::stable_sort(enumType->items.begin(), enumType->items.end());
                        #endif
                        }
                        else
                        {
                            enumType->items.push_back();
                            enumValue = &enumType->items.back();
                        }
                    }

                    currentSectionName = sections.back();
                }
                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                auto sectionName = sections.back();

                Utf8String key, value;
                line->GetKeyAndValue(key, value);
                if (currentSectionName == initialSectionName)
                {
                    if (key == "name")
                    {
                        UserDataTypes::FixName(value);

                        if (!value.empty() && userDataTypes->GetClass(value) == nullptr && userDataTypes->GetEnum(value) == nullptr)
                        {
                            enumType = AllocatedClass::New<UserDataEnum>(state.allocator, FINJIN_CALLER_ARGUMENTS);
                            enumType->name = value;

                            userDataTypes->enums.push_front(enumType);
                        }
                    }
                    else if (key == "type")
                    {
                        UserDataTypes::FixName(value);

                        if (enumType == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'type'. No enum type exists.");
                            return;
                        }

                        enumType->type = UserDataTypes::GetDataType(*enumType, value);
                    }
                    else if (key == "super")
                    {
                        if (!value.empty())
                        {
                            if (enumType == nullptr)
                            {
                                FINJIN_SET_ERROR(error, "Unexpectedly encountered 'super'. No enum type exists.");
                                return;
                            }

                            auto splitResult = Split(value, ' ', [enumType](Utf8StringView& superName)
                            {
                                if (enumType->supers.full())
                                    return ValueOrError<bool>(false);

                                enumType->supers.push_back();
                                if (enumType->supers.back().name.assign(superName.begin(), superName.end()).HasError())
                                {
                                    enumType->supers.pop_back();
                                    return ValueOrError<bool>::CreateError();
                                }

                                UserDataTypes::FixName(enumType->supers.back().name);

                                return ValueOrError<bool>(true);
                            });
                            if (splitResult.HasError())
                            {
                                FINJIN_SET_ERROR(error, "Failed to split enum super names.");
                                return;
                            }
                            else if (!splitResult.value)
                            {
                                FINJIN_SET_ERROR(error, "Failed to split all enum super names.");
                                return;
                            }
                        }
                    }
                }
                else if (sectionName == "enum-value")
                {
                    if (key == "name")
                    {
                        if (enumValue == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'name'. No enum value exists.");
                            return;
                        }

                        UserDataTypes::FixName(value);
                        enumValue->name = value;
                        enumValue->value = value;
                    }
                    else if (key == "value")
                    {
                        if (enumValue == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'value'. No enum value exists.");
                            return;
                        }

                        UserDataTypes::FixName(value);
                        if (!value.empty())
                            enumValue->value = value;
                    }
                }

                break;
            }
            default: break;
        }
    }
}

void HandwrittenUserDataTypesReader::ParseClass(State& state, UserDataTypes* userDataTypes, ConfigDocumentReader& reader, const Utf8String& initialSectionName, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    StaticVector<Utf8String, UserDataTypesConstants::MAX_SECTION_DEPTH> sections;
    auto currentSectionName = initialSectionName;

    UserDataClass* userDataClass = nullptr;
    UserDataClass::Item* data = nullptr;

    Utf8String minValue, maxValue;

    for (auto line = reader.Next(); line != nullptr; line = reader.Next())
    {
        switch (line->GetType())
        {
            case ConfigDocumentLine::Type::SECTION:
            {
                currentSectionName = line->GetSectionName();
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_START:
            {
                if (sections.full())
                {
                    //This is an error condition
                    return;
                }
                else
                {
                    if (currentSectionName == "properties")
                    {
                        if (userDataClass == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'properties'. No user data class exists.");
                            return;
                        }

                        auto propertyCount = reader.GetSectionCount("property");
                        if (propertyCount == 0)
                        {
                            FINJIN_SET_ERROR(error, "No 'property' elements found for user data class.");
                            return;
                        }

                        if (!userDataClass->items.CreateEmpty(propertyCount, state.allocator, state.allocator))
                        {
                            FINJIN_SET_ERROR(error, "Failed to create vector of user data properties.");
                            return;
                        }

                        userDataClass->items.push_back();
                        data = &userDataClass->items.back();
                    }

                    sections.push_back(currentSectionName);
                }
                break;
            }
            case ConfigDocumentLine::Type::SCOPE_END:
            {
                sections.pop_back();
                if (sections.empty())
                    return;
                else
                {
                    if (currentSectionName == "property")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'property' end. No property exists.");
                            return;
                        }

                        if (data->controlType == UserDataControlType::NONE)
                            data->controlType = UserDataTypes::GetControlType(*data, Utf8String::GetEmpty());

                        if (data->type.type == UserDataPrimitiveType::INT_DATA_TYPE)
                        {
                            data->minValue.intValue = minValue.empty() ? 0 : Convert::ToInteger(minValue, data->minValue.intValue);
                            data->maxValue.intValue = maxValue.empty() ? data->minValue.intValue + 100 : Convert::ToInteger(maxValue, data->maxValue.intValue);
                        }
                        else if (data->type.type == UserDataPrimitiveType::FLOAT_DATA_TYPE)
                        {
                            data->minValue.floatValue = minValue.empty() ? 0 : Convert::ToNumber(minValue, data->minValue.floatValue);
                            data->maxValue.floatValue = maxValue.empty() ? data->minValue.floatValue + 100 : Convert::ToNumber(maxValue, data->maxValue.floatValue);
                        }

                        if (userDataClass->items.full())
                        {
                            data = nullptr;
                        #if SORT_USER_DATA_ENUMS_AND_PROPERTIES
                            std::stable_sort(userDataClass->items.begin(), userDataClass->items.end());
                        #endif
                        }
                        else
                        {
                            userDataClass->items.push_back();
                            data = &userDataClass->items.back();
                        }

                        minValue.clear();
                        maxValue.clear();
                    }

                    currentSectionName = sections.back();
                }
                break;
            }
            case ConfigDocumentLine::Type::KEY_AND_VALUE:
            {
                auto sectionName = sections.back();

                Utf8String key, value;
                line->GetKeyAndValue(key, value);
                if (currentSectionName == initialSectionName)
                {
                    if (key == "name")
                    {
                        UserDataTypes::FixName(value);

                        if (!value.empty() && userDataTypes->GetClass(value) == nullptr && userDataTypes->GetEnum(value) == nullptr)
                        {
                            userDataClass = AllocatedClass::New<UserDataClass>(state.allocator, FINJIN_CALLER_ARGUMENTS);

                            userDataClass->name = value;
                            userDataClass->displayName = value;

                            userDataTypes->classes.push_front(userDataClass);
                        }
                    }
                    else if (key == "display-name")
                    {
                        if (!value.empty())
                        {
                            if (userDataClass == nullptr)
                            {
                                FINJIN_SET_ERROR(error, "Unexpectedly encountered 'display-name'. No user data class exists.");
                                return;
                            }

                            userDataClass->displayName = value;
                        }
                    }
                    else if (key == "super")
                    {
                        if (!value.empty())
                        {
                            if (userDataClass == nullptr)
                            {
                                FINJIN_SET_ERROR(error, "Unexpectedly encountered 'super'. No user data class exists.");
                                return;
                            }

                            auto splitResult = Split(value, ' ', [userDataClass](Utf8StringView& superName)
                            {
                                if (userDataClass->supers.full())
                                    return ValueOrError<bool>(false);

                                userDataClass->supers.push_back();
                                if (userDataClass->supers.back().name.assign(superName.begin(), superName.end()).HasError())
                                {
                                    userDataClass->supers.pop_back();
                                    return ValueOrError<bool>::CreateError();
                                }

                                UserDataTypes::FixName(userDataClass->supers.back().name);

                                return ValueOrError<bool>(true);
                            });
                            if (splitResult.HasError())
                            {
                                FINJIN_SET_ERROR(error, "Failed to split class super names.");
                                return;
                            }
                            else if (!splitResult.value)
                            {
                                FINJIN_SET_ERROR(error, "Failed to split all class super names.");
                                return;
                            }

                            for (auto& super : userDataClass->supers)
                                UserDataTypes::FixName(super.name);
                        }
                    }
                    else if (key == "usage")
                    {
                        if (!value.empty())
                        {
                            if (userDataClass == nullptr)
                            {
                                FINJIN_SET_ERROR(error, "Unexpectedly encountered 'usage'. No user data class exists.");
                                return;
                            }

                            auto splitResult = Split(value, ' ', [userDataClass](Utf8StringView& usage)
                            {
                                if (usage == "scene")
                                    userDataClass->usage |= UserDataUsage::SCENE;
                                else if (usage == "object")
                                    userDataClass->usage |= UserDataUsage::OBJECT;
                                else if (usage == "material")
                                    userDataClass->usage |= UserDataUsage::MATERIAL;
                                else if (usage == "private")
                                    userDataClass->usage |= UserDataUsage::PRIVATE;

                                return ValueOrError<bool>(true);
                            });
                            if (splitResult.HasError())
                            {
                                FINJIN_SET_ERROR(error, "Failed to split usages.");
                                return;
                            }
                        }
                    }
                }
                else if (sectionName == "property")
                {
                    if (key == "group-name")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'group-name'. No property exists.");
                            return;
                        }

                        data->groupName = value;
                    }
                    else if (key == "name")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'name'. No property exists.");
                            return;
                        }

                        data->name = value;
                        UserDataTypes::FixName(data->name);

                        data->displayName = data->name;
                    }
                    else if (key == "display-name")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'display-name'. No property exists.");
                            return;
                        }

                        if (!value.empty())
                            data->displayName = value;
                    }
                    else if (key == "visibility-parent-name")
                    {
                        //Ignored
                    }
                    else if (key == "visibility-children-names")
                    {
                        //Ignored
                    }
                    else if (key == "type")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'type'. No property exists.");
                            return;
                        }

                        UserDataTypes::FixName(value);
                        data->typeName = value;
                    }
                    else if (key == "default" || key == "default-value")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'default-value'. No property exists.");
                            return;
                        }

                        data->defaultValue = value;
                    }
                    else if (key == "control-type")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'control-type'. No property exists.");
                            return;
                        }

                        data->controlType = UserDataTypes::GetControlType(*data, value);
                    }
                    else if (key == "edit-width")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'edit-width'. No property exists.");
                            return;
                        }

                        data->editWidth = Convert::ToInteger(value, 0);
                    }
                    else if (key == "edit-height")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'edit-height'. No property exists.");
                            return;
                        }

                        data->editHeight = Convert::ToInteger(value, 0);
                    }
                    else if (key == "min-value")
                    {
                        minValue = value;
                    }
                    else if (key == "max-value")
                    {
                        maxValue = value;
                    }
                    else if (key == "increment")
                    {
                        if (data == nullptr)
                        {
                            FINJIN_SET_ERROR(error, "Unexpectedly encountered 'increment'. No property exists.");
                            return;
                        }

                        data->increment = Convert::ToNumber(value, data->increment);
                    }
                }

                break;
            }
            default: break;
        }
    }
}

//HandwrittenStringsReader
HandwrittenStringsReader::HandwrittenStringsReader()
{
}

HandwrittenStringsReader::~HandwrittenStringsReader()
{
    Destroy();
}

void HandwrittenStringsReader::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Destroy();

    this->settings = settings;

    if (!this->workingAssetRef.Create(this->settings.setupAllocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create working asset reference.");
        return;
    }

    if (!this->workingPath.Create(this->settings.setupAllocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create working path.");
        return;
    }
}

void HandwrittenStringsReader::Destroy()
{
    this->settings = Settings();

    this->workingAssetRef.Destroy();
    this->workingPath.Destroy();
}

HandwrittenStringsReader::Settings& HandwrittenStringsReader::GetSettings()
{
    return this->settings;
}

void HandwrittenStringsReader::Parse(const AssetReference& assetRef, const Path& filePath, State& state, const ByteBufferReader& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    state.CreateNewAssetHandleAndAsset<StringTable>(this->workingPath, filePath, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    state.assetHandle->asset->Parse(bytes, assetRef, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to parse string table asset.");
        return;
    }

    state.assetHandle = nullptr;
}


//FinjinTextureReader
FinjinTextureReader::FinjinTextureReader()
{
}

FinjinTextureReader::~FinjinTextureReader()
{
    Destroy();
}

void FinjinTextureReader::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Destroy();

    this->settings = settings;

    if (!this->workingAssetRef.Create(this->settings.setupAllocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create working asset reference.");
        return;
    }

    if (!this->workingPath.Create(this->settings.setupAllocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create working path.");
        return;
    }
}

void FinjinTextureReader::Destroy()
{
    this->settings = Settings();

    this->workingAssetRef.Destroy();
    this->workingPath.Destroy();
}

FinjinTextureReader::Settings& FinjinTextureReader::GetSettings()
{
    return this->settings;
}

void FinjinTextureReader::Parse(const AssetReference& assetRef, const Path& filePath, State& state, const ByteBufferReader& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    state.CreateNewAssetHandleAndAsset<FinjinTexture>(this->workingPath, filePath, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    if (!state.assetHandle->asset->fileBytes.Create(bytes.data_start(), bytes.max_size(), state.allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate texture buffer.");
        return;
    }

    state.assetHandle = nullptr;
}


//FinjinSoundReader
FinjinSoundReader::FinjinSoundReader()
{
}

FinjinSoundReader::~FinjinSoundReader()
{
    Destroy();
}

void FinjinSoundReader::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    Destroy();

    this->settings = settings;

    if (!this->workingAssetRef.Create(this->settings.setupAllocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create working asset reference.");
        return;
    }

    if (!this->workingPath.Create(this->settings.setupAllocator))
    {
        FINJIN_SET_ERROR(error, "Failed to create working path.");
        return;
    }
}

void FinjinSoundReader::Destroy()
{
    this->settings = Settings();

    this->workingAssetRef.Destroy();
    this->workingPath.Destroy();
}

FinjinSoundReader::Settings& FinjinSoundReader::GetSettings()
{
    return this->settings;
}

void FinjinSoundReader::Parse(const AssetReference& assetRef, const Path& filePath, State& state, const ByteBufferReader& bytes, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    state.CreateNewAssetHandleAndAsset<FinjinSound>(this->workingPath, filePath, error);
    if (error)
    {
        FINJIN_SET_ERROR_NO_MESSAGE(error);
        return;
    }

    if (!state.assetHandle->asset->fileBytes.Create(bytes.data_left(), bytes.max_size(), state.allocator))
    {
        FINJIN_SET_ERROR(error, "Failed to allocate sound buffer.");
        return;
    }

    state.assetHandle = nullptr;
}


//FinjinSceneReader
FinjinSceneReader::FinjinSceneReader(Allocator* allocator) : workingAssetRequest(allocator)
{
    this->filesLoaded = 0;

    this->createdCallbacks = false;
}

void FinjinSceneReader::Create(const Settings& settings, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (settings.assetReadQueue == nullptr)
    {
        FINJIN_SET_ERROR(error, "No asset read queue was specified.");
        return;
    }

    if (settings.assetClassFileReaders == nullptr)
    {
        FINJIN_SET_ERROR(error, "No asset class readers pointer was specified.");
        return;
    }

    if (settings.maxQueuedFiles == 0)
    {
        FINJIN_SET_ERROR(error, "The maximum queued files count must be greater than 0.");
        return;
    }

    Destroy();

    if (settings.state != nullptr)
    {
        //Try to set the state even though it will be overwritten with 'this->settings = settings' a few lines down
        SetState(settings.state, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to set specified destination state.");
            return;
        }
    }

    this->settings = settings;

    if (this->settings.userDataClassPropertiesLookup == nullptr)
    {
        this->userDataClassPropertiesLookup.Create(FinjinSceneReaderConstants::MAX_USER_DATA_OBJECTS, FinjinSceneReaderConstants::MAX_USER_DATA_ENUMS_OR_PROPERTIIES, this->settings.setupAllocator, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to allocate user data properties state.");
            return;
        }
        this->settings.userDataClassPropertiesLookup = &this->userDataClassPropertiesLookup;
    }

    if (!this->createdCallbacks)
    {
        this->createdCallbacks = true;

        this->sceneCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create scene callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Scene", this->sceneCallbacks);

        this->meshCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create mesh callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Mesh", this->meshCallbacks);

        this->skeletonCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create skeleton callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Skeleton", this->skeletonCallbacks);

        this->materialCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create material callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Material", this->materialCallbacks);

        this->prefabCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create prefab callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Prefab", this->prefabCallbacks);

        this->nodeAnimationCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create node animation callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Node animation", this->nodeAnimationCallbacks);

        this->skeletonAnimationCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create skeleton animation callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Skeleton animation", this->skeletonAnimationCallbacks);

        this->morphAnimationCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create morph animation callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Morph animation", this->morphAnimationCallbacks);

        this->poseAnimationCallbacks.Create(this->settings, error);
        if (error)
        {
            Destroy();

            FINJIN_SET_ERROR(error, "Failed to create pose animation callbacks.");
            return;
        }
        PRINT_CALLBACKS_COUNT("Pose animation", this->poseAnimationCallbacks);
    }

    if (!this->tempSimpleUri.Create(this->settings.setupAllocator))
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create temporary simple URI.");
        return;
    }

    if (!this->workingAssetRef.Create(this->settings.setupAllocator))
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create working asset reference.");
        return;
    }

    if (!this->workingAssetRequest.Create(this->settings.setupAllocator))
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create working asset request.");
        return;
    }

    if (!this->assetReadHandleQueue.Create(this->settings.maxQueuedFiles, this->settings.setupAllocator, this->settings.setupAllocator))
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create asset read handle queue.");
        return;
    }

    this->handwrittenUserDataTypesReader.Create(this->settings, error);
    if (error)
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create handwritten user data types reader.");
        return;
    }

    this->handwrittenStringsReader.Create(this->settings, error);
    if (error)
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create handwritten strings reader.");
        return;
    }

    this->textureReader.Create(this->settings, error);
    if (error)
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create texture reader.");
        return;
    }

    this->soundReader.Create(this->settings, error);
    if (error)
    {
        Destroy();

        FINJIN_SET_ERROR(error, "Failed to create sound reader.");
        return;
    }
}

void FinjinSceneReader::Destroy()
{
    if (this->settings.state != nullptr && this->settings.state->controller == this)
        this->settings.state->controller = nullptr;

    this->filesLoaded = 0;
    this->readStatus.Reset();

    this->tempSimpleUri.Destroy();
    this->workingAssetRef.Destroy();
    this->workingAssetRequest.Destroy();

    this->assetReadHandleQueue.Destroy();

    this->handwrittenUserDataTypesReader.Destroy();
    this->handwrittenStringsReader.Destroy();
    this->textureReader.Destroy();
    this->soundReader.Destroy();

    this->settings = Settings();
}

FinjinSceneReader::Settings& FinjinSceneReader::GetSettings()
{
    return this->settings;
}

FinjinSceneReader::State* FinjinSceneReader::GetState()
{
    return this->settings.state;
}

void FinjinSceneReader::SetState(State* state, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    switch (SetState(state))
    {
        case SetStateResult::SUCCESS: break;
        case SetStateResult::READ_IN_PROGRESS: FINJIN_SET_ERROR(error, "Can't change state until all asset requests have completed."); break;
        case SetStateResult::STATE_STILL_ATTACHED_TO_CONTROLLER: FINJIN_SET_ERROR(error, "The specified state is still attached to a controller."); break;
    }
}

FinjinSceneReader::SetStateResult FinjinSceneReader::SetState(State* state)
{
    if (this->settings.state == state)
    {
        //The state has already been set
        return SetStateResult::SUCCESS;
    }

    if (!IsResidentOnGpu())
        return SetStateResult::READ_IN_PROGRESS;

    if (state == nullptr)
    {
        //Detaching
        if (this->settings.state != nullptr)
        {
            this->settings.state->controller = nullptr;
            this->settings.state = nullptr;
        }
        this->readStatus.Reset();
        return SetStateResult::SUCCESS;
    }

    if (state->controller != nullptr)
        return SetStateResult::STATE_STILL_ATTACHED_TO_CONTROLLER;

    this->settings.state = state;
    this->settings.state->controller = this;
    this->readStatus.Reset();
    return SetStateResult::SUCCESS;
}

void FinjinSceneReader::RequestRead(const AssetReference* assetRefs, size_t count, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    for (size_t assetRefIndex = 0; assetRefIndex < count; assetRefIndex++)
    {
        RequestRead(assetRefs[assetRefIndex], true, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to request read for asset '%1%'", assetRefs[assetRefIndex].ToUriString()));
            return;
        }
    }
}

bool FinjinSceneReader::RequestRead(const AssetReference& assetRef, bool addToEncountered, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    if (this->readStatus.GetStatus() == OperationStatus::FAILURE)
    {
        FINJIN_SET_ERROR(error, "Failed to request read. Assets are in a failed state.");
        return false;
    }

    if (this->settings.state == nullptr)
    {
        FINJIN_SET_ERROR(error, "Failed request a read. A destination state has not been set.");
        return false;
    }

    if (this->assetReadHandleQueue.full())
    {
        FINJIN_SET_ERROR(error, "Maximum number of asset files have been queued.");
        return false;
    }

    if (addToEncountered)
    {
        auto addResult = this->settings.state->AddEncounteredFilePath(assetRef.filePath);
        if (addResult.HasError())
        {
            FINJIN_SET_ERROR(error, "Failed to add specified asset to visited set.");
            return false;
        }
        if (!addResult.value)
        {
            //Already encountered
            return false;
        }
    }

    Utf8String assetExtension;
    assetRef.filePath.GetExtension(assetExtension, false);
    auto assetClass = AssetClassUtilities::ParseFromExtension(assetExtension);
    if (assetClass == AssetClass::TEXTURE || this->settings.externalTextureExtensions.contains(assetExtension))
    {
        CreateFullParseAssetRequest(assetRef, AssetClass::TEXTURE, this->textureReader, this->settings.state->textureReaderState);
    }
    else if (assetClass == AssetClass::SOUND || this->settings.externalSoundExtensions.contains(assetExtension))
    {
        CreateFullParseAssetRequest(assetRef, AssetClass::SOUND, this->soundReader, this->settings.state->soundReaderState);
    }
    else
    {
        if (assetClass == AssetClass::COUNT)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to determine asset class from extension '%1%'.", assetExtension));
            return false;
        }

        StreamingFileFormat assetStreamingFormat;
        StreamingFileFormatUtilities::ParseFromExtension(assetStreamingFormat, assetExtension, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to determine asset streaming file type from extension '%1%'.", assetExtension));
            return false;
        }

        switch (assetClass)
        {
            case AssetClass::SCENE:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->sceneCallbacks, this->settings.state->sceneCallbacksState);
                break;
            }
            case AssetClass::MESH:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->meshCallbacks, this->settings.state->meshCallbacksState);
                break;
            }
            case AssetClass::SKELETON:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->skeletonCallbacks, this->settings.state->skeletonCallbacksState);
                break;
            }
            case AssetClass::MATERIAL:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->materialCallbacks, this->settings.state->materialCallbacksState);
                break;
            }
            case AssetClass::PREFAB:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->prefabCallbacks, this->settings.state->prefabCallbacksState);
                break;
            }
            case AssetClass::NODE_ANIMATION:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->nodeAnimationCallbacks, this->settings.state->nodeAnimationCallbacksState);
                break;
            }
            case AssetClass::SKELETON_ANIMATION:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->skeletonAnimationCallbacks, this->settings.state->skeletonAnimationCallbacksState);
                break;
            }
            case AssetClass::MORPH_ANIMATION:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->morphAnimationCallbacks, this->settings.state->morphAnimationCallbacksState);
                break;
            }
            case AssetClass::POSE_ANIMATION:
            {
                CreateStreamingAssetRequest(assetRef, assetClass, assetStreamingFormat, this->poseAnimationCallbacks, this->settings.state->poseAnimationCallbacksState);
                break;
            }
            case AssetClass::USER_DATA_TYPES:
            {
                if (AssetClassUtilities::IsHandwrittenExtension(assetExtension))
                    CreateFullParseAssetRequest(assetRef, assetClass, this->handwrittenUserDataTypesReader, this->settings.state->handwrittenUserDataTypesReaderState);
                else
                {
                    FINJIN_SET_ERROR(error, "Only handwritten user data types are supported.");
                    return false;
                }
                break;
            }
            case AssetClass::STRING_TABLE:
            {
                if (AssetClassUtilities::IsHandwrittenExtension(assetExtension))
                    CreateFullParseAssetRequest(assetRef, assetClass, this->handwrittenStringsReader, this->settings.state->handwrittenStringsReaderState);
                else
                {
                    FINJIN_SET_ERROR(error, "Only handwritten strings are supported.");
                    return false;
                }
                break;
            }
            default: break;
        }
    }

    if (this->readStatus.GetStatus() == OperationStatus::NONE)
    {
        this->readStatus.UpdateProgress(0);
        this->readStatus.SetStatus(OperationStatus::STARTED);
    }
    else if (this->readStatus.IsSuccess())
    {
        this->readStatus.SetStatus(OperationStatus::STARTED);
    }

    this->assetReadHandleQueue.push_back();
    auto& readHandle = this->assetReadHandleQueue.back();

    this->settings.assetReadQueue->AddRequest(readHandle, this->workingAssetRequest, error);
    if (error)
    {
        this->assetReadHandleQueue.pop_back();

        this->readStatus.SetStatus(OperationStatus::FAILURE);

        FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to add asset read request for asset '%1%'.", assetRef.ToUriString()));
        return false;
    }

    return true;
}

OperationStatus& FinjinSceneReader::GetReadStatus()
{
    return this->readStatus;
}

AssetCollisionResolution FinjinSceneReader::ResolveAssetCollision(AssetHandle<void>* existingAssetHandle, AssetClass assetClass, AssetHandle<void>* newAssetHandle)
{
    return AssetCollisionResolution::GENERATE_ERROR;
}

bool FinjinSceneReader::HandleNewAsset(const AssetReference& assetRef, AssetClass assetClass, Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    RequestRead(assetRef, false, error);
    if (error)
    {
        FINJIN_SET_ERROR(error, "Failed to add asset read request.");
        return false;
    }

    return true;
}

void FinjinSceneReader::FinishedFile(Error& error)
{
    FINJIN_ERROR_METHOD_START(error);

    this->assetReadHandleQueue.front().FinishInProgress();
    this->assetReadHandleQueue.pop_front();

    this->filesLoaded++;
    this->readStatus.UpdateProgress(RoundToFloat(this->filesLoaded) / RoundToFloat(this->settings.state->GetEncounteredFileCount()));

    if (IsResidentOnGpu())
    {
        //Resolve assets' internal dependencies
        for (size_t assetClass = 0; assetClass < this->settings.state->assetBucketsByClass.size(); assetClass++)
        {
            auto& assetBucket = this->settings.state->assetBucketsByClass[assetClass];

            for (auto assetHandle = assetBucket.internalDependencyHead; assetHandle != nullptr;)
            {
                switch (static_cast<AssetClass>(assetClass))
                {
                    case AssetClass::USER_DATA_TYPES:
                    {
                        auto userDataTypes = ((AssetHandle<UserDataTypes>*)assetHandle)->asset;

                        //Create lookups
                        if (this->settings.state->createOptimizedLookups)
                        {
                            userDataTypes->CreateLookups(this->settings.state->allocator, error);
                            if (error)
                            {
                                this->readStatus.SetStatus(OperationStatus::FAILURE);

                                FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to create lookups for user data types collection '%1%'.", userDataTypes->name));
                                return;
                            }
                        }

                        //Resolve internal dependencies
                        userDataTypes->ResolveInternalDependencies(error);
                        if (error)
                        {
                            this->readStatus.SetStatus(OperationStatus::FAILURE);

                            FINJIN_SET_ERROR(error, FINJIN_FORMAT_ERROR_MESSAGE("Failed to resolve dependencies for user data types collection '%1%'.", userDataTypes->name));
                            return;
                        }

                        break;
                    }
                    default: break;
                }

                auto next = assetHandle->internalDependencyNext;
                assetHandle->internalDependencyNext = nullptr;
                assetHandle = next;
            }

            assetBucket.internalDependencyHead = nullptr;
        }

        this->readStatus.SetStatus(OperationStatus::SUCCESS);
    }
    else if (!this->settings.state->filesToLoad.empty())
    {
        //This will generally not be called since the controller handles the addition of new read requests in HandleNewAsset()
        //However, it's helpful to leave around for testing.

        this->workingAssetRef.ForUriString(this->settings.state->filesToLoad.current(), this->tempSimpleUri, error);
        this->settings.state->filesToLoad.pop();
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to create asset reference for next asset.");
            return;
        }

        RequestRead(this->workingAssetRef, false, error);
        if (error)
        {
            FINJIN_SET_ERROR(error, "Failed to request read for next asset.");
            return;
        }
    }
}

bool FinjinSceneReader::IsResidentOnGpu() const
{
    return this->assetReadHandleQueue.empty() && (this->settings.state == nullptr || this->settings.state->filesToLoad.empty());
}
