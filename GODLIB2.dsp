# Microsoft Developer Studio Project File - Name="GODLIB" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GODLIB - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GODLIB.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GODLIB.mak" CFG="GODLIB - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GODLIB - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GODLIB - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GODLIB - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Zp2 /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "GODLIB - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GODLIB___Win32_Debug"
# PROP BASE Intermediate_Dir "GODLIB___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "GODLIB___Win32_Debug"
# PROP Intermediate_Dir "GODLIB___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /Zp2 /MDd /W4 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D dDEBUG=1 /D DIRECTINPUT_VERSION=0x0800 /D dCLI=1 /D dAUDIO_MIXER=1 /D dSYSTEM_D3D=1 /D dMEMORY_GUARD=1 /D dLINKFILE=1 /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "GODLIB - Win32 Release"
# Name "GODLIB - Win32 Debug"
# Begin Group "SOURCE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DUMMY.C
# End Source File
# End Group
# Begin Group "ASSERT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ASSERT\ASSERT.C
# End Source File
# Begin Source File

SOURCE=.\ASSERT\ASSERT.H
# End Source File
# End Group
# Begin Group "ASSET"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ASSET\ASSET.C
# End Source File
# Begin Source File

SOURCE=.\ASSET\ASSET.H
# End Source File
# Begin Source File

SOURCE=.\ASSET\CONTEXT.C
# End Source File
# Begin Source File

SOURCE=.\ASSET\CONTEXT.H
# End Source File
# Begin Source File

SOURCE=.\ASSET\PACKAGE.C
# End Source File
# Begin Source File

SOURCE=.\ASSET\PACKAGE.H
# End Source File
# Begin Source File

SOURCE=.\ASSET\PKG_DIR.C
# End Source File
# Begin Source File

SOURCE=.\ASSET\PKG_DIR.H
# End Source File
# Begin Source File

SOURCE=.\ASSET\PKG_LNK.C
# End Source File
# Begin Source File

SOURCE=.\ASSET\PKG_LNK.H
# End Source File
# Begin Source File

SOURCE=.\ASSET\RELOCATE.C
# End Source File
# Begin Source File

SOURCE=.\ASSET\RELOCATE.H
# End Source File
# End Group
# Begin Group "AUDIO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AUDIO\AMIXER.C
# End Source File
# Begin Source File

SOURCE=.\AUDIO\AMIXER.H
# End Source File
# Begin Source File

SOURCE=.\AUDIO\AMIXER_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\AUDIO\AUDIO.C
# End Source File
# Begin Source File

SOURCE=.\AUDIO\AUDIO.H
# End Source File
# Begin Source File

SOURCE=.\AUDIO\AUDIO_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\AUDIO\REL_SPL.C
# End Source File
# Begin Source File

SOURCE=.\AUDIO\REL_SPL.H
# End Source File
# Begin Source File

SOURCE=.\AUDIO\SSD.C
# End Source File
# Begin Source File

SOURCE=.\AUDIO\SSD.H
# End Source File
# Begin Source File

SOURCE=.\AUDIO\SSD_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "BASE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BASE\BASE.C
# End Source File
# Begin Source File

SOURCE=.\BASE\BASE.H
# End Source File
# End Group
# Begin Group "BIOS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BIOS\BIOS.H
# End Source File
# Begin Source File

SOURCE=.\BIOS\BIOS_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "BLITTER"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BLITTER\BLITTER.C
# End Source File
# Begin Source File

SOURCE=.\BLITTER\BLITTER.H
# End Source File
# End Group
# Begin Group "CHUNKY"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CHUNKY\CHUNKY.C
# End Source File
# Begin Source File

SOURCE=.\CHUNKY\CHUNKY.H
# End Source File
# Begin Source File

SOURCE=.\CHUNKY\CHUNKY_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "CLI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CLI\CLI.C
# End Source File
# Begin Source File

SOURCE=.\CLI\CLI.H
# End Source File
# End Group
# Begin Group "CLOCK"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CLOCK\CLOCK.C
# End Source File
# Begin Source File

SOURCE=.\CLOCK\CLOCK.H
# End Source File
# Begin Source File

SOURCE=.\CLOCK\CLOCK_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "COOKIE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\COOKIE\COOKIE.C
# End Source File
# Begin Source File

SOURCE=.\COOKIE\COOKIE.H
# End Source File
# End Group
# Begin Group "DEBUG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DEBUG\DEBUG.C
# End Source File
# Begin Source File

SOURCE=.\DEBUG\DEBUG.H
# End Source File
# End Group
# Begin Group "DEBUGLOG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DEBUGLOG\DEBUGLOG.C
# End Source File
# Begin Source File

SOURCE=.\DEBUGLOG\DEBUGLOG.H
# End Source File
# End Group
# Begin Group "DRIVE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DRIVE\DRIVE.C
# End Source File
# Begin Source File

SOURCE=.\DRIVE\DRIVE.H
# End Source File
# End Group
# Begin Group "EXCEPT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EXCEPT\EXCEPT.C
# End Source File
# Begin Source File

SOURCE=.\EXCEPT\EXCEPT.H
# End Source File
# Begin Source File

SOURCE=.\EXCEPT\EXCEPT_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "FADE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FADE\FADE.C
# End Source File
# Begin Source File

SOURCE=.\FADE\FADE.H
# End Source File
# Begin Source File

SOURCE=.\FADE\FADE_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "FE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FE\FED.C
# End Source File
# Begin Source File

SOURCE=.\FE\FED.H
# End Source File
# Begin Source File

SOURCE=.\FE\FEDPARSE.C
# End Source File
# Begin Source File

SOURCE=.\FE\FEDPARSE.H
# End Source File
# Begin Source File

SOURCE=.\FE\R_FED.C
# End Source File
# Begin Source File

SOURCE=.\FE\R_FED.H
# End Source File
# Begin Source File

SOURCE=.\FE\REL_FED.C
# End Source File
# Begin Source File

SOURCE=.\FE\REL_FED.H
# End Source File
# End Group
# Begin Group "FILE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FILE\FILE.C
# End Source File
# Begin Source File

SOURCE=.\FILE\FILE.H
# End Source File
# End Group
# Begin Group "FONT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FONT\FONT.C
# End Source File
# Begin Source File

SOURCE=.\FONT\FONT.H
# End Source File
# Begin Source File

SOURCE=.\FONT\REL_BFB.C
# End Source File
# Begin Source File

SOURCE=.\FONT\REL_BFB.H
# End Source File
# End Group
# Begin Group "FONT8X8"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FONT8X8\FONT8X8.C
# End Source File
# Begin Source File

SOURCE=.\FONT8X8\FONT8X8.H
# End Source File
# Begin Source File

SOURCE=.\FONT8X8\FONTDATA.C
# End Source File
# End Group
# Begin Group "GEMDOS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GEMDOS\GEMDOS.C
# End Source File
# Begin Source File

SOURCE=.\GEMDOS\GEMDOS.H
# End Source File
# Begin Source File

SOURCE=.\GEMDOS\GEMDOS_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "GRAPHIC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GRAPHIC\GRAPHIC.C
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRAPHIC.H
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRAPHIC.I
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRF_16.C
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRF_16.H
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRF_16_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRF_4.C
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRF_4.H
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRF_4_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRF_B4_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\GRAPHIC\GRF_TC_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GUI\GUI.C
# End Source File
# Begin Source File

SOURCE=.\GUI\GUI.H
# End Source File
# Begin Source File

SOURCE=.\GUI\GUIDATA.C
# End Source File
# Begin Source File

SOURCE=.\GUI\GUIDATA.H
# End Source File
# Begin Source File

SOURCE=.\GUI\GUIEDIT.C
# End Source File
# Begin Source File

SOURCE=.\GUI\GUIEDIT.H
# End Source File
# Begin Source File

SOURCE=.\GUI\GUIFS.C
# End Source File
# Begin Source File

SOURCE=.\GUI\GUIFS.H
# End Source File
# Begin Source File

SOURCE=.\GUI\GUIPARSE.C
# End Source File
# Begin Source File

SOURCE=.\GUI\GUIPARSE.H
# End Source File
# Begin Source File

SOURCE=.\GUI\R_GUI.C
# End Source File
# Begin Source File

SOURCE=.\GUI\R_GUI.H
# End Source File
# End Group
# Begin Group "HASHLIST"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HASHLIST\HASHLIST.C
# End Source File
# Begin Source File

SOURCE=.\HASHLIST\HASHLIST.H
# End Source File
# End Group
# Begin Group "HASHTREE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HASHTREE\HASHTREE.C
# End Source File
# Begin Source File

SOURCE=.\HASHTREE\HASHTREE.H
# End Source File
# End Group
# Begin Group "IKBD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\IKBD\IKBD.C
# End Source File
# Begin Source File

SOURCE=.\IKBD\IKBD.H
# End Source File
# Begin Source File

SOURCE=.\IKBD\IKBD_DI.C
# End Source File
# Begin Source File

SOURCE=.\IKBD\IKBD_DI.H
# End Source File
# Begin Source File

SOURCE=.\IKBD\IKBD_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\IKBD\IKBD_SDL.C
# End Source File
# Begin Source File

SOURCE=.\IKBD\IKBD_SDL.H
# End Source File
# End Group
# Begin Group "INPUT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\INPUT\INPUT.C
# End Source File
# Begin Source File

SOURCE=.\INPUT\INPUT.H
# End Source File
# End Group
# Begin Group "LINEA"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LINEA\LINEA.H
# End Source File
# Begin Source File

SOURCE=.\LINEA\LINEA_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "LINKFILE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LINKFILE\LINKFILE.C
# End Source File
# Begin Source File

SOURCE=.\LINKFILE\LINKFILE.H
# End Source File
# End Group
# Begin Group "MEMORY"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MEMORY\MEMORY.C
# End Source File
# Begin Source File

SOURCE=.\MEMORY\MEMORY.H
# End Source File
# End Group
# Begin Group "MFP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MFP\MFP.C
# End Source File
# Begin Source File

SOURCE=.\MFP\MFP.H
# End Source File
# Begin Source File

SOURCE=.\MFP\MFP_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "MUSIC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MUSIC\PINKNOTE.C
# End Source File
# Begin Source File

SOURCE=.\MUSIC\PINKNOTE.H
# End Source File
# Begin Source File

SOURCE=.\MUSIC\PNKNOT_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\MUSIC\SND.C
# End Source File
# Begin Source File

SOURCE=.\MUSIC\SND.H
# End Source File
# Begin Source File

SOURCE=.\MUSIC\SND_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "PACKER"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PACKER\ARI_DEC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\ARI_ENC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\BWT_DEC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\BWT_ENC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\GODPACK.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\GODPACK.H
# End Source File
# Begin Source File

SOURCE=.\PACKER\LZ77_DEC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\LZ77_ENC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\LZ77_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PACKER\LZ77B_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PACKER\LZ77BDEC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\LZ77BENC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\MTF_DEC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\MTF_ENC.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\PACKER.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\PACKER.H
# End Source File
# Begin Source File

SOURCE=.\PACKER\PACKER_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\PACKER\RLE.C
# End Source File
# Begin Source File

SOURCE=.\PACKER\RLE_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "PICTYPES"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PICTYPES\ANI.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\ART.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\ART.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\ASB.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\CANVAS.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\CANVAS.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\CANVASIC.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\CANVASIC.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\CE1.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\COLQUANT.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\COLQUANT.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\DEGAS.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\DEGAS.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\DOO.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\FLM.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\GFX.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\GFX.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\GIF.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\GIF.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\GOD.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\GOD.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\GSM.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\GSM.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\IC1.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\IFF.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\IMG.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\MAC.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\MBK.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\MUR.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\NEO.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\NEO.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\OCTTREE.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\OCTTREE.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\PAC.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\REL_GSM.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\REL_GSM.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\RGB.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\SEQ.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\SPC.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\SPS.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\SPU.H
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\TGA.C
# End Source File
# Begin Source File

SOURCE=.\PICTYPES\TGA.H
# End Source File
# End Group
# Begin Group "PROFILER"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PROFILER\PROFILE.C
# End Source File
# Begin Source File

SOURCE=.\PROFILER\PROFILE.H
# End Source File
# Begin Source File

SOURCE=.\PROFILER\PROFILER.C
# End Source File
# Begin Source File

SOURCE=.\PROFILER\PROFILER.H
# End Source File
# Begin Source File

SOURCE=.\PROFILER\PROFILES.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "PROGRAM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PROGRAM\PROGRAM.C
# End Source File
# Begin Source File

SOURCE=.\PROGRAM\PROGRAM.H
# End Source File
# End Group
# Begin Group "RANDOM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\RANDOM\RANDOM.C
# End Source File
# Begin Source File

SOURCE=.\RANDOM\RANDOM.H
# End Source File
# End Group
# Begin Group "SCRNGRAB"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SCRNGRAB\SCRNGRAB.C
# End Source File
# Begin Source File

SOURCE=.\SCRNGRAB\SCRNGRAB.H
# End Source File
# End Group
# Begin Group "SPRITE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SPRITE\ASPRIT_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SPRITE\ASPRITE.C
# End Source File
# Begin Source File

SOURCE=.\SPRITE\ASPRITE.H
# End Source File
# Begin Source File

SOURCE=.\SPRITE\REL_ASB.C
# End Source File
# Begin Source File

SOURCE=.\SPRITE\REL_ASB.H
# End Source File
# Begin Source File

SOURCE=.\SPRITE\REL_BSB.C
# End Source File
# Begin Source File

SOURCE=.\SPRITE\REL_BSB.H
# End Source File
# Begin Source File

SOURCE=.\SPRITE\REL_RSB.C
# End Source File
# Begin Source File

SOURCE=.\SPRITE\REL_RSB.H
# End Source File
# Begin Source File

SOURCE=.\SPRITE\RSPRIT_S.S
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\SPRITE\RSPRITE.C
# End Source File
# Begin Source File

SOURCE=.\SPRITE\RSPRITE.H
# End Source File
# Begin Source File

SOURCE=.\SPRITE\SPRITE.C
# End Source File
# Begin Source File

SOURCE=.\SPRITE\SPRITE.H
# End Source File
# End Group
# Begin Group "SYSTEM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SYSTEM\SYSTEM.C
# End Source File
# Begin Source File

SOURCE=.\SYSTEM\SYSTEM.H
# End Source File
# Begin Source File

SOURCE=.\SYSTEM\SYSTEM_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "TOKENISE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\TOKENISE\TOKENISE.C
# End Source File
# Begin Source File

SOURCE=.\TOKENISE\TOKENISE.H
# End Source File
# End Group
# Begin Group "VBL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VBL\VBL.C
# End Source File
# Begin Source File

SOURCE=.\VBL\VBL.H
# End Source File
# Begin Source File

SOURCE=.\VBL\VBL_S.INC

!IF  "$(CFG)" == "GODLIB - Win32 Release"

!ELSEIF  "$(CFG)" == "GODLIB - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\VBL\VBL_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "VECTOR"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VECTOR\VECTOR.H
# End Source File
# Begin Source File

SOURCE=.\VECTOR\VECTOR_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "VIDEO"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\VIDEO\VID_D3D.C
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VID_D3D.H
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VID_IMG.C
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VID_IMG.H
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VID_SDL.C
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VID_SDL.H
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VIDEL.C
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VIDEO.C
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VIDEO.H
# End Source File
# Begin Source File

SOURCE=.\VIDEO\VIDEO_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "WIPE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\WIPE\WIPE.H
# End Source File
# Begin Source File

SOURCE=.\WIPE\WIPE_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "XBIOS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\XBIOS\XBIOS.C
# End Source File
# Begin Source File

SOURCE=.\XBIOS\XBIOS.H
# End Source File
# Begin Source File

SOURCE=.\XBIOS\XBIOS_S.S
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "STRING"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\STRING\STRING.C
# End Source File
# Begin Source File

SOURCE=.\STRING\STRING.H
# End Source File
# Begin Source File

SOURCE=.\STRING\STRLIST.C
# End Source File
# Begin Source File

SOURCE=.\STRING\STRLIST.H
# End Source File
# End Group
# Begin Group "ENCRYPT"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ENCRYPT\ENCRYPT.C
# End Source File
# Begin Source File

SOURCE=.\ENCRYPT\ENCRYPT.H
# End Source File
# End Group
# Begin Group "LINKLIST"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LINKLIST\GOD_LL.H
# End Source File
# End Group
# Begin Group "ELFHASH"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ELFHASH\ELFHASH.C
# End Source File
# Begin Source File

SOURCE=.\ELFHASH\ELFHASH.H
# End Source File
# End Group
# Begin Group "PLATFORM"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PLATFORM\PLATFORM.C
# End Source File
# Begin Source File

SOURCE=.\PLATFORM\PLATFORM.H
# End Source File
# End Group
# Begin Group "SCREEN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SCREEN\SCREEN.C
# End Source File
# Begin Source File

SOURCE=.\SCREEN\SCREEN.H
# End Source File
# End Group
# Begin Group "KERNEL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\KERNEL\KERNEL.C
# End Source File
# Begin Source File

SOURCE=.\KERNEL\KERNEL.H
# End Source File
# End Group
# Begin Group "MAIN"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MAIN\GOD_MAIN.C
# End Source File
# Begin Source File

SOURCE=.\MAIN\GOD_MAIN.H
# End Source File
# End Group
# Begin Group "CUTSCENE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CUTSCENE\CUT_SYS.C
# End Source File
# Begin Source File

SOURCE=.\CUTSCENE\CUT_SYS.H
# End Source File
# Begin Source File

SOURCE=.\CUTSCENE\CUTPARSE.C
# End Source File
# Begin Source File

SOURCE=.\CUTSCENE\CUTPARSE.H
# End Source File
# Begin Source File

SOURCE=.\CUTSCENE\CUTSCENE.C
# End Source File
# Begin Source File

SOURCE=.\CUTSCENE\CUTSCENE.H
# End Source File
# Begin Source File

SOURCE=.\CUTSCENE\REL_CUT.C
# End Source File
# Begin Source File

SOURCE=.\CUTSCENE\REL_CUT.H
# End Source File
# End Group
# Begin Group "DOCS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DOCS\CUTSCENE.TXT
# End Source File
# End Group
# Begin Group "MATHS"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
