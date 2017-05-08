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
Usage: generate_d3d12_feature_levels.py

Reads a .csv containing the contents of the table from
  https://msdn.microsoft.com/en-us/library/windows/desktop/ff476876(v=vs.85).aspx
and reformats it to C++ code that can be copied/pasted into D3D12GpuDescription.cpp

This script can be run on any platform. The only requirement is that the feature-levels.csv
file be in the same directory as this script.

When the content of the table on the Microsoft web site changes:
  1)Copy the content of the feature level table (https://msdn.microsoft.com/en-us/library/windows/desktop/ff476876(v=vs.85).aspx) to the clipboard.
  2)Paste the content into a spreadsheet with OpenOffice or LibreOffice. It should automatically place the data into cells.
  3)Manually remove superscripts and other non-plain-text content. Also remove any '\' and '/' occurrences in the first column. These operations can be done quickly using find/replace.
  4)Copy all the cells, then perform a "Paste Special..." into the upper left cell of a new spreadsheet. In the dialog that appears, select "Copy All" and "Transpose".
  3)Save the spreadsheet as feature-levels.csv, in the same directory as this script. Make sure the files is saved as plain UTF-8 content, with all cell content quoted.
  4)Run this script. A file will be written to StandardFeatureLevelFeatures.cpp.
  5)Open StandardFeatureLevelFeatures.cpp in a texture editor and copy/paste the appropriate content to the top portion of D3D12GpuDescription.cpp, overwriting the existing code.
"""


#Imports-----------------------------------------------------------------------
import os
import sys
import csv


#Functions---------------------------------------------------------------------
def write_feature_level(cell_text, output):
    version = cell_text.split('_')
    major = version[0]
    minor = version[1]
    output.write('D3D_FEATURE_LEVEL_{0}_{1}, Version({0}, {1})'.format(major, minor))

def write_shader_model(cell_text, output):
    if cell_text == '6.0':
        output.write('D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_6_0, Version(6)')
    elif cell_text == '5.1':
        output.write('D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_5_1, Version(5, 1)')
    elif cell_text == '4.x':
        output.write('D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_4_X, Version(4, -1)')
    elif cell_text == '4':
        output.write('D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_4_0, Version(4)')
    elif '4_0_level_9_3' in cell_text:
        output.write('D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_2_0_LEVEL_9_3, Version(2, 0, 3)')
    elif '4_0_level_9_1' in cell_text:
        output.write('D3D12GpuStandardFeatureLevelFeatures::ShaderModel::MODEL_2_0_LEVEL_9_1, Version(2, 0, 1)')
    else:
        raise Exception('Unknown shader model \'{0}\''.format(cell_text))

def write_tier(cell_text, output):
    if cell_text == 'No':
        output.write('D3D12GpuStandardFeatureLevelFeatures::Tier::NONE')
    elif cell_text == 'Tier1':
        output.write('D3D12GpuStandardFeatureLevelFeatures::Tier::TIER_1')
    elif cell_text == 'Tier2':
        output.write('D3D12GpuStandardFeatureLevelFeatures::Tier::TIER_2')
    elif cell_text == 'Tier3':
        output.write('D3D12GpuStandardFeatureLevelFeatures::Tier::TIER_3')
    elif cell_text == 'Optional':
        output.write('D3D12GpuStandardFeatureLevelFeatures::Tier::NOT_REQUIRED')
    else:
        raise Exception('Unknown tier \'{0}\''.format(cell_text))

def write_feature_bool_plus(cell_text, output):
    if cell_text == 'No':
        output.write('D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NO')
    elif cell_text == 'Yes':
        output.write('D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::YES')
    elif cell_text == 'Optional':
        output.write('D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_REQUIRED')
    elif cell_text == 'NA':
        output.write('D3D12GpuStandardFeatureLevelFeatures::FeatureBoolPlus::NOT_AVAILABLE')
    else:
        raise Exception('Unknown feature bool \'{0}\''.format(cell_text))

def write_feature_bool_yes_or_optional(cell_text, output):
    if cell_text == 'Yes':
        output.write('D3D12GpuStandardFeatureLevelFeatures::FeatureBoolYesOrNotRequired::YES')
    elif cell_text == 'Optional':
        output.write('D3D12GpuStandardFeatureLevelFeatures::FeatureBoolYesOrNotRequired::NOT_REQUIRED')
    else:
        raise Exception('Unknown feature yes or optional \'{0}\''.format(cell_text))

def write_bool(cell_text, output):
    if cell_text == 'No':
        output.write('false')
    elif cell_text == 'Yes':
        output.write('true')
    else:
        raise Exception('Unknown boolean \'{0}\''.format(cell_text))

def write_typed_uav_loads(cell_text, output):
    if cell_text == 'No':
        output.write('0')
    elif ' formats,' in cell_text:
        output.write(cell_text.split(' ')[0])
    else:
        raise Exception('Unknown typed UAV load count \'{0}\''.format(cell_text))

def write_count(cell_text, output):
    if cell_text == 'NA':
        output.write('0')
    elif cell_text.startswith('2^32') and cell_text.endswith('1'): #cell_text == '2^32 – 1': #Note: I kept running into an issue with '-' being decoded improperly so this is a quick hack
        output.write('4294967295')
    else:
        output.write(cell_text)

CELL_COLUMN_WRITERS = {
    0 : write_feature_level, #Feature level
    1 : write_shader_model, #Shader model
    2 : write_tier, #Tiled resources
    3 : write_tier, #Conservative rasterization
    4 : write_feature_bool_plus, #Rasterizer order views
    5 : write_feature_bool_plus, #Min/max filters
    6 : write_feature_bool_plus, #Map default buffer
    7 : write_feature_bool_plus, #Shader specified stencil reference value
    8 : write_typed_uav_loads, #Typed unordered access view loads,
    9 : write_bool, #Geometry shaders
    10 : write_bool, #Stream out
    11 : write_feature_bool_plus, #DirectCompute compute shader
    12 : write_bool, #Hull and domain shader
    13 : write_bool, #Texture resource arrays
    14 : write_bool, #Cube map resource arrays
    15 : write_bool, #BC4/5 compression
    16 : write_bool, #BC6/HBC7 compression
    17 : write_bool, #Alpha to coverage
    18 : write_feature_bool_plus, #Extended formats
    19 : write_feature_bool_plus, #XR 10-bit high color format
    20 : write_feature_bool_plus, #Logical operations (output merger)
    21 : write_bool, #Target-independent rasterization
    22 : write_feature_bool_plus, #Multile render target with forced sample count 1
    23 : write_count, #UAV slot count
    24 : write_feature_bool_plus, #UAVs at every stage
    25 : write_count, #Max forced sample count for UAV-only rendering
    26 : write_feature_bool_plus, #Constant buffer offsetting and partial updates
    27 : write_feature_bool_yes_or_optional, #16 bits per pixel formats
    28 : write_count, #Max texture dimension
    29 : write_count, #Max cube map dimension
    30 : write_count, #Max volume extent
    31 : write_count, #Max texture repeat
    32 : write_count, #Max anisotrophy
    33 : write_count, #Max primitive count
    34 : write_count, #Max vertex index
    35 : write_count, #Max input slots
    36 : write_count, #Max simultaneous render targets
    37 : write_bool, #Occlusion queries
    38 : write_bool, #Separate alpha blend
    39 : write_bool, #Mirror once
    40 : write_bool, #Overlapping vertex elements
    41 : write_bool, #Independent write masks
    42 : write_bool, #Instancing
    43 : write_bool, #Non-powers of 2 conditionally
    44 : write_bool, #Non-powers of 2 unconditionally
    }


#Main--------------------------------------------------------------------------
if __name__== '__main__':
    output = open('StandardFeatureLevelFeatures.cpp', 'w') #sys.stdout
    column_count = 45
    row_index = 0
    with open('feature-levels.csv') as csvfile:
        output.write('static const D3D12GpuStandardFeatureLevelFeatures standardFeatureLevelFeatures[] = \n')
        output.write('{\n')
        reader = csv.reader(csvfile, delimiter=',', quotechar='"')
        for row in reader:
            if row_index > 0:
                output.write('    {\n')
                for column_index, cell in enumerate(row):
                    output.write('        ')
                    writer = CELL_COLUMN_WRITERS.get(column_index, None)
                    if writer is not None:
                        writer(row[column_index], output)
                    output.write(',\n')
                output.write('    }, \n')
            row_index += 1
        output.write('};\n')
    if output != sys.stdout:
        output.close()
