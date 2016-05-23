# Microsoft Developer Studio Project File - Name="asnh323" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=asnh323 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "asnh323.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "asnh323.mak" CFG="asnh323 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "asnh323 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "asnh323 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/ITGv4/asnh323", BEAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "asnh323 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /Zi /O2 /I "." /I "..\asn1" /D "NDEBUG" /D "_LIB" /D "__WIN32__" /D "WIN32" /D "_MBCS" /D "ASN1_HAS_IOSTREAM" /D "ASN1_STATIC" /D "H323_STATIC" /YX /FD /c
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\Lib\asnh323s.lib"

!ELSEIF  "$(CFG)" == "asnh323 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /ZI /Od /I "." /I "..\asn1" /I "../asn1" /I "../asnh323/cxx" /D "_DEBUG" /D "_LIB" /D "__WIN32__" /D "WIN32" /D "_MBCS" /D "ASN1_HAS_IOSTREAM" /D "ASN1_STATIC" /D "H323_STATIC" /YX /FD /GZ /c
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\LIB\asnh323sd.lib"

!ENDIF 

# Begin Target

# Name "asnh323 - Win32 Release"
# Name "asnh323 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cxx\addressing_data_elements.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\call_completion_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\call_diversion_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\call_hold_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\call_offer_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\call_park_pickup_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\call_transfer_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\call_waiting_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\h2250v4.asn

!IF  "$(CFG)" == "asnh323 - Win32 Release"

# PROP Intermediate_Dir "cxx"
# Begin Custom Build
IntDir=.\cxx
InputPath=.\h2250v4.asn

BuildCmds= \
	..\Lib\asnparser -o $(IntDir) -v -m H323 -c h245v7.asn h235v2.asn h2250v4.asn h248.asn ro.asn address.asn h4501.asn h4502.asn h4503.asn h4504.asn h4505.asn h4506.asn h4507.asn h4508.asn h4509.asn h45010.asn lgc.asn \
	del $(IntDir)\lgc.cxx \
	del $(IntDir)\lgc.h \
	del $(IntDir)\address.cxx \
	del $(IntDir)\address.h \
	del $(IntDir)\cap.cxx \
	del $(IntDir)\cap.h \
	del $(IntDir)\aliases.cxx \
	del $(IntDir)\cidmodule.cxx \
	del $(IntDir)\cidmodule.h \
	

"$(IntDir)\addressing_data_elements.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\addressing_data_elements.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\addressing_data_elements.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\aliases.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\alternategk_module.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\alternategk_module.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_completion_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_completion_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_completion_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_diversion_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_diversion_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_diversion_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_hold_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_hold_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_hold_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_offer_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_offer_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_offer_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_park_pickup_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_park_pickup_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_park_pickup_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_transfer_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_transfer_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_transfer_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_waiting_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_waiting_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_waiting_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h225_generic_parameters_definition.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h225_generic_parameters_definition.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h235_security_messages.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h235_security_messages.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h235_security_messages.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h235_security_messages_t.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h323_messages.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h323_messages.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h323_messages.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_general_error_list.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_general_error_list.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_supplementary_serviceapdu_structure.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_supplementary_serviceapdu_structure.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_supplementary_serviceapdu_structure.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_aliasaddress.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_aliasaddress.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_aliasaddress.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_alternategk.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_alternategk.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_alternategk.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_callidentifier.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_callidentifier.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_callidentifier.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_supportedprefix.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_supportedprefix.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_supportedprefix.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_transportaddress.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_transportaddress.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_transportaddress.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\manufacturer_specific_service_extension_definition.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\manufacturer_specific_service_extension_definition.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\manufacturer_specific_service_extension_definition.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\manufacturer_specific_service_extension_definition_t.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\media_gateway_control.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\media_gateway_control.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\media_gateway_control.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\message_waiting_indication_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\message_waiting_indication_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\message_waiting_indication_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_capabilitytableentry.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_capabilitytableentry.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_capabilitytableentry.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_openlogicalchannel.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_openlogicalchannel.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_openlogicalchannel.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_t38faxprofile.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_t38faxprofile.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_t38faxprofile.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_transportaddress.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_transportaddress.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_transportaddress.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\multimedia_system_control.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\multimedia_system_control.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\multimedia_system_control.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\name_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\name_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\name_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_apdus.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_apdus.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_apdus.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_apdus_t.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_information_objects.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_information_objects.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_information_objects.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "asnh323 - Win32 Debug"

# PROP Intermediate_Dir "cxx"
# Begin Custom Build
IntDir=.\cxx
InputPath=.\h2250v4.asn

BuildCmds= \
	..\Lib\asnparser -o $(IntDir) -v  -m H323 -c h245v7.asn h235v2.asn h2250v4.asn h248.asn ro.asn address.asn h4501.asn h4502.asn h4503.asn h4504.asn h4505.asn h4506.asn h4507.asn h4508.asn h4509.asn h45010.asn lgc.asn \
	del $(IntDir)\lgc.cxx \
	del $(IntDir)\lgc.h \
	del $(IntDir)\address.cxx \
	del $(IntDir)\address.h \
	del $(IntDir)\cap.cxx \
	del $(IntDir)\cap.h \
	del $(IntDir)\aliases.cxx \
	del $(IntDir)\cidmodule.cxx \
	del $(IntDir)\cidmodule.h \
	

"$(IntDir)\addressing_data_elements.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\addressing_data_elements.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\addressing_data_elements.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\aliases.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\alternategk_module.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\alternategk_module.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_completion_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_completion_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_completion_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_diversion_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_diversion_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_diversion_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_hold_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_hold_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_hold_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_offer_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_offer_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_offer_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_park_pickup_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_park_pickup_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_park_pickup_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_transfer_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_transfer_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_transfer_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_waiting_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_waiting_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\call_waiting_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h225_generic_parameters_definition.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h225_generic_parameters_definition.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h235_security_messages.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h235_security_messages.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h235_security_messages.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h235_security_messages_t.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h323_messages.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h323_messages.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h323_messages.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_general_error_list.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_general_error_list.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_supplementary_serviceapdu_structure.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_supplementary_serviceapdu_structure.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\h4501_supplementary_serviceapdu_structure.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_aliasaddress.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_aliasaddress.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_aliasaddress.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_alternategk.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_alternategk.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_alternategk.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_callidentifier.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_callidentifier.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_callidentifier.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_supportedprefix.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_supportedprefix.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_supportedprefix.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_transportaddress.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_transportaddress.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\hm_transportaddress.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\manufacturer_specific_service_extension_definition.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\manufacturer_specific_service_extension_definition.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\manufacturer_specific_service_extension_definition.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\manufacturer_specific_service_extension_definition_t.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\media_gateway_control.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\media_gateway_control.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\media_gateway_control.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\message_waiting_indication_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\message_waiting_indication_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\message_waiting_indication_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_capabilitytableentry.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_capabilitytableentry.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_capabilitytableentry.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_openlogicalchannel.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_openlogicalchannel.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_openlogicalchannel.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_t38faxprofile.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_t38faxprofile.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_t38faxprofile.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_transportaddress.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_transportaddress.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\msc_transportaddress.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\multimedia_system_control.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\multimedia_system_control.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\multimedia_system_control.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\name_operations.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\name_operations.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\name_operations.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_apdus.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_apdus.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_apdus.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_apdus_t.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_information_objects.cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_information_objects.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(IntDir)\remote_operations_information_objects.inl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cxx\h225_generic_parameters_definition.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\h235_security_messages.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\h323_messages.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\h4501_general_error_list.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\h4501_supplementary_serviceapdu_structure.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_aliasaddress.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_alternategk.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_callidentifier.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_supportedprefix.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_transportaddress.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\media_gateway_control.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\message_waiting_indication_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_capabilitytableentry.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_openlogicalchannel.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_t38faxprofile.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_transportaddress.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\multimedia_system_control.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\name_operations.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\remote_operations_apdus.cxx
# End Source File
# Begin Source File

SOURCE=.\cxx\remote_operations_information_objects.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\cxx\addressing_data_elements.h
# End Source File
# Begin Source File

SOURCE=.\cxx\addressing_data_elements.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\call_completion_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\call_completion_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\call_diversion_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\call_diversion_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\call_hold_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\call_hold_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\call_offer_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\call_offer_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\call_park_pickup_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\call_park_pickup_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\call_transfer_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\call_transfer_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\call_waiting_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\call_waiting_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\h225_generic_parameters_definition.h
# End Source File
# Begin Source File

SOURCE=.\cxx\h235_security_messages.h
# End Source File
# Begin Source File

SOURCE=.\cxx\h235_security_messages.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\h323_messages.h
# End Source File
# Begin Source File

SOURCE=.\cxx\h323_messages.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\h4501_general_error_list.h
# End Source File
# Begin Source File

SOURCE=.\cxx\h4501_supplementary_serviceapdu_structure.h
# End Source File
# Begin Source File

SOURCE=.\cxx\h4501_supplementary_serviceapdu_structure.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_aliasaddress.h
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_aliasaddress.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_alternategk.h
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_alternategk.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_callidentifier.h
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_callidentifier.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_supportedprefix.h
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_supportedprefix.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_transportaddress.h
# End Source File
# Begin Source File

SOURCE=.\cxx\hm_transportaddress.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\manufacturer_specific_service_extension_definition.h
# End Source File
# Begin Source File

SOURCE=.\cxx\manufacturer_specific_service_extension_definition.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\media_gateway_control.h
# End Source File
# Begin Source File

SOURCE=.\cxx\media_gateway_control.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\message_waiting_indication_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\message_waiting_indication_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_capabilitytableentry.h
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_capabilitytableentry.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_openlogicalchannel.h
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_openlogicalchannel.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_t38faxprofile.h
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_t38faxprofile.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_transportaddress.h
# End Source File
# Begin Source File

SOURCE=.\cxx\msc_transportaddress.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\multimedia_system_control.h
# End Source File
# Begin Source File

SOURCE=.\cxx\multimedia_system_control.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\name_operations.h
# End Source File
# Begin Source File

SOURCE=.\cxx\name_operations.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\remote_operations_apdus.h
# End Source File
# Begin Source File

SOURCE=.\cxx\remote_operations_apdus.inl
# End Source File
# Begin Source File

SOURCE=.\cxx\remote_operations_information_objects.h
# End Source File
# Begin Source File

SOURCE=.\cxx\remote_operations_information_objects.inl
# End Source File
# End Group
# End Target
# End Project
