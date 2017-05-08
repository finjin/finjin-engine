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
Usage: generate_dxgi_includes.py

Generates the DXGIIncludes.hpp and DXGIIncludes.cpp files in cpp/library/src/finjin/engine.

This script must be run on Windows, and assumes the dxgiformat.h is in a hardcoded location,
which itself is version-specific. When a new version of the Windows SDK is downloaded, this
script should be re-run.
"""


#Imports-----------------------------------------------------------------------
import os
import shutil
import sys


#Data--------------------------------------------------------------------------
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
//This file was generated by generate_dxgi_includes.py."""


#Functions---------------------------------------------------------------------
def generate_header(header, dxgi_formats):
    lines = []
    lines.append(FILE_HEADER)
    lines.append('')
    lines.append('')
    lines.append('#pragma once')
    lines.append('')
    lines.append('')
    lines.append('//Includes---------------------------------------------------------------------')
    #lines.append('#include "finjin/common/Error.hpp"')
    lines.append('#if FINJIN_TARGET_PLATFORM_IS_WINDOWS')
    lines.append('    #include <wtypes.h>')
    lines.append('    #include <dxgi1_4.h>')
    lines.append('#else')
    lines.append('    enum DXGI_FORMAT')
    lines.append('    {')
    for dxgi_format in dxgi_formats:
        lines.append('        {1} = {0},'.format(dxgi_format[0], dxgi_format[1]))
    lines.append('    };')
    lines.append('#endif')
    lines.append('')
    lines.append('')
    lines.append('//Types------------------------------------------------------------------------')
    lines.append('namespace Finjin { namespace Engine {')
    lines.append('')
    lines.append('    struct DXGIFormat')
    lines.append('    {')
    lines.append('        static const char* ToString(DXGI_FORMAT format);')
    lines.append('    };')
    lines.append('')
    lines.append('} }')

    return '\n'.join(lines)

def generate_source(header, dxgi_formats):
    lines = []
    lines.append(FILE_HEADER)
    lines.append('')
    lines.append('')
    lines.append('//Includes---------------------------------------------------------------------')
    lines.append('#include "FinjinPrecompiled.hpp"')
    lines.append('#include "%s"' % header)
    lines.append('')
    lines.append('using namespace Finjin::Engine;')
    lines.append('')
    lines.append('')
    lines.append('//Implementation---------------------------------------------------------------')

    lines.append('const char* DXGIFormat::ToString(DXGI_FORMAT format)')
    lines.append('{')
    lines.append('    switch (format)')
    lines.append('    {')
    for dxgi_format in dxgi_formats:
        lines.append('        case {0}: return "{1}";'.format(dxgi_format[0], dxgi_format[1]))
    lines.append('        default: return "<unknown>";')
    lines.append('    }')
    lines.append('}')

    return '\n'.join(lines)

def parse_dxgi_format_header(filename):
    dxgi_formats = []

    in_dxgi_format = False

    with open(filename, 'r') as f:
        for line in f:
            line = line.strip();

            if in_dxgi_format:
                if '} DXGI_FORMAT;' in line:
                    in_dxgi_format = False
                else:
                    key_value = line[:-1].split(' = ')
                    if len(key_value) == 2:
                        dxgi_formats.append([key_value[1].strip(), key_value[0].strip()]) #Put value, key into list
            elif line.startswith('typedef enum DXGI_FORMAT'):
                in_dxgi_format = True

    return dxgi_formats


#Main--------------------------------------------------------------------------
def main(args = None):
    #The application entry point
    result = 0 #Assume success

    dxgi_include_directory = r'c:\Program Files (x86)\Windows Kits\10\Include\10.0.14393.0\shared'
    dxgi_formats = parse_dxgi_format_header(os.path.join(dxgi_include_directory, 'dxgiformat.h'))

    print_results = False
    if print_results:
        print('dxgi_formats = [')
        for item in dxgi_formats:
            print('    [%s, "%s"],' % (item[0], item[1]))
        print(']')

    output_directory = '../../cpp/library/src/finjin/engine'
    output_file_base_name = 'DXGIIncludes'
    header_file_include_name = output_file_base_name + '.hpp'
    output_file_path_no_extension = os.path.join(output_directory, output_file_base_name)

    contents = generate_header(header_file_include_name, dxgi_formats)
    with open(output_file_path_no_extension + '.hpp', 'w') as f:
        print(contents, file=f)
    contents = generate_source(header_file_include_name, dxgi_formats)
    with open(output_file_path_no_extension + '.cpp', 'w') as f:
        print(contents, file=f)

    return result

if __name__ == '__main__':
    result = main()
    sys.exit(result)