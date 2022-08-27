################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F/%.obj: ../middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="D:/ccsProjects/workspace_v12/Tivana" --include_path="C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/ccsProjects/workspace_v12/Tivana/middlewares/FreeRTOS" --include_path="D:/ccsProjects/workspace_v12/Tivana/middlewares/FreeRTOS/Source/include" --include_path="D:/ccsProjects/workspace_v12/Tivana/middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="D:/ccsProjects/workspace_v12/Tivana/Debug" --include_path="D:/ccsProjects/workspace_v12/Tivana/middlewares/FreeRTOS-Plus-CLI" --define=ccs="ccs" --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F/$(basename $(<F)).d_raw" --obj_directory="middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F/%.obj: ../middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/TivaWare_C_Series-2.2.0.295" --include_path="D:/ccsProjects/workspace_v12/Tivana" --include_path="C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-arm_20.2.7.LTS/include" --include_path="D:/ccsProjects/workspace_v12/Tivana/middlewares/FreeRTOS" --include_path="D:/ccsProjects/workspace_v12/Tivana/middlewares/FreeRTOS/Source/include" --include_path="D:/ccsProjects/workspace_v12/Tivana/middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F" --include_path="D:/ccsProjects/workspace_v12/Tivana/Debug" --include_path="D:/ccsProjects/workspace_v12/Tivana/middlewares/FreeRTOS-Plus-CLI" --define=ccs="ccs" --define=PART_TM4C1294NCPDT -g --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F/$(basename $(<F)).d_raw" --obj_directory="middlewares/FreeRTOS/Source/portable/CCS/ARM_CM4F" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


