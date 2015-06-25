#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
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
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/DemoPIC18Coordinator.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=cof
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/DemoPIC18Coordinator.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/385479254/delay.o ${OBJECTDIR}/_ext/385479254/SHT1x.o ${OBJECTDIR}/_ext/385479254/Console.o ${OBJECTDIR}/_ext/385479254/MSPI.o ${OBJECTDIR}/_ext/1472/myZigBee.o ${OBJECTDIR}/_ext/385479254/sralloc.o ${OBJECTDIR}/_ext/1163520513/SymbolTime.o ${OBJECTDIR}/_ext/1163520513/zAPS.o ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o ${OBJECTDIR}/_ext/1163520513/zNVM.o ${OBJECTDIR}/_ext/1163520513/zNWK.o ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o ${OBJECTDIR}/_ext/1163520513/zZDO.o ${OBJECTDIR}/_ext/1472/Coordinator.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/385479254/delay.o.d ${OBJECTDIR}/_ext/385479254/SHT1x.o.d ${OBJECTDIR}/_ext/385479254/Console.o.d ${OBJECTDIR}/_ext/385479254/MSPI.o.d ${OBJECTDIR}/_ext/1472/myZigBee.o.d ${OBJECTDIR}/_ext/385479254/sralloc.o.d ${OBJECTDIR}/_ext/1163520513/SymbolTime.o.d ${OBJECTDIR}/_ext/1163520513/zAPS.o.d ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o.d ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o.d ${OBJECTDIR}/_ext/1163520513/zNVM.o.d ${OBJECTDIR}/_ext/1163520513/zNWK.o.d ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o.d ${OBJECTDIR}/_ext/1163520513/zZDO.o.d ${OBJECTDIR}/_ext/1472/Coordinator.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/385479254/delay.o ${OBJECTDIR}/_ext/385479254/SHT1x.o ${OBJECTDIR}/_ext/385479254/Console.o ${OBJECTDIR}/_ext/385479254/MSPI.o ${OBJECTDIR}/_ext/1472/myZigBee.o ${OBJECTDIR}/_ext/385479254/sralloc.o ${OBJECTDIR}/_ext/1163520513/SymbolTime.o ${OBJECTDIR}/_ext/1163520513/zAPS.o ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o ${OBJECTDIR}/_ext/1163520513/zNVM.o ${OBJECTDIR}/_ext/1163520513/zNWK.o ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o ${OBJECTDIR}/_ext/1163520513/zZDO.o ${OBJECTDIR}/_ext/1472/Coordinator.o


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
	${MAKE} ${MAKE_OPTIONS} -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/DemoPIC18Coordinator.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=18F26K20
MP_PROCESSOR_OPTION_LD=18f26k20
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0xfdc0 -u_DEBUGCODELEN=0x240 -u_DEBUGDATASTART=0xef4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/385479254/delay.o: ../../Microchip/Common/delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/delay.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/delay.o   ../../Microchip/Common/delay.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/delay.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/385479254/SHT1x.o: ../../Microchip/Common/SHT1x.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/SHT1x.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/SHT1x.o   ../../Microchip/Common/SHT1x.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/SHT1x.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/SHT1x.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/385479254/Console.o: ../../Microchip/Common/Console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/Console.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/Console.o   ../../Microchip/Common/Console.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/Console.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/Console.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/385479254/MSPI.o: ../../Microchip/Common/MSPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/MSPI.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/MSPI.o   ../../Microchip/Common/MSPI.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/MSPI.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/MSPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/myZigBee.o: ../myZigBee.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/myZigBee.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/myZigBee.o   ../myZigBee.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/myZigBee.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/myZigBee.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/385479254/sralloc.o: ../../Microchip/Common/sralloc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/sralloc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/sralloc.o   ../../Microchip/Common/sralloc.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/sralloc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/sralloc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/SymbolTime.o: ../../Microchip/ZigBeeStack/SymbolTime.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/SymbolTime.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/SymbolTime.o   ../../Microchip/ZigBeeStack/SymbolTime.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/SymbolTime.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/SymbolTime.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zAPS.o: ../../Microchip/ZigBeeStack/zAPS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zAPS.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zAPS.o   ../../Microchip/ZigBeeStack/zAPS.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zAPS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zAPS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o: ../../Microchip/ZigBeeStack/ZigbeeTasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o   ../../Microchip/ZigBeeStack/ZigbeeTasks.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o: ../../Microchip/ZigBeeStack/zMAC_MRF24J40.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o   ../../Microchip/ZigBeeStack/zMAC_MRF24J40.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zNVM.o: ../../Microchip/ZigBeeStack/zNVM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zNVM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zNVM.o   ../../Microchip/ZigBeeStack/zNVM.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zNVM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zNVM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zNWK.o: ../../Microchip/ZigBeeStack/zNWK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zNWK.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zNWK.o   ../../Microchip/ZigBeeStack/zNWK.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zNWK.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zNWK.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o: ../../Microchip/ZigBeeStack/zPHY_MRF24J40.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o   ../../Microchip/ZigBeeStack/zPHY_MRF24J40.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zZDO.o: ../../Microchip/ZigBeeStack/zZDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zZDO.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zZDO.o   ../../Microchip/ZigBeeStack/zZDO.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zZDO.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zZDO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/Coordinator.o: ../Coordinator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Coordinator.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PICKIT2=1 -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/Coordinator.o   ../Coordinator.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/Coordinator.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Coordinator.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
else
${OBJECTDIR}/_ext/385479254/delay.o: ../../Microchip/Common/delay.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/delay.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/delay.o   ../../Microchip/Common/delay.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/delay.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/delay.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/385479254/SHT1x.o: ../../Microchip/Common/SHT1x.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/SHT1x.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/SHT1x.o   ../../Microchip/Common/SHT1x.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/SHT1x.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/SHT1x.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/385479254/Console.o: ../../Microchip/Common/Console.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/Console.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/Console.o   ../../Microchip/Common/Console.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/Console.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/Console.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/385479254/MSPI.o: ../../Microchip/Common/MSPI.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/MSPI.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/MSPI.o   ../../Microchip/Common/MSPI.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/MSPI.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/MSPI.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/myZigBee.o: ../myZigBee.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/myZigBee.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/myZigBee.o   ../myZigBee.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/myZigBee.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/myZigBee.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/385479254/sralloc.o: ../../Microchip/Common/sralloc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/385479254 
	@${RM} ${OBJECTDIR}/_ext/385479254/sralloc.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/385479254/sralloc.o   ../../Microchip/Common/sralloc.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/385479254/sralloc.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/385479254/sralloc.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/SymbolTime.o: ../../Microchip/ZigBeeStack/SymbolTime.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/SymbolTime.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/SymbolTime.o   ../../Microchip/ZigBeeStack/SymbolTime.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/SymbolTime.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/SymbolTime.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zAPS.o: ../../Microchip/ZigBeeStack/zAPS.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zAPS.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zAPS.o   ../../Microchip/ZigBeeStack/zAPS.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zAPS.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zAPS.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o: ../../Microchip/ZigBeeStack/ZigbeeTasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o   ../../Microchip/ZigBeeStack/ZigbeeTasks.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/ZigbeeTasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o: ../../Microchip/ZigBeeStack/zMAC_MRF24J40.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o   ../../Microchip/ZigBeeStack/zMAC_MRF24J40.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zMAC_MRF24J40.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zNVM.o: ../../Microchip/ZigBeeStack/zNVM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zNVM.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zNVM.o   ../../Microchip/ZigBeeStack/zNVM.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zNVM.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zNVM.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zNWK.o: ../../Microchip/ZigBeeStack/zNWK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zNWK.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zNWK.o   ../../Microchip/ZigBeeStack/zNWK.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zNWK.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zNWK.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o: ../../Microchip/ZigBeeStack/zPHY_MRF24J40.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o   ../../Microchip/ZigBeeStack/zPHY_MRF24J40.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zPHY_MRF24J40.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1163520513/zZDO.o: ../../Microchip/ZigBeeStack/zZDO.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1163520513 
	@${RM} ${OBJECTDIR}/_ext/1163520513/zZDO.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1163520513/zZDO.o   ../../Microchip/ZigBeeStack/zZDO.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1163520513/zZDO.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1163520513/zZDO.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
${OBJECTDIR}/_ext/1472/Coordinator.o: ../Coordinator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} ${OBJECTDIR}/_ext/1472 
	@${RM} ${OBJECTDIR}/_ext/1472/Coordinator.o.d 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION) -I"../../../../../../../../MCC18/h" -I".." -I"../../microchip/Common" -I"../../common" -I"../../../microchip/ZigBeeStack" -I"../../../DemoPIC18Coordinator" -I"../../microchip/ZigBeeStack" -I"../../Microchip/Common" -I"../../Microchip/ZigBeeStack" -ms -oa- -Ls -pa=4 -nw=2066  -I ${MP_CC_DIR}\\..\\h  -fo ${OBJECTDIR}/_ext/1472/Coordinator.o   ../Coordinator.c  -nw=2066
	@${DEP_GEN} -d ${OBJECTDIR}/_ext/1472/Coordinator.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1472/Coordinator.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c18 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/DemoPIC18Coordinator.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "..\zLink.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG -w -l"../../../../../../../../MCC18/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PICKIT2=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/DemoPIC18Coordinator.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
else
dist/${CND_CONF}/${IMAGE_TYPE}/DemoPIC18Coordinator.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE) "..\zLink.lkr"  -p$(MP_PROCESSOR_OPTION_LD)  -w  -w -l"../../../../../../../../MCC18/lib"  -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}\\..\\lib  -o dist/${CND_CONF}/${IMAGE_TYPE}/DemoPIC18Coordinator.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}   
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
