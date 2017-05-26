#!/usr/bin/env python3

#Copyright (c) 2017 Finjin
#
#This file is part of Finjin Engine (finjin-engine).
#
#Finjin Engine is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#This Source Code Form is subject to the terms of the Mozilla Public
#License, v. 2.0. If a copy of the MPL was not distributed with this
#file, You can obtain one at http://mozilla.org/MPL/2.0/.


"""
Usage: generate_shaders.py

A work in progress tool that compiles various configurations of prewritten shader code files.

This script should be run on Windows.

Support will eventually be added for:
-Reading the shader configuration from an external Python file.
-Compiling Vulkan shaders on Linux.
-Compiling Metal shaders on macOS.
"""


#Imports-----------------------------------------------------------------------
import os
import os.path
import imp
import shutil
import struct
import sys
import time
import subprocess
import platform
from collections import OrderedDict, deque


#Utility functions-------------------------------------------------------------
def to_utf8(text):
    return text.encode('utf-8')

def expand_user(path):
    return os.path.expanduser(path)

def read_binary_file(filename):
    with open(filename, 'rb') as f:
        return f.read()


#Classes-----------------------------------------------------------------------
class Strings:
    def __init__(self, uppercase_long, lowercase_long, lowercase_short):
        self.uppercase_long = uppercase_long
        self.lowercase_long = lowercase_long
        self.lowercase_short = lowercase_short

class LightType:
    DIRECTIONAL = 0
    POINT = 1
    SPOT = 2

    COUNT = 3

    STRINGS = (
        Strings('DIRECTIONAL', 'directional', 'd'),
        Strings('POINT', 'point', 'p'),
        Strings('SPOT', 'spot', 's')
        )

    @staticmethod
    def _increment_light_counts(light_type_counts, max_count):
        for i in range(len(light_type_counts)):
            light_type_counts[i] += 1
            if light_type_counts[i] <= max_count:
                return True
            if i == len(light_type_counts) - 1:
                return False
            light_type_counts[i] = 0

    @staticmethod
    def get_all_combinations(max_count, light_types = None):
        light_types = light_types or [LightType.DIRECTIONAL, LightType.POINT, LightType.SPOT]
        light_type_counts = [0 for i in range(len(light_types))]
        while True:
            result = []
            light_sum = 0
            for light_type_index in range(len(light_types)):
                light_type = light_types[light_type_index]
                light_sum += light_type_counts[light_type_index]
                for i in range(light_type_counts[light_type_index]):
                    result.append(light_type)
            if light_sum <= max_count:
                yield sorted(result)
            if not LightType._increment_light_counts(light_type_counts, max_count):
                break


class LightTypes:
    def __init__(self):
        self.light_types = None
        self.light_lists = None

    @staticmethod
    def simple_list(light_types):
        result = LightTypes()
        result.light_types = light_types or []
        return result

    @staticmethod
    def combination_list(max_count, light_types = None):
        #When using a combination light type list, it must be the "variable" portion of the variation settings, not the "fixed" part
        result = LightTypes()
        result.light_lists = [item for item in LightType.get_all_combinations(max_count, light_types) if len(item) > 0]
        return result

    def deep_copy(self):
        result = LightTypes()
        if self.light_types is not None:
            result.light_types = list(self.light_types)
        elif self.light_lists is not None:
            result.light_lists = []
            for sublist in self.light_lists:
                result.light_lists.append(list(sublist))
        return result

    def merge_from(self, other):
        if self.light_types is None and self.light_lists is None:
            other_copy = other.deep_copy()
            self.light_types = other_copy.light_types
            self.light_lists = other_copy.light_lists
        else:
            for item in other:
                append(item)
            self.sort()

    def sort(self):
        if self.light_types is not None:
            self.light_types.sort()
        if self.light_lists is not None:
            for sublist in self.light_lists:
                sublist.sort()

    def __iter__(self):
        if self.light_types is not None:
            for light_type in self.light_types:
                yield light_type
        elif self.light_lists is not None:
            for sublist in self.light_lists:
                yield sublist

    def __len__(self):
        if self.light_types is not None:
            return len(self.light_types)
        elif self.light_lists is not None:
            return len(self.light_lists)
        else:
            return 0

    def append(self, item):
        if isinstance(item, list):
            #The specified item is a list
            if self.light_lists is not None:
                #Ensure the item doesn't already exist in list
                for sublist in self.light_lists:
                    if cmp(sublist, item) == 0:
                        #Found a match
                        return
                self.light_lists.append(list(item)) #Make a copy of the list when appending
            elif self.light_types is not None:
                self.light_types += item
            else:
                raise RuntimeError('Failed to append specified list. The light types collection has not been initialized.')
        else:
            #The specified item is a single item, presumably a LightType number
            if self.light_lists is not None:
                #Add the light to each sublist
                for sublist in self.light_lists:
                    sublist.append(item)
            elif self.light_types is not None:
                self.light_types.append(item)
            else:
                raise RuntimeError('Failed to append specified value. The light types collection has not been initialized.')

class PrimitiveTopology:
    POINT = 0
    LINE = 1
    TRIANGLE = 2
    PATCH = 3

    COUNT = 4

    STRINGS = (
        Strings('POINT', 'point', 'po'),
        Strings('LINE', 'line', 'li'),
        Strings('TRIANGLE', 'triangle', 'tr'),
        Strings('PATCH', 'patch', 'pa')
        )

class ShaderFeatureFlag:
    NONE = 0

    #Map (texture)
    MAP_DIFFUSE = 1 << 0
    MAP_SPECULAR = 1 << 1
    MAP_REFLECTION = 1 << 2
    MAP_REFRACTION = 1 << 3
    MAP_BUMP = 1 << 4
    MAP_HEIGHT = 1 << 5
    MAP_SELF_ILLUMINATION = 1 << 6
    MAP_OPACITY = 1 << 7
    MAP_ENVIRONMENT = 1 << 8,
    MAP_SHININESS = 1 << 9,

    #Mesh
    MESH_SKELETON_ANIMATION = 1 << 10
    MESH_POSE_ANIMATION = 1 << 11
    MESH_MORPH_ANIMATION = 1 << 12

    #Environment
    ENVIRONMENT_LINEAR_DISTANCE_FOG = 1 << 13
    ENVIRONMENT_EXP_DISTANCE_FOG = 1 << 14

    #Rendering
    RENDERING_FILL_WIREFRAME = 1 << 15 #If not specified, solid fill is assumed
    RENDERING_VELOCITY_BLUR = 1 << 16

    ALL = 0xffffffff

    COUNT = 17

    STRINGS = (
        Strings('MAP_DIFFUSE', 'map-diffuse', 'map-di'),
        Strings('MAP_SPECULAR', 'map-specular', 'map-sp'),
        Strings('MAP_REFLECTION', 'map-reflection', 'map-rl'),
        Strings('MAP_REFRACTION', 'map-refraction', 'map-rr'),
        Strings('MAP_BUMP', 'map-bump', 'map-bu'),
        Strings('MAP_HEIGHT', 'map-height', 'map-he'),
        Strings('MAP_SELF_ILLUMINATION', 'map-self-illumination', 'map-si'),
        Strings('MAP_OPACITY', 'map-opacity', 'map-op'),
        Strings('MAP_ENVIRONMENT', 'map-environment', 'map-en'),
        Strings('MAP_SHININESS', 'map-shininess', 'map-sh'),

        Strings('MESH_SKELETON_ANIMATION', 'mesh-skeleton-animation', 'mesh-sa'),
        Strings('MESH_POSE_ANIMATION', 'mesh-pose-animation', 'mesh-pa'),
        Strings('MESH_MORPH_ANIMATION', 'mesh-morph-animation', 'mesh-ma'),

        Strings('ENVIRONMENT_LINEAR_DISTANCE_FOG', 'environment-linear-distance-fog', 'environment-ldf'),
        Strings('ENVIRONMENT_EXP_DISTANCE_FOG', 'environment-exp-distance-fog', 'environment-edf'),

        Strings('RENDERING_FILL_WIREFRAME', 'rendering-wireframe', 'rendering-w'),
        Strings('RENDERING_VELOCITY_BLUR', 'rendering-velocity-blur', 'rendering-vb')
        )

class ShaderAPI:
    D3D12 = 0
    VULKAN = 1
    METAL = 2

    COUNT = 3

    STRINGS = (
        Strings('D3D12', 'd3d12', 'd3d12'),
        Strings('VULKAN', 'vulkan', 'vulkan'),
        Strings('METAL', 'metal', 'metal')
        )

class TargetOS:
    WINDOWS = 0
    MACOS = 1
    IOS = 2
    TVOS = 3
    ANDROID = 4
    LINUX = 5

class ShaderType:
    VERTEX = 0
    PIXEL = 1
    DOMAIN = 2
    GEOMETRY = 3
    HULL = 4

    COUNT = 5

    STRINGS = (
        Strings('VERTEX', 'vertex', 'v'),
        Strings('PIXEL', 'pixel', 'p'),
        Strings('DOMAIN', 'domain', 'd'),
        Strings('GEOMETRY', 'geometry', 'g'),
        Strings('HULL', 'hull', 'h')
        )

class VulkanShaderType:
    VERTEX = 0
    TESSELLATION_CONTROL = 1
    TESSELLATION_EVALUATION = 2
    GEOMETRY = 3
    FRAGMENT = 4
    COMPUTE = 5

    COUNT = 6

    STRINGS = (
        Strings('VERTEX', 'vertex', 'v'),
        Strings('TESSELLATION_CONTROL', 'tessellation-control', 'tc'),
        Strings('TESSELLATION_EVALUATION', 'tessellation-evaluation', 'te'),
        Strings('GEOMETRY', 'geometry', 'g'),
        Strings('FRAGMENT', 'fragment', 'f')
        )

class ShaderVariationSettings:
    def __init__(self, rendering_flags = ShaderFeatureFlag.NONE, light_types = None):

        self.rendering_flags = rendering_flags

        if light_types is not None and isinstance(light_types, list):
            self.light_types = LightTypes.simple_list(light_types)
        else:
            self.light_types = light_types or LightTypes()
        self.light_types.sort()

    def merge_from(self, other):
        self.rendering_flags |= other.rendering_flags
        self.light_types.merge_from(other.light_types)

    def deep_copy(self):
        result = ShaderVariationSettings()
        result.rendering_flags = self.rendering_flags
        result.light_types = self.light_types.deep_copy()
        return result

    def rendering_flag_strings(self):
        result = []
        for flag_index in range(ShaderFeatureFlag.COUNT):
            if self.rendering_flags & (1 << flag_index):
                result.append(ShaderFeatureFlag.STRINGS[flag_index].lowercase_long)
        return result

    def rendering_flag_exported_string(self, use_long_names):
        result = ''
        for flag_index in range(ShaderFeatureFlag.COUNT):
            if self.rendering_flags & (1 << flag_index):
                if use_long_names:
                    result += ShaderFeatureFlag.STRINGS[flag_index].lowercase_long
                else:
                    result += ShaderFeatureFlag.STRINGS[flag_index].lowercase_short
                result += ' '
        return result.strip()

    def light_type_strings(self):
        result = []
        for light_type in self.light_types:
            if isinstance(light_type, list):
                raise RuntimeError('Unexpectedly encountered a list type.')
            result.append(LightType.STRINGS[light_type].lowercase_long)
        return result

    def light_type_exported_string(self, use_long_names):
        result = ''
        for light_type in self.light_types:
            if isinstance(light_type, list):
                raise RuntimeError('Unexpectedly encountered a list type.')
            if use_long_names:
                result += LightType.STRINGS[light_type].lowercase_long
            else:
                result += LightType.STRINGS[light_type].lowercase_short
            result += ' '
        return result.strip()

    def get_light_count(self, value):
        count = 0
        for light_type in self.light_types:
            if light_type == value:
                count += 1
        return count

class ShaderVariation:
    def __init__(
        self,
        name,
        parent_variation = None,
        fixed_settings = None,
        variable_settings = None,
        root_signature = None,
        input_format = None,
        primitive_topology = PrimitiveTopology.TRIANGLE,
        generate_shader_types = None,
        shader_input_file_base_name = None
        ):

        self.name = name

        self.fixed_settings = fixed_settings or ShaderVariationSettings()

        self.variable_settings = variable_settings or ShaderVariationSettings()

        self.root_signature = root_signature
        self.input_format = input_format
        self.primitive_topology = primitive_topology

        self.generate_shader_types = generate_shader_types or []

        self.shader_input_file_base_name = shader_input_file_base_name

        self.parent_variation = parent_variation

        self.child_variations = []

        self.output_file_paths = [None for shader_type in range(ShaderType.COUNT)]

    def primitive_topology_exported_string(self, use_long_names):
        if use_long_names:
            return PrimitiveTopology.STRINGS[self.primitive_topology].lowercase_long
        else:
            return PrimitiveTopology.STRINGS[self.primitive_topology].lowercase_short

    def is_rendering_variable(self):
        return self.variable_settings.rendering_flags != 0

    def is_lighting_variable(self):
        return len(self.variable_settings.light_types) > 0

    def is_variable(self):
        return self.is_rendering_variable() or self.is_lighting_variable()

    def iterate_child_variations(self, names):
        count = 0
        for child in self.child_variations:
            name = self.name + '_c' + str(count)
            if names.get(name, None) is None:
                names[name] = 1
                count += 1

                #Shallow copy on child.variable_settings
                result = ShaderVariation(name, None, child.fixed_settings.deep_copy(), child.variable_settings, self.root_signature, self.input_format, self.primitive_topology, self.generate_shader_types, self.shader_input_file_base_name)
                result.fixed_settings.merge_from(self.fixed_settings)

                yield result

    def iterate_rendering_flags(self, names):
        if not self.is_rendering_variable():
            yield self
            return

        count = 0

        flag_list = []
        for flag_index in range(ShaderFeatureFlag.COUNT):
            flag = self.variable_settings.rendering_flags & (1 << flag_index)
            if flag:
                flag_list.append(flag)

        include_base_case = True
        start_index = 0 if include_base_case else 1

        permutation_count = 1 << len(flag_list)
        for permutation_bits in range(start_index, permutation_count):
            flags = ShaderFeatureFlag.NONE
            for flag_index in range(len(flag_list)):
                if permutation_bits & (1 << flag_index):
                    flags |= flag_list[flag_index]

            name = self.name + '_r' + str(count)
            if names.get(name, None) is None:
                names[name] = 1
                count += 1

                result = ShaderVariation(name, None, self.fixed_settings.deep_copy(), None, self.root_signature, self.input_format, self.primitive_topology, self.generate_shader_types, self.shader_input_file_base_name)
                if self.parent_variation:
                    result.fixed_settings.merge_from(self.parent_variation.fixed_settings)

                result.fixed_settings.rendering_flags |= flags;

                #Pass the variable light settings
                result.variable_settings.light_types = self.variable_settings.light_types.deep_copy()

                result.child_variations = self.child_variations #Shallow copy

                yield result

    def iterate_light_variations(self, names):
        if not self.is_lighting_variable():
            yield self
            return

        count = 0

        include_base_case = True
        if include_base_case:
            name = self.name + '_l' + str(count)
            if names.get(name, None) is None:
                names[name] = 1
                count += 1

                result = ShaderVariation(name, None, self.fixed_settings.deep_copy(), None, self.root_signature, self.input_format, self.primitive_topology, self.generate_shader_types, self.shader_input_file_base_name)
                if self.parent_variation:
                    result.fixed_settings.merge_from(self.parent_variation.fixed_settings)

                #Pass the variable rendering settings
                result.variable_settings.rendering_flags = self.variable_settings.rendering_flags

                result.child_variations = self.child_variations #Shallow copy

                yield result

        if self.variable_settings.light_types.light_types is not None:
            for i in range(len(self.variable_settings.light_types.light_types)):
                name = self.name + '_l' + str(count)
                if names.get(name, None) is None:
                    names[name] = 1
                    count += 1

                    result = ShaderVariation(name, None, self.fixed_settings.deep_copy(), None, self.root_signature, self.input_format, self.primitive_topology, self.generate_shader_types, self.shader_input_file_base_name)
                    if self.parent_variation:
                        result.fixed_settings.merge_from(self.parent_variation.fixed_settings)

                    for light_type in self.variable_settings.light_types.light_types[:i + 1]:
                        result.fixed_settings.light_types.append(light_type)
                    result.fixed_settings.light_types.sort()

                    #Pass the variable rendering settings
                    result.variable_settings.rendering_flags = self.variable_settings.rendering_flags

                    result.child_variations = self.child_variations #Shallow copy

                    yield result
        elif self.variable_settings.light_types.light_lists is not None:
            for sublist in self.variable_settings.light_types.light_lists:
                name = self.name + '_l' + str(count)
                if names.get(name, None) is None:
                    names[name] = 1
                    count += 1

                    result = ShaderVariation(name, None, self.fixed_settings.deep_copy(), None, self.root_signature, self.input_format, self.primitive_topology, self.generate_shader_types, self.shader_input_file_base_name)
                    if self.parent_variation:
                        result.fixed_settings.merge_from(self.parent_variation.fixed_settings)

                    result.fixed_settings.light_types = result.fixed_settings.light_types or []
                    for light_type in sublist:
                        result.fixed_settings.light_types.append(light_type)
                    result.fixed_settings.light_types.sort()

                    #Pass the variable rendering settings
                    result.variable_settings.rendering_flags = self.variable_settings.rendering_flags

                    result.child_variations = self.child_variations #Shallow copy

                    yield result

    @staticmethod
    def create_hierarchy(variations):
        #Create variations lookup
        lookup = {variation.name : variation for variation in variations}

        #Resolve dependencies
        for variation in variations:
            if isinstance(variation.parent_variation, str):
                parent_variation_name = variation.parent_variation
                variation.parent_variation = lookup.get(parent_variation_name, None)
                if variation.parent_variation is None:
                    raise RuntimeError('Failed to locate parent variation \'{0}\'.'.format(parent_variation_name))

                #Add to parent
                variation.parent_variation.child_variations.append(variation)

class CompilerMacro:
    def __init__(self, key, value):
        self.key = key
        self.value = value

class CompilerRun:
    def __init__(self, shader_compiler_path, shader_entry_point_prefix, shader_file_name_suffix, shader_output_file_extension, shader_model_and_version):
        self.shader_compiler_path = shader_compiler_path
        self.shader_entry_point_prefix = shader_entry_point_prefix
        self.shader_file_name_suffix = shader_file_name_suffix
        self.shader_output_file_extension = shader_output_file_extension
        self.shader_model_and_version = shader_model_and_version

        self.combined_shader_start_alignment = 4

class D3D12CompilerRun(CompilerRun):
    def __init__(self, shader_compiler_path, shader_model_prefix, shader_model_version):
        CompilerRun.__init__(self, shader_compiler_path, shader_model_prefix.upper(), shader_model_prefix, 'cso', shader_model_prefix + '_' + shader_model_version)

    def get_command_string(self, macros, shader_entry_point, shader_output_file_path, shader_input_file_path):
        shader_entry_point = self.shader_entry_point_prefix + shader_entry_point
        macros_string = ' '.join('/D ' + m.key + '=' + m.value for m in macros)
        return '"{0}" /enable_unbounded_descriptor_tables /nologo /T {1} {2} /E {3} /Fo "{4}" "{5}"'.format(self.shader_compiler_path, self.shader_model_and_version.lower(), macros_string, shader_entry_point, shader_output_file_path, shader_input_file_path)

class CompilerProcesses:
    MAX_PROCESSES = 50

    def __init__(self):
        self.queue = deque()

    def append(self, process):
        self.queue.append(process)

    def wait_for_one(self):
        if len(self.queue) == CompilerProcesses.MAX_PROCESSES:
            #The queue is full and the caller wants to execute another process
            #Rather than wait for just one to complete, it is ultimately more efficient overall to wait for the entire queue to finish
            self.wait_for_all()

    def wait_for_all(self):
        while len(self.queue) > 0:
            process = self.queue.pop()
            self._wait_for_process(process)

    def _wait_for_process(self, process):
        process.wait()


#Functions---------------------------------------------------------------------
def print_variation(variation, depth = 0):
    indent = '  ' * depth

    print(indent + 'Name: ' + variation.name)

    print(indent + 'Input file base name: ' + variation.shader_input_file_base_name)

    print(indent + 'Input format: ' + str(variation.input_format))

    print(indent + 'Fixed rendering flags: ' + str(variation.fixed_settings.rendering_flag_strings()))
    print(indent + 'Fixed light types: ' + str(variation.fixed_settings.light_type_strings()))

    print(indent + 'Variable rendering flags: ' + str(variation.variable_settings.rendering_flag_strings()))
    print(indent + 'Variable light types: ' + str(variation.variable_settings.light_type_strings()))

def print_variation_hierarchy(variation, depth = 0):
    print_variation(variation, depth)

    print(50 * '-')

    for child in variation.child_variations:
        print_variation_hierarchy(child, depth + 1)

def iterate_variations(variation, handle_variation, names):
    if variation.is_variable():
        for rendering_flag_variation in variation.iterate_rendering_flags(names):
            for light_variation in rendering_flag_variation.iterate_light_variations(names):
                iterate_variations(light_variation, handle_variation, names)
                for child in light_variation.iterate_child_variations(names):
                    iterate_variations(child, handle_variation, names)
    else:
        handle_variation(variation)
        for child in variation.iterate_child_variations(names):
            iterate_variations(child, handle_variation, names)

def generate_variations(variations):
    result = OrderedDict()

    def handle_variation(var):
        result[var.name] = var

    names = {}
    for variation in variations:
        if variation.parent_variation is None:
            iterate_variations(variation, handle_variation, names)

    return result


#Main--------------------------------------------------------------------------
def main(args = None):
    #The application entry point
    result = 0 #Assume success

    #count = 0
    #for x in LightType.get_all_combinations(4, None):#[LightType.DIRECTIONAL, LightType.SPOT]):
    #    print(str(x))
    #    count += 1
    #rint('count: ' + str(count))
    #return result

    variations = [
        ShaderVariation('base', None, ShaderVariationSettings(ShaderFeatureFlag.NONE, None), ShaderVariationSettings(ShaderFeatureFlag.MAP_DIFFUSE | ShaderFeatureFlag.MAP_BUMP | ShaderFeatureFlag.MAP_REFLECTION, LightTypes.combination_list(4)), 'finjin.smr.rs.default', 'finjin.input-format.simple', PrimitiveTopology.TRIANGLE, [ShaderType.VERTEX, ShaderType.PIXEL], 'ForwardShaders'),

        #ShaderVariation('derived-dir', 'base', ShaderVariationSettings(ShaderFeatureFlag.NONE, [LightType.DIRECTIONAL]), ShaderVariationSettings(ShaderFeatureFlag.NONE, [LightType.DIRECTIONAL, LightType.DIRECTIONAL])),
        #ShaderVariation('derived-spot', 'base', ShaderVariationSettings(ShaderFeatureFlag.NONE, [LightType.SPOT]), ShaderVariationSettings(ShaderFeatureFlag.NONE, [LightType.SPOT, LightType.SPOT])),
        #ShaderVariation('derived-point', 'base', ShaderVariationSettings(ShaderFeatureFlag.NONE, [LightType.POINT]), ShaderVariationSettings(ShaderFeatureFlag.NONE, [LightType.POINT, LightType.POINT])),
        #ShaderVariation('base', None, ShaderVariationSettings(ShaderFeatureFlag.MAP_DIFFUSE, [LightType.DIRECTIONAL]), ShaderVariationSettings(ShaderFeatureFlag.MAP_BUMP, [LightType.DIRECTIONAL, LightType.DIRECTIONAL]), 'finjin.smr.rs.default', 'finjin.input-format.simple', PrimitiveTopology.TRIANGLE, [ShaderType.VERTEX, ShaderType.PIXEL]),
        #ShaderVariation('derived1', 'base', ShaderVariationSettings(ShaderFeatureFlag.MESH_SKELETON_ANIMATION, [LightType.SPOT]), ShaderVariationSettings(ShaderFeatureFlag.ENVIRONMENT_LINEAR_DISTANCE_FOG | ShaderFeatureFlag.RENDERING_PER_PIXEL_LIGHTING))
    ]
    ShaderVariation.create_hierarchy(variations)

    generated_variations = generate_variations(variations)

    #print_variation_hierarchy(variations[0])

    print_variations = True
    print_commands = True

    if print_variations:
        print('Generated \'{0}\' shader variations.'.format(str(len(generated_variations))))
        for name in generated_variations:
            variation = generated_variations[name]
            print_variation(variation)
            print(50 * '-')

    #Not used but it should be in a future version
    #input_formats_input_file_path = r'C:\git\nedelman\finjin-viewer\project\windows-win32-vs\x64\D3D12-Win32-Debug\app\settings\gpu-context-input-formats.cfg'

    #output_directory = expand_user('~/shader-generator')
    output_directory = r'C:\git\nedelman\finjin-viewer\project\windows-win32-vs\x64\D3D12-Win32-Debug\app'
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    combine_shader_files = True
    remove_uncombined_shader_files = True
    use_long_value_names = False

    standard_macros = []

    shader_entry_point_name = 'Main'

    shader_output_file_base_name = 'gpu-context-static-mesh-renderer-render-states'

    shader_api = ShaderAPI.D3D12
    target_os = TargetOS.WINDOWS

    settings_output_directory = os.path.join(output_directory, 'settings-' + ShaderAPI.STRINGS[shader_api].lowercase_long)
    if not os.path.exists(settings_output_directory):
        os.makedirs(settings_output_directory)

    shader_output_directory = os.path.join(output_directory, 'shaders-' + ShaderAPI.STRINGS[shader_api].lowercase_long)
    if not os.path.exists(shader_output_directory):
        os.makedirs(shader_output_directory)

    pipeline_state_output_filepath = os.path.join(settings_output_directory, shader_output_file_base_name + '.cfg')

    compiler_runs = [None for shader_type in range(ShaderType.COUNT)]
    if shader_api == ShaderAPI.D3D12:
        compiler_path = 'C:/Program Files (x86)/Windows Kits/10/bin/x64/fxc'
        shader_input_directory = 'C:/git/nedelman/finjin-viewer/assets/shaders/d3d12'
        shader_input_file_extension = 'hlsl'
        d3d12_shader_model_version = '5_1'
        compiler_runs[ShaderType.VERTEX] = D3D12CompilerRun(compiler_path, 'VS', d3d12_shader_model_version)
        compiler_runs[ShaderType.PIXEL] = D3D12CompilerRun(compiler_path, 'PS', d3d12_shader_model_version)
        compiler_runs[ShaderType.DOMAIN] = D3D12CompilerRun(compiler_path, 'DS', d3d12_shader_model_version)
        compiler_runs[ShaderType.GEOMETRY] = D3D12CompilerRun(compiler_path, 'GS', d3d12_shader_model_version)
        compiler_runs[ShaderType.HULL] = D3D12CompilerRun(compiler_path, 'HS', d3d12_shader_model_version)
    elif shader_api == ShaderAPI.METAL:
        if target_os == TargetOS.MACOS:
            metal_os_option = 'macosx'
            metal_platform_id = 'osx'
        else:
            metal_os_option = 'iphoneos'
            metal_platform_id = 'ios'
        metal_shader_model_version = '1.1'
        compiler_path = 'xcrun -sdk {0} metal'.format(metal_os_option)
        ar_compiler_path = 'xcrun -sdk {0} metal-ar'.format(metal_os_option)
        metallib_compiler_path = 'xcrun -sdk {0} metallib'.format(metal_os_option)
        shader_input_directory = '/Users/derek/git/nedelman/finjin-viewer/assets/shaders/metal'
        shader_input_file_extension = 'metal'

        #$COMPILER_PATH -std=$PLATFORM_ID-metal$METAL_VERSION $INCLUDE_OPTIONS -o "$OUTPUT_PATH/shaders.air" shaders.metal
        #$AR_COMPILER_PATH rcs "$OUTPUT_PATH/shaders.metalar" "$OUTPUT_PATH/shaders.air"
        #rm "$OUTPUT_PATH/shaders.metallib"
        #$METALLIB_COMPILER_PATH -o "$OUTPUT_PATH/shaders.metallib" "$OUTPUT_PATH/shaders.metalar"
        #rm "$OUTPUT_PATH/shaders.air"
        #rm "$OUTPUT_PATH/shaders.metalar"

        raise RuntimeError('Not implemented!')
    elif shader_api == ShaderAPI.VULKAN:
        compiler_path = 'C:/VulkanSDK/1.0.49.0/Bin32/glslangvalidator'
        shader_input_directory = '/Users/derek/git/nedelman/finjin-viewer/assets/shaders/vulkan'
        shader_input_file_extension = 'vulkan'
        vulkan_shader_model_version = '450'

        #"%COMPILER_PATH%" -V shaders.frag -o "%OUTPUT_PATH%450\shaders.frag.spv"
        #"%COMPILER_PATH%" -V shaders.vert -o "%OUTPUT_PATH%450\shaders.vert.spv"

        raise RuntimeError('Not implemented!')
    else:
        raise RuntimeError('Invalid shader API.')

    if combine_shader_files:
        combined_shader_output_filename = shader_output_file_base_name + '.shader'
        combined_shader_output_filepath = os.path.join(shader_output_directory, combined_shader_output_filename)
        combined_shader_output_file = open(combined_shader_output_filepath, 'wb')

    with open(pipeline_state_output_filepath, 'wb') as shader_output_file:
        compiler_processes = CompilerProcesses()

        #Run the compiler across all the variations and shader types
        for name in generated_variations:
            variation = generated_variations[name]

            for shader_type in variation.generate_shader_types:
                compiler_run = compiler_runs[shader_type]
                if compiler_run is not None:
                    #Set up macros
                    macros = []
                    macros += standard_macros
                    macros.append(CompilerMacro('COMPILING_' + compiler_run.shader_entry_point_prefix, '1'))
                    for flag_index in range(ShaderFeatureFlag.COUNT):
                        if variation.fixed_settings.rendering_flags & (1 << flag_index):
                            macros.append(CompilerMacro(ShaderFeatureFlag.STRINGS[flag_index].uppercase_long + '_ENABLED', '1'))
                    for light_type in range(LightType.COUNT):
                        light_count = variation.fixed_settings.get_light_count(light_type)
                        if light_count > 0:
                            macros.append(CompilerMacro(LightType.STRINGS[light_type].uppercase_long + '_LIGHT_COUNT', str(light_count)))
                    macros.append(CompilerMacro('PRIMITIVE_TOPOLOGY_' + PrimitiveTopology.STRINGS[variation.primitive_topology].uppercase_long, str(1)))

                    #Determine shader file output path
                    variation.output_file_paths[shader_type] = os.path.normpath(os.path.join(shader_output_directory, variation.name + '_' + compiler_run.shader_file_name_suffix + '.' + compiler_run.shader_output_file_extension))

                    #Start compiler process
                    shader_input_file_path = os.path.normpath(os.path.join(shader_input_directory, variation.shader_input_file_base_name) + '.' + shader_input_file_extension)
                    command = compiler_run.get_command_string(macros, shader_entry_point_name, variation.output_file_paths[shader_type], shader_input_file_path)
                    if print_commands:
                        print(command)
                    compiler_processes.wait_for_one()
                    proc = subprocess.Popen(command, close_fds = True)
                    compiler_processes.append(proc)

        #Wait for the processes to complete
        compiler_processes.wait_for_all()

        #Write pipeline states
        shader_output_file.write(to_utf8('[pipeline-states]\n'))
        shader_output_file.write(to_utf8('{\n'))

        #Write reference to combined shader file if necessary
        if combine_shader_files:
            shader_output_file.write(to_utf8('\tshader-file-ref=file:///' + combined_shader_output_filename + '\n'))
        shader_output_file.write(to_utf8('\tcount=' + str(len(generated_variations)) + '\n'))

        variation_count = 0
        for name in generated_variations:
            variation = generated_variations[name]

            shader_output_file.write(to_utf8('\t[' + str(variation_count) + ']\n'))
            shader_output_file.write(to_utf8('\t{\n'))
            shader_output_file.write(to_utf8('\t\ttype=' + name + '\n'))
            if variation.root_signature:
                shader_output_file.write(to_utf8('\t\troot-signature=' + variation.root_signature + '\n'))
            shader_output_file.write(to_utf8('\t\tinput-format=' + variation.input_format + '\n'))
            if combine_shader_files:
                for shader_type in variation.generate_shader_types:
                    compiler_run = compiler_runs[shader_type]
                    if compiler_run is not None:
                        #Determine offset
                        shader_offset = combined_shader_output_file.tell()
                        if compiler_run.combined_shader_start_alignment > 1 and shader_offset % compiler_run.combined_shader_start_alignment != 0:
                            #Pad the start offset to fall on the proper byte boundary
                            pad_byte_count = combined_shader_start_alignment - shader_offset % compiler_run.combined_shader_start_alignment
                            for byte_index in range(pad_byte_count):
                                combined_shader_output_file.write(struct.pack('B', 0))
                            shader_offset += pad_byte_count

                        #Read previously generated shader and then write it to combined file
                        shader_bytes = read_binary_file(variation.output_file_paths[shader_type])
                        combined_shader_output_file.write(shader_bytes)
                        if remove_uncombined_shader_files:
                            os.remove(variation.output_file_paths[shader_type])

                        #Write offset/size
                        shader_output_file.write(to_utf8('\t\t' + ShaderType.STRINGS[shader_type].lowercase_long + '-shader-offset=' + str(shader_offset) + ' ' + str(len(shader_bytes)) + '\n'))
            else:
                for shader_type in variation.generate_shader_types:
                    compiler_run = compiler_runs[shader_type]
                    if compiler_run is not None:
                        shader_output_file.write(to_utf8('\t\t' + ShaderType.STRINGS[shader_type].lowercase_long + '-shader-ref=file:///' + name + '_' + compiler_run.shader_file_name_suffix + '.' + compiler_run.shader_output_file_extension + '#' + name + '\n'))
            shader_output_file.write(to_utf8('\t\tprimitive-topology=' + variation.primitive_topology_exported_string(use_long_value_names) + '\n'))

            feature_flags = variation.fixed_settings.rendering_flag_exported_string(use_long_value_names)
            if len(feature_flags) > 0:
                shader_output_file.write(to_utf8('\t\tfeature-flags=' + feature_flags + '\n'))

            lights = variation.fixed_settings.light_type_exported_string(use_long_value_names)
            if len(lights) > 0:
                shader_output_file.write(to_utf8('\t\tlights=' + lights + '\n'))
            shader_output_file.write(to_utf8('\t}\n'))

            variation_count += 1
        shader_output_file.write(to_utf8('}\n'))

    if combine_shader_files:
        combined_shader_output_file.close()

    return result

if __name__ == '__main__':
    result = main()
    sys.exit(result)
