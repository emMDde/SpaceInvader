\# SpaceInvader - STM32 Embedded System



Implementation of the classic "Space Invaders" game, built from scratch for the STM32F429I-DISC1 board. The core game logic is object-oriented and written in C++, while the low-level hardware drivers and peripheral configurations are handled in C using HAL. I intentionally avoided external GUI frameworks like LVGL or TouchGFX. The graphics engine is entirely custom, writing directly to the frame buffer in the external SDRAM. All peripheral drivers and helper functions were written from the ground up, making STM32 HAL the highest level of external abstraction used in this project.



\# System Architecture



The diagram below illustrates the system architecture, including the communication between the microcontroller and peripheral devices:



\[System Architecture Diagram](doc/architektura.png)



\# Key Features



\- Custom Graphics Engine: Direct frame buffer management and rendering optimization.

\- Display Controller (LTDC): Custom library developed for TFT display handling.

\- Memory Management (FMC): FMC controller configuration to support external SDRAM.

\- Controls: Integration of touch panel and gyroscope controls via SPI/I2C communication.



\# Tools \& Platform



\- Evaluation Board: STM32F429I-DISC1 (STM32F429ZI ARM Cortex-M4)

\- IDE: STM32CubeIDE

\- Language: C + C++



\# MCU pinout



Pin configuration and peripheral mapping:



\[MCUpinout](doc/mcu.png)



