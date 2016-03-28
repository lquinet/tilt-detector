#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=cof
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/TiltDetector.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/TiltDetector.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../int.c ../taskdesc.c ../main.c ../tsk_task_Main.c ../Kernel/int_man.c ../drivers/drv_i2c.c ../drivers/drv_rs.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1472/int.o ${OBJECTDIR}/_ext/1472/taskdesc.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/tsk_task_Main.o ${OBJECTDIR}/_ext/1966662772/int_man.o ${OBJECTDIR}/_ext/239857660/drv_i2c.o ${OBJECTDIR}/_ext/239857660/drv_rs.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1472/int.o.d ${OBJECTDIR}/_ext/1472/taskdesc.o.d ${OBJECTDIR}/_ext/1472/main.o.d ${OBJECTDIR}/_ext/1472/tsk_task_Main.o.d ${OBJECTDIR}/_ext/1966662772/int_man.o.d ${OBJECTDIR}/_ext/239857660/drv_i2c.o.d ${OBJECTDIR}/_ext/239857660/drv_rs.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1472/int.o ${OBJECTDIR}/_ext/1472/taskdesc.o ${OBJECTDIR}/_ext/1472/main.o ${OBJECTDIR}/_ext/1472/tsk_task_Main.o ${OBJECTDIR}/_ext/1966662772/int_man.o ${OBJECTDIR}/_ext/239857660/drv_i2c.o ${OBJECTDIR}/_ext/239857660/drv_rs.o

# Source Files
SOURCEFILES=../int.c ../taskdesc.c ../main.c ../tsk_task_Main.c ../Kernel/int_man.c ../drivers/drv_i2c.c ../drivers/drv_rs.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/TiltDetector.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F46J50
MP_PROCESSOR_OPTION_LD=18f46j50
MP_LINKER_DEBUG_OPTION=  -u_DEBUGSTACK
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1472/int.o: ../int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/int.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/int.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/int.o   ../int.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/taskdesc.o: ../taskdesc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/taskdesc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/taskdesc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/taskdesc.o   ../taskdesc.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/taskdesc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/taskdesc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/main.o   ../main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/tsk_task_Main.o: ../tsk_task_Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/tsk_task_Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/tsk_task_Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/tsk_task_Main.o   ../tsk_task_Main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/tsk_task_Main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/tsk_task_Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1966662772/int_man.o: ../Kernel/int_man.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1966662772" 
	@${RM} ${OBJECTDIR}/_ext/1966662772/int_man.o.d 
	@${RM} ${OBJECTDIR}/_ext/1966662772/int_man.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1966662772/int_man.o   ../Kernel/int_man.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1966662772/int_man.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1966662772/int_man.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/239857660/drv_i2c.o: ../drivers/drv_i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/239857660" 
	@${RM} ${OBJECTDIR}/_ext/239857660/drv_i2c.o.d 
	@${RM} ${OBJECTDIR}/_ext/239857660/drv_i2c.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/239857660/drv_i2c.o   ../drivers/drv_i2c.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/239857660/drv_i2c.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/239857660/drv_i2c.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/239857660/drv_rs.o: ../drivers/drv_rs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/239857660" 
	@${RM} ${OBJECTDIR}/_ext/239857660/drv_rs.o.d 
	@${RM} ${OBJECTDIR}/_ext/239857660/drv_rs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/239857660/drv_rs.o   ../drivers/drv_rs.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/239857660/drv_rs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/239857660/drv_rs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/_ext/1472/int.o: ../int.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/int.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/int.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/int.o   ../int.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/int.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/int.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/taskdesc.o: ../taskdesc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/taskdesc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/taskdesc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/taskdesc.o   ../taskdesc.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/taskdesc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/taskdesc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/main.o: ../main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/main.o   ../main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/tsk_task_Main.o: ../tsk_task_Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1472" 
	@${RM} ${OBJECTDIR}/_ext/1472/tsk_task_Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1472/tsk_task_Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/tsk_task_Main.o   ../tsk_task_Main.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/tsk_task_Main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/tsk_task_Main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1966662772/int_man.o: ../Kernel/int_man.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1966662772" 
	@${RM} ${OBJECTDIR}/_ext/1966662772/int_man.o.d 
	@${RM} ${OBJECTDIR}/_ext/1966662772/int_man.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1966662772/int_man.o   ../Kernel/int_man.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1966662772/int_man.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1966662772/int_man.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/239857660/drv_i2c.o: ../drivers/drv_i2c.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/239857660" 
	@${RM} ${OBJECTDIR}/_ext/239857660/drv_i2c.o.d 
	@${RM} ${OBJECTDIR}/_ext/239857660/drv_i2c.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/239857660/drv_i2c.o   ../drivers/drv_i2c.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/239857660/drv_i2c.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/239857660/drv_i2c.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/239857660/drv_rs.o: ../drivers/drv_rs.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/239857660" 
	@${RM} ${OBJECTDIR}/_ext/239857660/drv_rs.o.d 
	@${RM} ${OBJECTDIR}/_ext/239857660/drv_rs.o 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -oi -I"../include" -I".." -I"." -ml -oa-  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/239857660/drv_rs.o   ../drivers/drv_rs.c 
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/239857660/drv_rs.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/239857660/drv_rs.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/TiltDetector.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../Kernel/picos18.lib  
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" -l"../Kernel"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/TiltDetector.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}  ..\Kernel\picos18.lib 
else
dist/${CND_CONF}/${IMAGE_TYPE}/TiltDetector.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../Kernel/picos18.lib 
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w  -m"${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map" -l"../Kernel"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/TiltDetector.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}  ..\Kernel\picos18.lib 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
