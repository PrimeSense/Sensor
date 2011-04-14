#/****************************************************************************
#*                                                                           *
#*  PrimeSense Sensor 5.0 Alpha                                              *
#*  Copyright (C) 2010 PrimeSense Ltd.                                       *
#*                                                                           *
#*  This file is part of PrimeSense Common.                                  *
#*                                                                           *
#*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
#*  it under the terms of the GNU Lesser General Public License as published *
#*  by the Free Software Foundation, either version 3 of the License, or     *
#*  (at your option) any later version.                                      *
#*                                                                           *
#*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
#*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
#*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
#*  GNU Lesser General Public License for more details.                      *
#*                                                                           *
#*  You should have received a copy of the GNU Lesser General Public License *
#*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
#*                                                                           *
#****************************************************************************/
#

#
import os
import sys
import shutil
import win32con, pywintypes, win32api
import re
import subprocess
from xml.dom.minidom import parse, parseString

#------------Check args---------------------------------------------#

Make_Doxy=1
vc_build_bits = "32"
vc_build_type = "/Rebuild"
VC_version = 9

if len(sys.argv) in [4,5]:
    if sys.argv[1] == 'n':
        Make_Doxy=0
    if sys.argv[2] == '64':
        vc_build_bits = "64"
    if sys.argv[3] == 'n':
        vc_build_type = "/Build"
    if len(sys.argv) > 4:
        if sys.argv[4] == '10':
            VC_version = 10    

CONFIG_XML = parse("Engine_Config.xml")
SDK_VER = str(CONFIG_XML.getElementsByTagName("VERSION_NUMBER")[0].firstChild.data)

ROOT_DIR = os.path.abspath(os.path.dirname(sys.argv[0]))
REDIST_DIR = os.path.join(ROOT_DIR, "..", "Redist")
OUTPUT_DIR = os.path.join(ROOT_DIR, "Output")
SCRIPT_DIR = os.getcwd()

print 'work dir of redist.py:'
print SCRIPT_DIR

def finish_script(exit_code):
    os.chdir(SCRIPT_DIR)
    #logging.shutdown()
    exit(exit_code)

def regx_replace(findStr,repStr,filePath):
    "replaces all findStr by repStr in file filePath using regualr expression"
    findStrRegx = re.compile(findStr)
    tempName=filePath+'~~~'
    input = open(filePath)
    output = open(tempName,'w')
    for s in input:
        output.write(findStrRegx.sub(repStr,s))
    output.close()
    input.close()
    os.remove(filePath)
    os.rename(tempName,filePath)

def get_reg_values(reg_key, value_list):
    # open the reg key
    try:
        reg_key = win32api.RegOpenKeyEx(*reg_key)
    except pywintypes.error as e:
        raise Exception("Failed to open registry key!")
    # Get the values
    try:
        values = [(win32api.RegQueryValueEx(reg_key, name), data_type) for name, data_type in value_list]
        # values list of ((value, type), expected_type)
        for (value, data_type), expected in values:
            if data_type != expected:
                raise Exception("Bad registry value type! Expected %d, got %d instead." % (expected, data_type))
        # values okay, leave only values
        values = [value for ((value, data_type), expected) in values]
    except pywintypes.error as e:
        raise Exception("Failed to get registry value!")
    finally:
        try:
            win32api.RegCloseKey(reg_key)
        except pywintypes.error as e:
            # We don't care if reg key close failed...
            pass
    return tuple(values)




# remove output dir
if os.path.exists(REDIST_DIR):
    os.system("rmdir /S /Q \"" + REDIST_DIR + "\"")
if os.path.exists(OUTPUT_DIR):
    os.system("rmdir /S /Q \"" + OUTPUT_DIR + "\"")
os.mkdir(REDIST_DIR)
os.mkdir(OUTPUT_DIR)

try:
    VS_NEED_UPGRADE = 0
    MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\9.0")
    MSVC_VALUES = [("InstallDir", win32con.REG_SZ)]
    VS_INST_DIR = get_reg_values(MSVC_KEY, MSVC_VALUES)[0]
except Exception as e:
    VC_version = 10

if VC_version == 10:    
    VS_NEED_UPGRADE = 1
    MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\10.0")
    MSVC_VALUES = [("InstallDir", win32con.REG_SZ)]
    VS_INST_DIR = get_reg_values(MSVC_KEY, MSVC_VALUES)[0]



# build
print("Building...")

os.chdir(os.path.join(ROOT_DIR, "..", "Build"))

if VS_NEED_UPGRADE == 1:
	res = os.system("\"" + VS_INST_DIR + "devenv\" " + "Engine.sln" + " /upgrade > ..\\CreateRedist\\Output\\build.log")
	if res != 0:
	    raise Exception("build failed!")
res = os.system("\"" + VS_INST_DIR + "devenv\" " + "Engine.sln" + " /build Release > ..\\CreateRedist\\Output\\build.log")
if res != 0:
    raise Exception("build failed!")
os.chdir(SCRIPT_DIR)

print("Copying into redist folder...")

# create folder structure
os.mkdir(os.path.join(REDIST_DIR, "Bin"))
os.mkdir(os.path.join(REDIST_DIR, "Data"))

# copy EPL
shutil.copy(os.path.join(ROOT_DIR, "..", "..", "..", "GPL.txt"), REDIST_DIR)
shutil.copy(os.path.join(ROOT_DIR, "..", "..", "..", "GPL.txt"), OUTPUT_DIR)
shutil.copy(os.path.join(ROOT_DIR, "..", "..", "..", "LGPL.txt"), REDIST_DIR)
shutil.copy(os.path.join(ROOT_DIR, "..", "..", "..", "LGPL.txt"), OUTPUT_DIR)

# copy binaries
RELEASE_DIR = os.path.join(ROOT_DIR, "..", "Bin", "Release")
for file in os.listdir(RELEASE_DIR):
    if os.path.splitext(file)[1] in [".dll", ".exe"]:
        shutil.copy(os.path.join(RELEASE_DIR, file), os.path.join(REDIST_DIR, "Bin"))

# copy data
DATA_DIR = os.path.join(ROOT_DIR, "..", "..", "..", "Data")
for file in os.listdir(DATA_DIR):
    shutil.copy(os.path.join(DATA_DIR, file), os.path.join(REDIST_DIR, "Data"))
    
# create installer
print("Creating installer...")
os.chdir(SCRIPT_DIR + "\\EE_NI")
#print "* move XnLeanDeviceSensorV2.dll"
#os.system("move /Y " + WORK_DIR + "\\Platform\\Win32\\Bin\\XnLeanDeviceSensorV2.dll \\Platform\\Win32\\Bin\\XnDeviceSensorV2.dll")

print "* Set BinaryOnlyRedist=True"
os.system("attrib -r Includes\\EENIVariables.wxi")
regx_replace("BinaryOnlyRedist=(.*)", "BinaryOnlyRedist=True?>", "Includes\\EENIVariables.wxi")

logFilename = "BuildEngine"



print "* Build EE_NI.wixproj"

if VS_NEED_UPGRADE == 1:
    res = subprocess.call("\""+VS_INST_DIR + "devenv\" EE_NI.sln /upgrade /out " + SCRIPT_DIR + "\\Output\\" + logFilename + ".txt",close_fds=True)
    if res != 0:
        raise Exception("Failed upgrade installer!")

res = subprocess.call("\""+VS_INST_DIR + "devenv\" EE_NI.wixproj /build \"release|x86"\
		+"\" /out "+SCRIPT_DIR+"\\Output\\" + logFilename + ".txt",close_fds=True)
if res != 0:
    raise Exception("Failed creating installer!")

currDir = os.getcwd()
print currDir
moveCmd = "move bin\\Release\\en-US\\EE_NI.msi " + SCRIPT_DIR + "\\Output\\Sensor-Win-OpenSource" + str(vc_build_bits) + "-" + SDK_VER + ".msi"
print moveCmd + "..."
os.system(moveCmd)
#print "* Move installers"
#os.system("move .\\Output\\*.msi " + SCRIPT_DIR + "\\Output")

#NSIS_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\NSIS")
#NSIS_VALUES = [("InstallLocation", win32con.REG_EXPAND_SZ)]
#NSIS_INST_DIR = get_reg_values(NSIS_KEY, NSIS_VALUES)[0]
# make installer
#os.chdir(ROOT_DIR)
#res = os.system("\"" + NSIS_INST_DIR + "\\makensis.exe\" " + "Engine.nsi" + " > ..\\CreateRedist\\Output\\nsis.log")
#if res != 0:
    #raise Exception("Failed creating installer!")

print("Done.")
finish_script(0)