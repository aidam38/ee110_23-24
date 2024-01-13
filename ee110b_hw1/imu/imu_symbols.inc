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

; magnetometer auxiliary register offsets
MAG_XOUT_H_OFFSET .equ 			0x03		; magnetometer X-axis high byte
MAG_XOUT_L_OFFSET .equ 			0x04		; magnetometer X-axis low byte
MAG_YOUT_H_OFFSET .equ 			0x05		; magnetometer Y-axis high byte
MAG_YOUT_L_OFFSET .equ 			0x06		; magnetometer Y-axis low byte
MAG_ZOUT_H_OFFSET .equ 			0x07		; magnetometer Z-axis high byte
MAG_ZOUT_L_OFFSET .equ 			0x08		; magnetometer Z-axis low byte
