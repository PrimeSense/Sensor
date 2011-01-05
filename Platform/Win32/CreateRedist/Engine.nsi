;/****************************************************************************
;*                                                                           *
;*  PrimeSense Sensor 5.0 Alpha                                              *
;*  Copyright (C) 2010 PrimeSense Ltd.                                       *
;*                                                                           *
;*  This file is part of PrimeSense Common.                                  *
;*                                                                           *
;*  PrimeSense Sensor is free software: you can redistribute it and/or modify*
;*  it under the terms of the GNU Lesser General Public License as published *
;*  by the Free Software Foundation, either version 3 of the License, or     *
;*  (at your option) any later version.                                      *
;*                                                                           *
;*  PrimeSense Sensor is distributed in the hope that it will be useful,     *
;*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
;*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
;*  GNU Lesser General Public License for more details.                      *
;*                                                                           *
;*  You should have received a copy of the GNU Lesser General Public License *
;*  along with PrimeSense Sensor. If not, see <http://www.gnu.org/licenses/>.*
;*                                                                           *
;****************************************************************************/
;

;


;SetCompress off
SetCompress auto
SetCompressor /FINAL /SOLID lzma
SetCompressorDictSize 64
SetDatablockOptimize on

!include "MUI.nsh"
!include "x64.nsh"
!include "winmessages.nsh"
!include LogicLib.nsh
!include "FileFunc.nsh"
;***************
;*** General ***
;***************
!define EE_VER "5.0.0"
!define EE_NAME "PrimeSensor ${EE_VER} for Windows"
!define env_hklm 'HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"'
!define OPENNI_VER "1.0.0"
!define OPENNI_NAME "PrimeSense - OpenNI ${OPENNI_VER} for Windows"
Var OPEN_NI_INST

Name "${EE_NAME}"
OutFile "Output\Sensor-Win32-${EE_VER}.exe"
InstallDir "$PROGRAMFILES\Prime Sense\Sensor"

;*******************************
;*** face Configuration ***
;*******************************
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "Header.bmp"
!define MUI_ABORTWARNING

;*************
;*** Pages ***
;*************
;!insertmacro MUI_PAGE_LICENSE "License.txt" --- Not needed for now
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;****************
;*** Languages***
;****************
!insertmacro MUI_LANGUAGE "English"

;***************
;*** Install ***
;***************

; OpenNI Section
Section "OpenNI"
   
   ; Section is mandatory
   SectionIn RO

   ClearErrors
   ReadRegStr $0 HKLM "Software\OpenNI" "Version"
   IfErrors NotInstalled
   
   
    ; Check the version
   ${If} $0 == ${OPENNI_VER}
    ; That version is already installed
   ${Else}
    ; Is installed with diferent version, install to the same directory
    ReadEnvStr $3 "OPEN_NI_INSTALL_PATH"
    ExecWait "OpenNI-Win32-${OPENNI_VER}.exe /D=$3"
   ${EndIf}


   Goto Done
   
   AddSize 121902
   
   
   NotInstalled:
      ExecWait "OpenNI-Win32-${OPENNI_VER}.exe"

   Done:
   
SectionEnd



; EE Section
Section "Sensor" Sensor
  
  CreateDirectory $INSTDIR
  CreateDirectory "$INSTDIR\Bin"
  CreateDirectory "$INSTDIR\Data"
  
  ; Copy all the files from the redist dir
  File "/oname=$INSTDIR\GPL.txt" ..\redist\GPL.txt
  File "/oname=$INSTDIR\LGPL.txt" ..\redist\LGPL.txt
  File "/oname=$INSTDIR\Bin\XnCore.dll" ..\redist\Bin\XnCore.dll
  File "/oname=$INSTDIR\Bin\XnFormats.dll" ..\redist\Bin\XnFormats.dll
  File "/oname=$INSTDIR\Bin\XnDDK.dll" ..\redist\Bin\XnDDK.dll
  File "/oname=$INSTDIR\Bin\XnDeviceSensorV2.dll" ..\redist\Bin\XnDeviceSensorV2.dll
  File "/oname=$INSTDIR\Bin\XnDeviceFile.dll" ..\redist\Bin\XnDeviceFile.dll
  File "/oname=$INSTDIR\Bin\XnSensorServer.exe" ..\redist\Bin\XnSensorServer.exe
  File "/oname=$INSTDIR\Data\GlobalDefaults.ini" ..\redist\Data\GlobalDefaults.ini
  
  ; Register it in OpenNI
  ReadRegStr $OPEN_NI_INST HKLM "Software\OpenNI" "InstallDir"
  ; Also add OPEN_NI_INSTALL_PATH to current environment (current environment is not updated by writing to registry)
  System::Call 'Kernel32::SetEnvironmentVariableA(t, t) i("OPEN_NI_INSTALL_PATH", "$OPEN_NI_INST").r2'
  ExecWait '"$OPEN_NI_INST\Bin\niReg.exe" "$INSTDIR\Bin\XnDeviceFile.dll"'
  IfErrors FailedToRegister
  ExecWait '"$OPEN_NI_INST\Bin\niReg.exe" "$INSTDIR\Bin\XnDeviceSensorV2.dll" "$INSTDIR\Data"'
  IfErrors FailedToRegister
  
  ; Create an uninstaller
  WriteUninstaller "$INSTDIR\Uninstall EE ${EE_VER}.exe"

  ; Register the .xns file type
  WriteRegStr HKCR ".xns" "" "PrimeSenseRecording"
  WriteRegStr HKCR "PrimeSenseRecording" "" "PrimeSense Recording File"
  WriteRegStr HKCR "PrimeSenseRecording\DefaultIcon" "" "$OPEN_NI_INST\Samples\Bin\Release\NiViewer.exe,0"  
  WriteRegStr HKCR "PrimeSenseRecording\shell\open\command" "" '"$OPEN_NI_INST\Samples\Bin\Release\NiViewer.exe" "%1"'  
  
  ; Add the uninstall option into the control panel add/remove
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${EE_NAME}" "DisplayName"\
  "${EE_NAME} (remove only)"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${EE_NAME}" "UninstallString" \
  "$INSTDIR\Uninstall EE ${EE_VER}.exe"

  WriteRegStr HKLM "Software\PrimeSense\EE" "Version" "${EE_VER}"

  ; Add Env Var
  WriteRegExpandStr ${env_hklm} XN_EE_INSTALL_PATH $INSTDIR
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000 
  Goto Done
  
  FailedToRegister:
	MessageBox MB_OK|MB_ICONSTOP "Installation Failed! Could not register module with OpenNI!"
	Abort "Installation Failed! Could not register module with OpenNI!"
	
  Done:
  
SectionEnd

 
;*****************
;*** Uninstall ***
;*****************
Section "Uninstall"
  ; Unreg from OpenNI
  ReadEnvStr $0 "OPEN_NI_BIN"
  ExecWait '"$0\niReg.exe" -u "$INSTDIR\Bin\XnDeviceFile.dll"'
  ExecWait '"$0\niReg.exe" -u "$INSTDIR\Bin\XnDeviceSensorV2.dll"'
  
  ; Unregister the .xns file type
  DeleteRegKey HKCR ".xns"
  DeleteRegKey HKCR "PrimeSenseRecording"  

  ; Delete the files
  RMDir /r /REBOOTOK $INSTDIR
  RMDir $INSTDIR

  ; Delete the uninstall option from the control panel add/remove
  DeleteRegKey HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\${EE_NAME}"
  DeleteRegKey HKLM "Software\PrimeSense\EE"

  ; Remove the Env Var
  DeleteRegValue ${env_hklm} XN_EE_INSTALL_PATH
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000 
  
SectionEnd