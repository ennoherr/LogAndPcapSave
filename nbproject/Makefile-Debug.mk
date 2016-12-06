#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/LogAndPcapSave/DbgView.o \
	${OBJECTDIR}/LogAndPcapSave/Files.o \
	${OBJECTDIR}/LogAndPcapSave/HddMgmt.o \
	${OBJECTDIR}/LogAndPcapSave/LogAndPcapSave.o \
	${OBJECTDIR}/LogAndPcapSave/LogCapture.o \
	${OBJECTDIR}/LogAndPcapSave/NetCapture.o \
	${OBJECTDIR}/LogAndPcapSave/Process.o \
	${OBJECTDIR}/LogAndPcapSave/Search.o \
	${OBJECTDIR}/LogAndPcapSave/Settings.o \
	${OBJECTDIR}/LogAndPcapSave/TimeInfo.o \
	${OBJECTDIR}/LogAndPcapSave/UniConvert.o \
	${OBJECTDIR}/LogAndPcapSave/dbgprint.o \
	${OBJECTDIR}/LogAndPcapSave/stdafx.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpthread -lpcap

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/logandpcapsave

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/logandpcapsave: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/logandpcapsave ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/LogAndPcapSave/DbgView.o: LogAndPcapSave/DbgView.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/DbgView.o LogAndPcapSave/DbgView.cpp

${OBJECTDIR}/LogAndPcapSave/Files.o: LogAndPcapSave/Files.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/Files.o LogAndPcapSave/Files.cpp

${OBJECTDIR}/LogAndPcapSave/HddMgmt.o: LogAndPcapSave/HddMgmt.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/HddMgmt.o LogAndPcapSave/HddMgmt.cpp

${OBJECTDIR}/LogAndPcapSave/LogAndPcapSave.o: LogAndPcapSave/LogAndPcapSave.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/LogAndPcapSave.o LogAndPcapSave/LogAndPcapSave.cpp

${OBJECTDIR}/LogAndPcapSave/LogCapture.o: LogAndPcapSave/LogCapture.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/LogCapture.o LogAndPcapSave/LogCapture.cpp

${OBJECTDIR}/LogAndPcapSave/NetCapture.o: LogAndPcapSave/NetCapture.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/NetCapture.o LogAndPcapSave/NetCapture.cpp

${OBJECTDIR}/LogAndPcapSave/Process.o: LogAndPcapSave/Process.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/Process.o LogAndPcapSave/Process.cpp

${OBJECTDIR}/LogAndPcapSave/Search.o: LogAndPcapSave/Search.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/Search.o LogAndPcapSave/Search.cpp

${OBJECTDIR}/LogAndPcapSave/Settings.o: LogAndPcapSave/Settings.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/Settings.o LogAndPcapSave/Settings.cpp

${OBJECTDIR}/LogAndPcapSave/TimeInfo.o: LogAndPcapSave/TimeInfo.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/TimeInfo.o LogAndPcapSave/TimeInfo.cpp

${OBJECTDIR}/LogAndPcapSave/UniConvert.o: LogAndPcapSave/UniConvert.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/UniConvert.o LogAndPcapSave/UniConvert.cpp

${OBJECTDIR}/LogAndPcapSave/dbgprint.o: LogAndPcapSave/dbgprint.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/dbgprint.o LogAndPcapSave/dbgprint.cpp

${OBJECTDIR}/LogAndPcapSave/stdafx.o: LogAndPcapSave/stdafx.cpp 
	${MKDIR} -p ${OBJECTDIR}/LogAndPcapSave
	${RM} "$@.d"
	$(COMPILE.cc) -g -D_DEBUG -std=c++11 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/LogAndPcapSave/stdafx.o LogAndPcapSave/stdafx.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/logandpcapsave

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
