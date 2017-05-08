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
Usage: generate_metal_feature_sets.py

Reads the 3 tables contained in the HTML (copied to local files) at
  https://developer.apple.com/metal/availability/
  https://developer.apple.com/metal/limits/
  https://developer.apple.com/metal/capabilities/
and reformats them to C++ code that can be copied/pasted into MetalGpuDescription.mm

This script can be run on any platform. The only requirement is that the files
  availability.html
  limits.html
  capabilities.html
be in the same directory as this script.

When the content of the table on the Apple web site changes:
  1)Copy the content of the 3 pages to the appropriate local files, making sure the data is saved as UTF-8 text.
  2)Run this script. A file will be written to StandardFeatureSets.mm.
  3)Open StandardFeatureSets.cpp in a texture editor and copy/paste the appropriate content to the top portion of MetalGpuDescription.mm, overwriting the existing code.

Due to the fragility of this process, changes to the original content may require changes to this code.
"""


#Imports-----------------------------------------------------------------------
import sys
from bs4 import BeautifulSoup


#Functions---------------------------------------------------------------------
def from_utf8(byte_array):
    return str(byte_array.decode('utf-8', 'ignore'))

def read_binary_file(filename):
    with open(filename, 'rb') as f:
        return f.read()

def read_text_file(filename):
    return from_utf8(read_binary_file(filename))

def format_bool(value):
    if value:
        return 'true'
    else:
        return 'false'

def format_limit(value):
    if value is None or len(value) == 0:
        return '0'
    elif value == 'No limit':
        return '(size_t)-1'
    elif ' bits' in value:
        return str(int(int(value.split(' ')[0]) / 8))
    elif ' GB' in value:
        return str(int(value.split(' ')[0]) * 1000 * 1000 * 1000)
    elif ' MB' in value:
        return str(int(value.split(' ')[0]) * 1000 * 1000)
    elif ' KB' in value:
        return str(int(value.split(' ')[0]) * 1000)
    elif ' B' in value or ' px' in value:
        return value.split(' ')[0]
    else:
        return value

def format_capability(values):
    if values is None:
        return 'MetalFeatureSetCapability::NONE'

    results = ['MetalFeatureSetCapability::' + value.upper() for value in values]
    return ' | '.join(results)

def is_availability_data_row(element):
    paragraph = element.find('p')
    if paragraph and paragraph.get('class')[0] == 'p4' and not paragraph.find('b'):
        return True
    return False

def get_availability_row_of_bools(element):
    heading = None
    values = []
    cell_index = 0
    for cell in element.find_all('td'):
        if cell_index == 0:
            heading = cell.find('span').contents[0]
        else:
            if cell.find('b'):
                values.append(True)
            else:
                values.append(False)
        cell_index += 1
    return heading, values

def is_limits_heading_row(element):
    paragraph = element.find('p')
    if paragraph and paragraph.get('class')[0] == 'p4':
        span = paragraph.find('span')
        if span and span.get('class')[0] == 's1':
            bold = span.find('b')
            if bold:
                return True
    return False

def get_limits_row_of_values(element):
    heading = None
    values = []
    cell_index = 0
    for cell in element.find_all('td'):
        span = cell.find('span')
        if span:
            value = span.contents[0]
        else:
            value = None

        if cell_index == 0:
            heading = value
        else:
            values.append(value)
        cell_index += 1
    return heading, values

def is_capabilities_heading_row(element):
    return is_limits_heading_row(element)

def get_capabilities_row_of_values(element):
    heading = None
    values = []
    cell_index = 0
    for cell in element.find_all('td'):
        span = cell.find('span')
        if span:
            value = []
            for span in cell.find_all('span'):
                value.append(span.contents[0])
        else:
            value = None

        if cell_index == 0:
            heading = value[0]
        else:
            values.append(value)
        cell_index += 1
    return heading, values

class GpuFamily:
    iOS_GPUFamily1_v1 = 0
    iOS_GPUFamily2_v1 = 1
    iOS_GPUFamily1_v2 = 2
    iOS_GPUFamily2_v2 = 3
    iOS_GPUFamily3_v1 = 4
    iOS_GPUFamily1_v3 = 5
    iOS_GPUFamily2_v3 = 6
    iOS_GPUFamily3_v2 = 7
    tvOS_GPUFamily1_v1 = 8
    tvOS_GPUFamily1_v2 = 9
    OSX_GPUFamily1_v1 = 10
    OSX_GPUFamily1_v2 = 11

    COUNT = 12

    STRINGS = (
        'iOS_GPUFamily1_v1',
        'iOS_GPUFamily2_v1',
        'iOS_GPUFamily1_v2',
        'iOS_GPUFamily2_v2',
        'iOS_GPUFamily3_v1',
        'iOS_GPUFamily1_v3',
        'iOS_GPUFamily2_v3',
        'iOS_GPUFamily3_v2',
        'tvOS_GPUFamily1_v1',
        'tvOS_GPUFamily1_v2',
        'OSX_GPUFamily1_v1',
        'OSX_GPUFamily1_v2'
        )

    VERSION_STRINGS = (
        'Version(1, 1)',
        'Version(2, 1)',
        'Version(1, 2)',
        'Version(2, 2)',
        'Version(3, 1)',
        'Version(1, 3)',
        'Version(2, 3)',
        'Version(3, 2)',
        'Version(1, 1)',
        'Version(1, 2)',
        'Version(1, 1)',
        'Version(1, 2)'
        )

    GUARDS = (
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_IOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_TVOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_TVOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS',
        'FINJIN_TARGET_PLATFORM == FINJIN_TARGET_PLATFORM_MACOS'
        )

#Main--------------------------------------------------------------------------
if __name__== '__main__':
    print_values = False

    #Availability
    availability_values = []
    availability_html = read_text_file('availability.html')
    doc = BeautifulSoup(availability_html, 'html.parser')
    for row in doc.find_all('tr'):
        if is_availability_data_row(row):
            heading, values = get_availability_row_of_bools(row)
            if print_values:
                print(heading + ': ' + str(values))
            availability_values.append([heading, values])

    #Limits
    in_limits_values = False
    limits_values = []
    limits_html = read_text_file('limits.html')
    doc = BeautifulSoup(limits_html, 'html.parser')
    for row in doc.find_all('tr'):
        if is_limits_heading_row(row):
            in_limits_values = True
        elif in_limits_values:
            heading, values = get_limits_row_of_values(row)
            if print_values:
                print(heading + ': ' + str(values))
            limits_values.append([heading, values])

    #Capabilities
    in_capabilities_values = False
    capabilities_values = []
    capabilities_html = read_text_file('capabilities.html')
    doc = BeautifulSoup(capabilities_html, 'html.parser')
    for row in doc.find_all('tr'):
        if is_capabilities_heading_row(row):
            in_capabilities_values = True
        elif in_capabilities_values:
            heading, values = get_capabilities_row_of_values(row)
            if print_values:
                print(heading + ': ' + str(values))
            capabilities_values.append([heading, values])

    #Output
    output = open('StandardFeatureSets.mm', 'w') #sys.stdout
    output.write('static const MetalFeatureSet standardFeatureSets[] = \n')
    output.write('{\n')
    for gpuFamilyIndex in range(GpuFamily.COUNT):
        output.write('#if {0}\n'.format(GpuFamily.GUARDS[gpuFamilyIndex]))
        output.write('    //{0}\n'.format(GpuFamily.STRINGS[gpuFamilyIndex]))
        output.write('    {\n')
        output.write('        MTLFeatureSet_{0}, {1},\n'.format(GpuFamily.STRINGS[gpuFamilyIndex], GpuFamily.VERSION_STRINGS[gpuFamilyIndex]))
        output.write('        //Availability\n')
        output.write('        {\n')
        for availability_value in availability_values:
            values = availability_value[1]
            output.write('            {0},\n'.format(format_bool(values[gpuFamilyIndex])))
        output.write('        },\n')
        output.write('        //Limits\n')
        output.write('        {\n')
        for limits_value in limits_values:
            values = limits_value[1]
            output.write('            {0},\n'.format(format_limit(values[gpuFamilyIndex])))
        output.write('        },\n')
        output.write('        //Capabilities\n')
        output.write('        {\n')
        for capabilities_value in capabilities_values:
            values = capabilities_value[1]
            output.write('            {0},\n'.format(format_capability(values[gpuFamilyIndex])))
        output.write('        },\n')
        output.write('    },\n')
        output.write('#endif\n')
    output.write('};\n')
    output.write("""static bool GetStandardFeatureSet(MetalFeatureSet& result, MTLFeatureSet gpuFamily)
{
    for (size_t i = 0; i < FINJIN_COUNT_OF(standardFeatureSets); i++)
    {
        if (standardFeatureSets[i].gpuFamily == gpuFamily)
        {
            result = standardFeatureSets[i];
            return true;
        }
    }
    return false;
}
""")
    if output != sys.stdout:
        output.close()
