################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
A51_UPPER_SRCS += \
../src/SILABS_STARTUP.A51 

C_SRCS += \
../src/EFM8SB1_SleepMode_PortMatchWake.c \
../src/InitDevice.c \
../src/Interrupts.c \
../src/SmaRTClock.c \
../src/power.c 

OBJS += \
./src/EFM8SB1_SleepMode_PortMatchWake.OBJ \
./src/InitDevice.OBJ \
./src/Interrupts.OBJ \
./src/SILABS_STARTUP.OBJ \
./src/SmaRTClock.OBJ \
./src/power.OBJ 


# Each subdirectory must supply rules for building sources it contributes
src/%.OBJ: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Compiler'
	C51 "@$(patsubst %.OBJ,%.__i,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/EFM8SB1_SleepMode_PortMatchWake.OBJ: D:/simplicitystdio/developer/toolchains/keil_8051/9.53/INC/STDLIB.H D:/simplicitystdio/developer/toolchains/keil_8051/9.53/INC/STRING.H D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Register_Enums.h E:/redtempturebee/inc/InitDevice.h E:/redtempturebee/inc/SmaRTClock.h E:/redtempturebee/inc/power.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Defs.h

src/InitDevice.OBJ: D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Register_Enums.h E:/redtempturebee/inc/InitDevice.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/compiler_defs.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Defs.h E:/redtempturebee/inc/SmaRTClock.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h

src/Interrupts.OBJ: D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Register_Enums.h E:/redtempturebee/inc/InitDevice.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Defs.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

src/%.OBJ: ../src/%.A51
	@echo 'Building file: $<'
	@echo 'Invoking: Keil 8051 Assembler'
	AX51 "@$(patsubst %.OBJ,%.__ia,$@)" || $(RC)
	@echo 'Finished building: $<'
	@echo ' '

src/SmaRTClock.OBJ: D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Register_Enums.h E:/redtempturebee/inc/SmaRTClock.h E:/redtempturebee/inc/power.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Defs.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h

src/power.OBJ: D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Register_Enums.h E:/redtempturebee/inc/SmaRTClock.h E:/redtempturebee/inc/power.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/EFM8SB1/inc/SI_EFM8SB1_Defs.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/si_toolchain.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdint.h D:/simplicitystdio/developer/sdks/si8051/v3/Device/shared/si8051Base/stdbool.h


