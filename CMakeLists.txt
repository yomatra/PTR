cmake_minimum_required(VERSION 4.4)
project(PRT C)

set(CMAKE_C_STANDARD 11)

include_directories(eMIOS-OPWM/eMIOS-OPWM/Project_Headers)
include_directories(PLL_sysclk/PLL_sysclk/Project_Headers)

add_executable(PRT
        eMIOS-OPWM/eMIOS-OPWM/Project_Headers/Exceptions.h
        eMIOS-OPWM/eMIOS-OPWM/Project_Headers/IntcInterrupts.h
        eMIOS-OPWM/eMIOS-OPWM/Project_Headers/MPC5606B.h
        eMIOS-OPWM/eMIOS-OPWM/Project_Headers/MPC5606B_HWInit.h
        eMIOS-OPWM/eMIOS-OPWM/Project_Headers/typedefs.h
        eMIOS-OPWM/eMIOS-OPWM/Project_Settings/Startup_Code/__ppc_eabi_init.c
        eMIOS-OPWM/eMIOS-OPWM/Project_Settings/Startup_Code/MPC5606B_HWInit.c
        eMIOS-OPWM/eMIOS-OPWM/Project_Settings/Startup_Code/MPC5606B_init_flash.c
        eMIOS-OPWM/eMIOS-OPWM/Project_Settings/Startup_Code/MPC5606B_init_ram.c
        eMIOS-OPWM/eMIOS-OPWM/Project_Settings/Startup_Code/MPC5606B_Startup.c
        eMIOS-OPWM/eMIOS-OPWM/Sources/Exceptions.c
        eMIOS-OPWM/eMIOS-OPWM/Sources/IntcInterrupts.c
        eMIOS-OPWM/eMIOS-OPWM/Sources/ivor_branch_table.c
        eMIOS-OPWM/eMIOS-OPWM/Sources/main.c
        PLL_sysclk/PLL_sysclk/Project_Headers/Exceptions.h
        PLL_sysclk/PLL_sysclk/Project_Headers/IntcInterrupts.h
        PLL_sysclk/PLL_sysclk/Project_Headers/MPC5606B.h
        PLL_sysclk/PLL_sysclk/Project_Headers/MPC5606B_HWInit.h
        PLL_sysclk/PLL_sysclk/Project_Headers/typedefs.h
        PLL_sysclk/PLL_sysclk/Project_Settings/Startup_Code/__ppc_eabi_init.c
        PLL_sysclk/PLL_sysclk/Project_Settings/Startup_Code/MPC5606B_HWInit.c
        PLL_sysclk/PLL_sysclk/Project_Settings/Startup_Code/MPC5606B_init_flash.c
        PLL_sysclk/PLL_sysclk/Project_Settings/Startup_Code/MPC5606B_init_ram.c
        PLL_sysclk/PLL_sysclk/Project_Settings/Startup_Code/MPC5606B_Startup.c
        PLL_sysclk/PLL_sysclk/Sources/Exceptions.c
        PLL_sysclk/PLL_sysclk/Sources/IntcInterrupts.c
        PLL_sysclk/PLL_sysclk/Sources/ivor_branch_table.c
        PLL_sysclk/PLL_sysclk/Sources/main.c)
