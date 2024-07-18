# T818_VCU_PIX Firmware

## Overview

This repository contains the firmware for the T818_VCU_PIX system, developed for the STM32F767ZI microcontroller. The firmware is based on FreeRTOS and is designed to handle the core functionalities of the drive-by-wire system, including processing input data from the Thrustmaster T818 steering wheel via USB, applying force feedback commands, and managing CAN bus communication. This is essential for controlling the vehicle actuators and receiving feedback from the vehicle.

## Authors

- Alessio Guarini
- Antonio Vitale

## Modules

### CommonDrivers

The `CommonDrivers` directory contains various drivers and utilities used across the system. For detailed information on the modules within this directory, please refer to the [CommonDrivers repository](https://github.com/USBHostSTM32/CommonDrivers/tree/ed16187cb28592cdae1f1a6aea7bc15de712704b).

### Core

The `Core` directory contains the main application code, including the initialization and configuration of the microcontroller, FreeRTOS tasks, and peripheral setups.

### Drivers

The `Drivers` directory includes the hardware abstraction layer (HAL) drivers provided by STMicroelectronics for the STM32F767ZI microcontroller.

### Middlewares

The `Middlewares` directory contains middleware components such as FreeRTOS, which are used to create the real-time operating system environment for the firmware.

### USB_HOST

The `USB_HOST` directory includes the USB host stack and related functionalities to handle communication with the Thrustmaster T818 steering wheel.

## Tasks

### UpdateStateTask

This periodic task runs every 20 ms and is responsible for updating the state and preparing the CAN and USB peripherals for transmission. It ensures that the system's state is up-to-date and ready for the next cycle of operations.

### UrbTxTask

This task runs every 2 ms, which is the maximum rate supported by the Thrustmaster T818 for handling URB interrupt requests. It manages the transmission of URB requests to ensure timely and efficient communication with the steering wheel.

### Default Task

The default task is responsible for initializing the USB host by calling `MX_USB_HOST_Init()`. This task sets up the USB host stack to handle communication with USB devices connected to the system.

## Usage

To use this firmware, include the necessary header files in your project and call the initialization functions for each module as required. Ensure that the USB host and CAN interfaces are properly configured and initialized before using the DBW system. The main application should create the FreeRTOS tasks (`UpdateStateTask`, `UrbTxTask`, and the default task) and start the scheduler.
