;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                                                                            ;
;                                   imu.s                                    ;
;                                 IMU Driver                                 ;
;                                                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; This file contains the initialization and operation code of a MPU-9250 IMU,
; using the SPI interface. This implementation also includes the capability
; of reading the accelerometer, gyroscope, and magnetometer data.
; 
; This file defines functions:
;		InitIMU
;		GetAccelX
;		GetAccelY
;		GetAccelZ
;		GetGyroX
;		GetGyroY
;		GetGyroZ
;		GetMagX
;		GetMagY
;		GetMagZ

; Revision History:
;		12/5/23	Adam Krivka		initial revision



; local includes
	.include "../std.inc"
	.include "imu_symbols.inc"

; import functions from other files
	.ref SerialGetData
	.ref SerialSendData

; export functions to other files
	.def InitIMU
	.def GetAccelX
	.def GetAccelY
	.def GetAccelZ
	.def GetGyroX
	.def GetGyroY
	.def GetGyroZ
	.def GetMagX
	.def GetMagY
	.def GetMagZ


; TODO: InitIMU
InitIMU:
	PUSH	{LR}			; save return address

	MOV		R0, #((USER_CTRL_OFFSET << IMU_WORD) | (IMU_WRITE | USER_CTRL_I2C_IF_DIS | USER_CTRL_I2C_MST_EN))
	BL		SerialSendData

	POP		{LR}			; restor return address
	BX 		LR				; return

; ReadAccelGyroReg
;
; Description:			Reads a register from the accelerometer and gyroscope.
;						Registers are 8 bits wide. 
;
; Arguments:			R0 = register address
; Returns:				R0 = register value
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

ReadAccelGyroReg:
	PUSH	{LR}						; save return address and used registers

	ORR		R0, #IMU_READ				; specify read operation
	LSL		R0, #IMU_WORD				; move address to the first byte

	BL		SerialSendData				; send the register address
	BL		SerialGetData				; get the register value

	POP		{LR}						; restore return address and used registers
	BX		LR							; return


; ReadMagnetReg
;
; Description:			Reads a register from the magnetometer.
;						Registers are 8 bits wide. 
;
; Arguments:			R0 = register address
; Returns:				R0 = register value
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

ReadMagnetReg:
	; TODO: select magnetometer through I2C somehow



; GetAccelX, GetAccelY, GetAccelZ
;
; Description:			Reads the X-axis, Y-axis, or Z-axis accelerometer value.
;
; Arguments:			None.
; Returns:				R0 = X-axis, Y-axis, or Z-axis accelerometer value
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

GetAccelX:
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #ACCEL_XOUT_H_OFFSET	; set register address
	BL		ReadAccelGyroReg			; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8						
	MOV		R0, #ACCEL_XOUT_L_OFFSET	; set register address
	BL		ReadAccelGyroReg			; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return

GetAccelY:
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #ACCEL_YOUT_H_OFFSET	; set register address
	BL		ReadAccelGyroReg			; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8						
	MOV		R0, #ACCEL_YOUT_L_OFFSET	; set register address
	BL		ReadAccelGyroReg			; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return

GetAccelZ:
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #ACCEL_ZOUT_H_OFFSET	; set register address
	BL		ReadAccelGyroReg			; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8						
	MOV		R0, #ACCEL_ZOUT_L_OFFSET	; set register address
	BL		ReadAccelGyroReg			; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return


; GetGyroX, GetGyroY, GetGyroZ
;
; Description:			Reads the X-axis, Y-axis, or Z-axis gyroscope value.
;
; Arguments:			None.
; Returns:				R0 = X-axis, Y-axis, or Z-axis gyroscope value
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

GetGyroX:
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #GYRO_XOUT_H_OFFSET		; set register address
	BL		ReadAccelGyroReg			; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8
	MOV		R0, #GYRO_XOUT_L_OFFSET		; set register address
	BL		ReadAccelGyroReg			; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return

GetGyroY:
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #GYRO_YOUT_H_OFFSET		; set register address
	BL		ReadAccelGyroReg			; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8
	MOV		R0, #GYRO_YOUT_L_OFFSET		; set register address
	BL		ReadAccelGyroReg			; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return

GetGyroZ:
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #GYRO_ZOUT_H_OFFSET		; set register address
	BL		ReadAccelGyroReg			; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8
	MOV		R0, #GYRO_ZOUT_L_OFFSET		; set register address
	BL		ReadAccelGyroReg			; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return


; GetMagX, GetMagY, GetMagZ
;
; Description:			Reads the X-axis, Y-axis, or Z-axis magnetometer value.
;
; Arguments:			None.
; Returns:				R0 = X-axis, Y-axis, or Z-axis magnetometer value
;
; Local Variables:      None.
; Shared Variables:     None.
; Global Variables:     None.
;
; Error Handling:       None.
;
; Registers Changed:    flags, R0, R1, R2, R3
; Stack Depth:          1
;
; Revision History:

GetMagX:
	PUSH	{LR}						; save return address and used registers
	MOV		R0, #MAG_XOUT_L_OFFSET		; set register address
	BL		ReadMagnetReg				; read register
	POP		{LR}						; restore return address and used registers
	BX		LR							; return

GetMagY:
	PUSH	{LR}						; save return address and used registers
	MOV		R0, #MAG_YOUT_L_OFFSET		; set register address
	BL		ReadMagnetReg				; read register
	POP		{LR}						; restore return address and used registers
	BX		LR							; return

GetMagZ:
	PUSH	{LR}						; save return address and used registers
	MOV		R0, #MAG_ZOUT_L_OFFSET		; set register address
	BL		ReadMagnetReg				; read register
	POP		{LR}						; restore return address and used registers
	BX		LR							; return
