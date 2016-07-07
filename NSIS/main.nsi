Name "ProjectX"

OutFile "ProjectXInstall.exe"

InstallDir $LOCALAPPDATA\ProjectX

RequestExecutionLevel admin

; Registry Key for program
InstallDirRegKey HKLM "Software\ProjectX" "Install_Dir"

; Pages
PageEx license
	LicenseText "ProjectX licence"
	LicenseData license.txt
	LicenseForceSelection checkbox
PageExEnd

Page components
Page directory
Page instfiles

; Uninstallation Pages
UninstPage uninstConfirm
UninstPage instFiles

; different sections:
Section "ProjectX (required)"
	
	SectionIn RO
	
	SetOutPath $INSTDIR
	
	File /r game\*.*
	
	CreateDirectory "$SMPROGRAMS\ProjectX"
	
	CreateShortcut "$SMPROGRAMS\ProjectX\ProjectX.lnk" "$INSTDIR\ProjectX.exe" "" "$LOCALAPPDATA\resources" 0
	
	CreateShortcut "$DESKTOP\ProjectX.lnk" "$INSTDIR\ProjectX.exe" "" "$LOCALAPPDATA\resources" 0
	
	; Write the installation path into the registry
	WriteRegStr HKLM SOFTWARE\ProjectX "Install_Dir" "$INSTDIR"
	 
	; Write the uninstall keys for Windows
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ProjectX" "DisplayName" "ProjectX"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ProjectX" "UninstallString" '"$INSTDIR\Uninstall.exe"'
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ProjectX" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ProjectX" "NoRepair" 1
	WriteUninstaller "Uninstall.exe"
  
SectionEnd

SectionGroup "Development Tools"
	Section "ProjectX Scripting"
	
		SetOutPath $INSTDIR
		
		File /r atom
		File /r .atom
		
		# Shortcut
		CreateShortcut "$SMPROGRAMS\ProjectX\ProjectX Scripting.lnk" "$INSTDIR\atom\atom.exe" "$INSTDIR\res" "$INSTDIR\atom\atom.exe" 0		

	SectionEnd

	Section "ProjectX Source Control"

		SetOutPath $INSTDIR
		
		File /r gitkraken
		
		CreateShortcut "$SMPROGRAMS\ProjectX\ProjectX Source Control.lnk" "$INSTDIR\gitkraken\gitkraken.exe" "$INSTDIR\res" "$LOCALAPPDATA\gitkraken\gitkraken.exe" 0
		
	SectionEnd
	Section "ProjectX Documentation"
		
		SetOutPath $INSTDIR\docs
		
		File ProjectXBook.pdf
		
	SectionEnd
SectionGroupEnd

Section "Previewer"

	SetOutPath $INSTDIR
	
	File /r preview
	
	CreateShortcut "$DESKTOP\ProjectX Sprite Previewer.lnk" "$INSTDIR\preview\Previewer.exe" "" "$INSTDIR\preview\Previewer.exe" 0
SectionEnd

Section "Uninstall"
	
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ProjectX"
	DeleteRegKey HKLM SOFTWARE\ProjectX
	
	RMDir /r $INSTDIR
	
SectionEnd