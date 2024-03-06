################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../ee110b_hw5.cfg 

CMD_SRCS += \
../cc26x2r1f.cmd 

C_SRCS += \
../RTOS_demo.c \
../app.c \
../haiku.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./RTOS_demo.d \
./app.d \
./haiku.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./RTOS_demo.obj \
./app.obj \
./haiku.obj 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"RTOS_demo.obj" \
"app.obj" \
"haiku.obj" 

C_DEPS__QUOTED += \
"RTOS_demo.d" \
"app.d" \
"haiku.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../RTOS_demo.c" \
"../app.c" \
"../haiku.c" 


