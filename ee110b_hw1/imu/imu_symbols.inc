;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                              imu_symbols.inc                               ;
;                                IMU Symbols                                 ;
;                               Include File                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; This file contains symbols to configure an MPU-9250 IMU.
;
; Revision History:

; general
IMU_WRITE .equ              00000000b           ; write bit
IMU_READ .equ               10000000b           ; read bit
IMU_WORD .equ               8                   ; length of IMU address or data

; main register offsets
ACCEL_XOUT_H_OFFSET .equ 			0x3B		; accelerometer X-axis high byte
ACCEL_XOUT_L_OFFSET .equ 			0x3C		; accelerometer X-axis low byte
ACCEL_YOUT_H_OFFSET .equ 			0x3D		; accelerometer Y-axis high byte
ACCEL_YOUT_L_OFFSET .equ 			0x3E		; accelerometer Y-axis low byte
ACCEL_ZOUT_H_OFFSET .equ 			0x3F		; accelerometer Z-axis high byte
ACCEL_ZOUT_L_OFFSET .equ 			0x40		; accelerometer Z-axis low byte 
GYRO_XOUT_H_OFFSET .equ 			0x43		; gyroscope X-axis high byte
GYRO_XOUT_L_OFFSET .equ 			0x44		; gyroscope X-axis low byte
GYRO_YOUT_H_OFFSET .equ 			0x45		; gyroscope Y-axis high byte
GYRO_YOUT_L_OFFSET .equ 			0x46		; gyroscope Y-axis low byte
GYRO_ZOUT_H_OFFSET .equ 			0x47		; gyroscope Z-axis high byte
GYRO_ZOUT_L_OFFSET .equ 			0x48		; gyroscope Z-axis low byte
USER_CTRL_OFFSET .equ               0x6A        ; user control register

; magnetometer auxiliary register offsets
MAG_XOUT_H_OFFSET .equ 			0x03		; magnetometer X-axis high byte
MAG_XOUT_L_OFFSET .equ 			0x04		; magnetometer X-axis low byte
MAG_YOUT_H_OFFSET .equ 			0x05		; magnetometer Y-axis high byte
MAG_YOUT_L_OFFSET .equ 			0x06		; magnetometer Y-axis low byte
MAG_ZOUT_H_OFFSET .equ 			0x07		; magnetometer Z-axis high byte
MAG_ZOUT_L_OFFSET .equ 			0x08		; magnetometer Z-axis low byte

; register values
USER_CTRL_I2C_IF_DIS .equ       0x1 << 4    ; Reset I2C Slave module and put the serial interface in SPI mode only.
USER_CTRL_I2C_MST_EN .equ       0x1 << 5    ; Enable the I2C Master I/F module
USER_CTRL_SIG_COND_RST .equ     0x1         ; Reset accelerometer, gyroscope, and temperature sensors
