################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Instancia

dependents:
	-cd /home/utnso/tp-2018-1c-Kernel-Panic/manejoDeSocket/Debug && $(MAKE) clean all

# Tool invocations
Instancia: $(OBJS) $(USER_OBJS) /home/utnso/tp-2018-1c-Kernel-Panic/manejoDeSocket/Debug/libmanejoDeSocket.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L/usr/include/math.h -L"/home/utnso/tp-2018-1c-Kernel-Panic/manejoDeSocket/Debug" -o "Instancia" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) Instancia
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/tp-2018-1c-Kernel-Panic/manejoDeSocket/Debug/libmanejoDeSocket.so:

-include ../makefile.targets
