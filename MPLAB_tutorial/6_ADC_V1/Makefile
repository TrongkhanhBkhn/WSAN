# MPLAB IDE generated this makefile for use with GNU make.
# Project: MeasureTemperature.mcp
# Date: Mon Apr 30 17:48:46 2012

AS = MPASMWIN.exe
CC = mcc18.exe
LD = mplink.exe
AR = mplib.exe
RM = rm

MeasureTemperature.cof : MeasureTemperature.o
	$(LD) /p18F46K20 /l"C:\Program Files\Microchip\mplabc18\v3.40\lib" "MeasureTemperature.o" /u_CRUNTIME /u_DEBUG /z__MPLAB_BUILD=1 /z__MPLAB_DEBUG=1 /o"MeasureTemperature.cof" /M"MeasureTemperature.map" /W

MeasureTemperature.o : MeasureTemperature.c C:/Program\ Files/Microchip/mplabc18/v3.40/h/delays.h MeasureTemperature.c C:/Program\ Files/Microchip/mplabc18/v3.40/h/p18f46k20.h C:/Program\ Files/Microchip/mplabc18/v3.40/h/p18cxxx.h
	$(CC) -p=18F46K20 "MeasureTemperature.c" -fo="MeasureTemperature.o" -D__DEBUG -Ou- -Ot- -Ob- -Op- -Or- -Od- -Opa-

clean : 
	$(RM) "MeasureTemperature.o" "MeasureTemperature.cof" "MeasureTemperature.hex"

