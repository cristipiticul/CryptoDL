################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/architecture/HEBackend/seal/SealCipherText.cpp 

OBJS += \
./src/architecture/HEBackend/seal/SealCipherText.o 

CPP_DEPS += \
./src/architecture/HEBackend/seal/SealCipherText.d 


# Each subdirectory must supply rules for building sources it contributes
src/architecture/HEBackend/seal/%.o: ../src/architecture/HEBackend/seal/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ $(CXXFLAGS) $(INCLUDES) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


