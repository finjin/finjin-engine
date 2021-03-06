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
Usage: generate_vulkan_includes.py

Generates the VulkanIncludes.hpp and VulkanIncludes.cpp files in cpp/library/src/finjin/engine/internal/gpu/vulkan.

This script can be run on Windows or Linux, and assumes the vulkan.h is in a hardcoded location,
which itself is version-specific. When a new version of the Vulkan SDK is downloaded, this
script should be re-run.
"""


#Imports-----------------------------------------------------------------------
import os
import shutil
import sys


#Data--------------------------------------------------------------------------
VULKAN_GLOBAL_FUNCTIONS_DISPATCH = (None,)
VULKAN_GLOBAL_FUNCTIONS_CLASS = 'VulkanGlobalFunctions'

VULKAN_INSTANCE_FUNCTIONS_DISPATCH = ('VkInstance', 'VkPhysicalDevice')
VULKAN_INSTANCE_FUNCTIONS_CLASS = 'VulkanInstanceFunctions'

VULKAN_DEVICE_FUNCTIONS_DISPATCH = ('VkDevice', 'VkCommandBuffer', 'VkQueue')
VULKAN_DEVICE_FUNCTIONS_CLASS = 'VulkanDeviceFunctions'

FILE_HEADER = """//Copyright (c) 2017 Finjin
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
//
//This file was generated by generate_vulkan_includes.py."""


#Classes-----------------------------------------------------------------------
class Command(object):
    PLATFORM = 0
    LOADER = 1
    INSTANCE = 2
    DEVICE = 3

    def __init__(self, name, dispatch):
        self.name = name
        self.dispatch = dispatch
        self.ty = self._get_type()

    @staticmethod
    def valid_c_typedef(c):
        return (c.startswith('typedef') and
                c.endswith(');') and
                '*PFN_vkVoidFunction' not in c)

    @classmethod
    def from_c_typedef(cls, c):
        name_begin = c.find('*PFN_vk') + 7
        name_end = c.find(')(', name_begin)
        name = c[name_begin:name_end]

        dispatch_begin = name_end + 2
        dispatch_end = c.find(' ', dispatch_begin)
        dispatch = c[dispatch_begin:dispatch_end]
        if not dispatch.startswith('Vk'):
            dispatch = None

        return cls(name, dispatch)

    def _get_type(self):
        if self.dispatch:
            if self.dispatch in ['VkDevice', 'VkQueue', 'VkCommandBuffer']:
                return self.DEVICE
            else:
                return self.INSTANCE
        else:
            if self.name in ['GetInstanceProcAddr']:
                return self.PLATFORM
            else:
                return self.LOADER

    def __repr__(self):
        return 'Command(name=%s, dispatch=%s)' % (repr(self.name), repr(self.dispatch))

class Extension(object):
    def __init__(self, name, version, guard=None, commands=[]):
        self.name = name
        self.version = version
        self.guard = guard
        self.commands = commands[:]

    def add_command(self, cmd):
        self.commands.append(cmd)

    def __repr__(self):
        lines = []
        lines.append('Extension(name=%s, version=%s, guard=%s, commands=[' % (repr(self.name), repr(self.version), repr(self.guard)))

        for cmd in self.commands:
            lines.append('    %s,' % repr(cmd))

        lines.append('])')

        return '\n'.join(lines)


#Functions---------------------------------------------------------------------
def generate_header_items_of_dispatch(lines, extensions, dispatch_list, class_name):
    lines.append('    struct %s' % class_name)
    lines.append('    {')

    #Constructor
    if class_name == VULKAN_GLOBAL_FUNCTIONS_CLASS:
        lines.append('        %s();' % class_name)
        lines.append('')
        lines.append('        void InitializeGlobal(PFN_vkGetInstanceProcAddr GetInstanceProcAddr, Finjin::Common::Error& error);')
        lines.append('        void ShutdownGlobal();')
    elif class_name == VULKAN_INSTANCE_FUNCTIONS_CLASS:
        lines.append('        VkInstance instance;')
        lines.append('')
        lines.append('        %s();' % class_name)
        lines.append('')
        lines.append('        void InitializeInstance(VkInstance instance, %s& globalFunctions, Finjin::Common::Error& error);' % (VULKAN_GLOBAL_FUNCTIONS_CLASS,))
        lines.append('        void ShutdownInstance(VkAllocationCallbacks* allocationCallbacks);')
    elif class_name == VULKAN_DEVICE_FUNCTIONS_CLASS:
        lines.append('        VkDevice device;')
        lines.append('')
        lines.append('        %s();' % class_name)
        lines.append('')
        lines.append('        void InitializeDevice(VkDevice device, %s& instanceFunctions, Finjin::Common::Error& error);' % (VULKAN_INSTANCE_FUNCTIONS_CLASS,))
        lines.append('        void ShutdownDevice(VkAllocationCallbacks* allocationCallbacks);')
    lines.append('')

    #Some special case members
    if class_name == VULKAN_GLOBAL_FUNCTIONS_CLASS:
        lines.append('        PFN_vkGetInstanceProcAddr GetInstanceProcAddr;')
        lines.append('')

    #Auto-discovered members
    for ext in extensions:
        for cmd in ext.commands:
            if cmd.dispatch in dispatch_list:
                if ext.guard:
                    lines.append('    #ifdef %s' % ext.guard)
                lines.append('        PFN_vk%s %s; //%s' % (cmd.name, cmd.name, ext.name))
                if ext.guard:
                    lines.append('    #endif')

    lines.append('    };')
    lines.append('')

def generate_header(guard, extensions, vk_formats, vk_results):
    lines = []
    lines.append(FILE_HEADER)
    lines.append('')
    lines.append('')
    lines.append('#pragma once')
    lines.append('')
    lines.append('')
    lines.append('//Includes---------------------------------------------------------------------')
    lines.append('#include "vulkan.h"')
    lines.append('#include "finjin/common/Error.hpp"')
    lines.append('')
    lines.append('')
    lines.append('//Types------------------------------------------------------------------------')
    lines.append('namespace Finjin { namespace Engine {')
    lines.append('')

    generate_header_items_of_dispatch(lines, extensions, VULKAN_GLOBAL_FUNCTIONS_DISPATCH, VULKAN_GLOBAL_FUNCTIONS_CLASS) #Global
    generate_header_items_of_dispatch(lines, extensions, VULKAN_INSTANCE_FUNCTIONS_DISPATCH, VULKAN_INSTANCE_FUNCTIONS_CLASS) #Instance
    generate_header_items_of_dispatch(lines, extensions, VULKAN_DEVICE_FUNCTIONS_DISPATCH, VULKAN_DEVICE_FUNCTIONS_CLASS) #Device

    """lines.append('    struct VulkanFunctions : %s, %s, %s' % (VULKAN_GLOBAL_FUNCTIONS_CLASS, VULKAN_INSTANCE_FUNCTIONS_CLASS, VULKAN_DEVICE_FUNCTIONS_CLASS))
    lines.append('    {')
    lines.append('        VulkanFunctions& operator = (VulkanGlobalFunctions& other)')
    lines.append('        {')
    lines.append('            static_cast<VulkanGlobalFunctions&>(*this) = other;')
    lines.append('            return *this;')
    lines.append('        }')
    lines.append('        VulkanFunctions& operator = (VulkanInstanceFunctions& other)')
    lines.append('        {')
    lines.append('            static_cast<VulkanInstanceFunctions&>(*this) = other;')
    lines.append('            return *this;')
    lines.append('        }')
    lines.append('        VulkanFunctions& operator = (VulkanDeviceFunctions& other)')
    lines.append('        {')
    lines.append('            static_cast<VulkanDeviceFunctions&>(*this) = other;')
    lines.append('            return *this;')
    lines.append('        }')
    lines.append('    };')
    lines.append('')"""

    lines.append('    struct VulkanFormat')
    lines.append('    {')
    lines.append('        static const char* ToString(VkFormat format);')
    lines.append('    };')

    lines.append('')

    lines.append('    struct VulkanResult')
    lines.append('    {')
    lines.append('        static const char* ToString(VkResult result);')
    lines.append('        static bool IsError(VkResult result) { return result < 0; }')
    lines.append('    };')

    lines.append('')
    lines.append('} }')

    return '\n'.join(lines)

def get_proc_addr(class_name, cmd, guard=None):
    if class_name == VULKAN_DEVICE_FUNCTIONS_CLASS:
        func = 'GetDeviceProcAddr'
    else:
        func = 'GetInstanceProcAddr'

    if class_name == VULKAN_GLOBAL_FUNCTIONS_CLASS:
        prefix = ''
        dispatch_arg = 'VK_NULL_HANDLE'
    elif class_name == VULKAN_INSTANCE_FUNCTIONS_CLASS:
        prefix = 'this->'
        dispatch_arg = 'instance'
    elif class_name == VULKAN_DEVICE_FUNCTIONS_CLASS:
        prefix = 'this->'
        dispatch_arg = 'device'

    c =  '    this->%s = reinterpret_cast<PFN_vk%s>(%s%s(%s, "vk%s"));' % (cmd.name, cmd.name, prefix, func, dispatch_arg, cmd.name)
    if guard:
        c = ('#ifdef %s\n' % guard) + c + '\n#endif'

    return c

def generate_source_items_of_dispatch(lines, extensions, dispatch_list, class_name):
    #Constructor
    lines.append('%s::%s()' % (class_name, class_name))
    lines.append('{')
    lines.append('    FINJIN_ZERO_ITEM(*this);')
    lines.append('}')
    lines.append('')

    #'Initialize' method
    if class_name == VULKAN_GLOBAL_FUNCTIONS_CLASS:
        lines.append('void %s::InitializeGlobal(PFN_vkGetInstanceProcAddr GetInstanceProcAddr, Finjin::Common::Error& error)' % (class_name,))
        lines.append('{')
        lines.append('    FINJIN_ERROR_METHOD_START(error);')
        lines.append('')
        lines.append('    this->GetInstanceProcAddr = GetInstanceProcAddr;')
        lines.append('')
    elif class_name == VULKAN_INSTANCE_FUNCTIONS_CLASS:
        lines.append('void %s::InitializeInstance(VkInstance instance, %s& globalFunctions, Finjin::Common::Error& error)' % (class_name, VULKAN_GLOBAL_FUNCTIONS_CLASS,))
        lines.append('{')
        lines.append('    FINJIN_ERROR_METHOD_START(error);')
        lines.append('')
        lines.append('    this->instance = instance;')
        lines.append('')
        lines.append('    this->GetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(globalFunctions.GetInstanceProcAddr(instance, "vkGetInstanceProcAddr"));')
    elif class_name == VULKAN_DEVICE_FUNCTIONS_CLASS:
        lines.append('void %s::InitializeDevice(VkDevice device, %s& instanceFunctions, Finjin::Common::Error& error)' % (class_name, VULKAN_INSTANCE_FUNCTIONS_CLASS,))
        lines.append('{')
        lines.append('    FINJIN_ERROR_METHOD_START(error);')
        lines.append('')
        lines.append('    this->device = device;')
        lines.append('')
        lines.append('    this->GetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(instanceFunctions.GetInstanceProcAddr(instanceFunctions.instance, \"vkGetDeviceProcAddr\"));')
        lines.append('    this->GetDeviceProcAddr = reinterpret_cast<PFN_vkGetDeviceProcAddr>(this->GetDeviceProcAddr(device, \"vkGetDeviceProcAddr\"));')
        lines.append('')

    #Auto-discovered members
    for ext in extensions:
        for cmd in ext.commands:
            if cmd.dispatch in dispatch_list:
                if cmd.name != 'GetInstanceProcAddr' and cmd.name != 'GetDeviceProcAddr':
                    lines.append('%s' % (get_proc_addr(class_name, cmd, ext.guard),))

    lines.append('}')
    lines.append('')

    #'Shutdown' method
    if class_name == VULKAN_GLOBAL_FUNCTIONS_CLASS:
        lines.append('void %s::ShutdownGlobal()' % (class_name,))
        lines.append('{')
        lines.append('    FINJIN_ZERO_ITEM(*this);')
        lines.append('}')
        lines.append('')
    elif class_name == VULKAN_INSTANCE_FUNCTIONS_CLASS:
        lines.append('void %s::ShutdownInstance(VkAllocationCallbacks* allocationCallbacks)' % (class_name,))
        lines.append('{')
        lines.append('    if (this->DestroyInstance != nullptr)')
        lines.append('        this->DestroyInstance(this->instance, allocationCallbacks);')
        lines.append('    FINJIN_ZERO_ITEM(*this);')
        lines.append('}')
        lines.append('')
    elif class_name == VULKAN_DEVICE_FUNCTIONS_CLASS:
        lines.append('void %s::ShutdownDevice(VkAllocationCallbacks* allocationCallbacks)' % (class_name,))
        lines.append('{')
        lines.append('    if (this->DestroyDevice != nullptr)')
        lines.append('        this->DestroyDevice(this->device, allocationCallbacks);')
        lines.append('    FINJIN_ZERO_ITEM(*this);')
        lines.append('}')
        lines.append('')

def generate_source(header, extensions, vk_formats, vk_results):
    lines = []
    lines.append(FILE_HEADER)
    lines.append('')
    lines.append('')
    lines.append('//Includes---------------------------------------------------------------------')
    lines.append('#include "FinjinPrecompiled.hpp"')
    lines.append('')
    lines.append('#if FINJIN_TARGET_GPU_SYSTEM == FINJIN_TARGET_GPU_SYSTEM_VULKAN')
    lines.append('')
    lines.append('#include "%s"' % header)
    lines.append('')
    lines.append('using namespace Finjin::Engine;')
    lines.append('')
    lines.append('')
    lines.append('//Implementation---------------------------------------------------------------')

    generate_source_items_of_dispatch(lines, extensions, VULKAN_GLOBAL_FUNCTIONS_DISPATCH, VULKAN_GLOBAL_FUNCTIONS_CLASS) #Global
    generate_source_items_of_dispatch(lines, extensions, VULKAN_INSTANCE_FUNCTIONS_DISPATCH, VULKAN_INSTANCE_FUNCTIONS_CLASS) #Instance
    generate_source_items_of_dispatch(lines, extensions, VULKAN_DEVICE_FUNCTIONS_DISPATCH, VULKAN_DEVICE_FUNCTIONS_CLASS) #Device

    lines.append('const char* VulkanFormat::ToString(VkFormat format)')
    lines.append('{')
    lines.append('    switch (format)')
    lines.append('    {')
    for vk_format in vk_formats:
        lines.append('        case {0}: return "{1}";'.format(vk_format[0], vk_format[1]))
    lines.append('        default: return FINJIN_ENUM_UNKNOWN_STRING;')
    lines.append('    }')
    lines.append('}')

    lines.append('')

    lines.append('const char* VulkanResult::ToString(VkResult result)')
    lines.append('{')
    lines.append('    switch (result)')
    lines.append('    {')
    for vk_result in vk_results:
        lines.append('        case {0}: return "{1}";'.format(vk_result[0], vk_result[1]))
    lines.append('        default: return FINJIN_ENUM_UNKNOWN_STRING;')
    lines.append('    }')
    lines.append('}')

    lines.append('')

    lines.append('#endif')

    return '\n'.join(lines)

def parse_vulkan_header(filename):
    extensions = []

    vk_formats = []
    vk_results = []

    in_vk_format = False
    in_vk_result = False

    with open(filename, 'r') as f:
        current_ext = None
        ext_guard = None
        spec_version = None

        for line in f:
            line = line.strip();

            if in_vk_format:
                if 'VK_FORMAT_BEGIN_RANGE' in line:
                    in_vk_format = False
                else:
                    key_value = line[:-1].split(' = ')
                    vk_formats.append([key_value[1], key_value[0]]) #Put value, key into list
            elif in_vk_result:
                if 'VK_RESULT_BEGIN_RANGE' in line:
                    in_vk_result = False
                else:
                    key_value = line[:-1].split(' = ')
                    vk_results.append([key_value[1], key_value[0]]) #Put value, key into list
            elif line.startswith('typedef enum VkFormat {'):
                in_vk_format = True
            elif line.startswith('typedef enum VkResult {'):
                in_vk_result = True
            elif line.startswith('#define VK_API_VERSION'):
                minor_end = line.rfind(",")
                minor_begin = line.rfind(",", 0, minor_end) + 1
                spec_version = int(line[minor_begin:minor_end])

                #Add core
                current_ext = Extension('VK_core', spec_version)
                extensions.append(current_ext)
            elif Command.valid_c_typedef(line):
                current_ext.add_command(Command.from_c_typedef(line))
            elif line.startswith('#ifdef VK_USE_PLATFORM'):
                guard_begin = line.find(" ") + 1
                ext_guard = line[guard_begin:]
            elif line.startswith('#define') and 'SPEC_VERSION ' in line:
                version_begin = line.rfind(" ") + 1
                spec_version = int(line[version_begin:])
            elif line.startswith('#define') and 'EXTENSION_NAME ' in line:
                name_end = line.rfind('\"')
                name_begin = line.rfind('\"', 0, name_end) + 1
                name = line[name_begin:name_end]

                #Add extension
                current_ext = Extension(name, spec_version, ext_guard)
                extensions.append(current_ext)
            elif ext_guard and line.startswith('#endif') and ext_guard in line:
                ext_guard = None

    return extensions, vk_formats, vk_results


#Main--------------------------------------------------------------------------
def main(args = None):
    #The application entry point
    result = 0 #Assume success

    if sys.platform == 'win32':
        vulkan_include_directory = r'C:\VulkanSDK\1.0.51.0\Include\vulkan'
    else:
        vulkan_include_directory = r'/home/derek/Downloads/VulkanSDK/1.0.51.0/x86_64/include/vulkan'
    extensions, vk_formats, vk_results = parse_vulkan_header(os.path.join(vulkan_include_directory, 'vulkan.h'))

    print_results = False
    if print_results:
        for ext in extensions:
            print('%s = %s' % (ext.name.lower(), repr(ext)))
            print('')

        print('extensions = [')
        for ext in extensions:
            print('    %s,' % ext.name.lower())
        print(']')

        print('vk_formats = [')
        for item in vk_formats:
            print('    [%s, "%s"],' % (item[0], item[1]))
        print(']')

        print('vk_results = [')
        for item in vk_results:
            print('    [%s, "%s"],' % (item[0], item[1]))
        print(']')

    output_directory = '../../cpp/library/src/finjin/engine/internal/gpu/vulkan'
    output_file_base_name = 'VulkanIncludes'
    header_file_include_name = output_file_base_name + '.hpp'
    output_file_path_no_extension = os.path.join(output_directory, output_file_base_name)

    contents = generate_header(header_file_include_name, extensions, vk_formats, vk_results)
    with open(output_file_path_no_extension + '.hpp', 'w') as f:
        print(contents, file=f)
    contents = generate_source(header_file_include_name, extensions, vk_formats, vk_results)
    with open(output_file_path_no_extension + '.cpp', 'w') as f:
        print(contents, file=f)

    #Copy the necessary Vulkan include files to the output directory
    shutil.copyfile(os.path.join(vulkan_include_directory, 'vulkan.h'), os.path.join(output_directory, 'vulkan.h'))
    shutil.copyfile(os.path.join(vulkan_include_directory, 'vk_platform.h'), os.path.join(output_directory, 'vk_platform.h'))

    return result

if __name__ == '__main__':
    result = main()
    sys.exit(result)
