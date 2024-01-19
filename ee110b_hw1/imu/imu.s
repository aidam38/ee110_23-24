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


; InitIMU
;
; Description:			Initializes the IMU in 9 DoF mode.
;
; Arguments:			None.
; Returns:				None.
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

DELAY .equ		500
InitIMU:
	PUSH	{LR}			; save return address

	; configure the IMU to 
	;	- disable I2C slave interface (use SPI only instead)
	;	- enable I2C master interface
	;	- reset signal paths (clears all sensor registers)
	MOV		R0, #(((IMU_WRITE | USER_CTRL_OFFSET) << IMU_WORD) | (USER_CTRL_I2C_IF_DIS | USER_CTRL_I2C_MST_EN | USER_CTRL_SIG_COND_RST))
	BL		SerialSendData

	;MOV		R0, #DELAY
;blah:
	;SUBS	R0, #1
	;BNE		blah

	;MOV		R0, #USER_CTRL_OFFSET
	;BL		ReadAccelGyroReg

	; configure the I2C master interface
	;MOV		R0, #(((IMU_WRITE | I2C_MST_CTRL_OFFSET) << IMU_WORD) | (I2C_MST_CLK_400 | I2C_MST_P_NSR_STOP))
	;BL		SerialSendData

	; configure the accelerometer
	MOV		R0, #(((IMU_WRITE | ACCEL_CONFIG1_OFFSET) << IMU_WORD) | (ACCEL_FS_SEL_2))
	BL		SerialSendData

	; configure the gyroscope
	MOV		R0, #(((IMU_WRITE | GYRO_CONFIG_OFFSET) << IMU_WORD) | (GYRO_FS_SEL_250))
	BL		SerialSendData

	; configure the magnetometer
	;MOV		R0, #MAG_CNTL2_OFFSET
	;MOV		R1, #MAG_SRST
	;BL		WriteMagnetReg		; write to the CNTL2 register

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


; WriteMagnetReg
;
; Description:			Writes a register to the magnetometer.
;						Registers are 8 bits wide.
;
; Arguments:			R0 = register address
;						R1 = register value
; Returns:				None.
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

WriteMagnetReg:
	PUSH	{LR, R4, R5}					; save return address and used registers

	; save variables
	MOV		R4, R0	; register address
	MOV		R5, R1	; register value

	; configure an I2C slave for the magnetometer
	MOV		R0, #(((IMU_WRITE | I2C_SLV4_ADDR_OFFSET) << IMU_WORD) | (I2C_SLV4_WRITE | MAG_ADDR))
	BL		SerialSendData

	; set register address we want to read
	ORR		R0, R4, #(((IMU_WRITE | I2C_SLV4_REG_OFFSET) << IMU_WORD))
	BL		SerialSendData

	MOV		R0, #I2C_SLV4_REG_OFFSET
	BL		ReadAccelGyroReg

	; set register value we want to write
	ORR		R0, R5, #(((IMU_WRITE | I2C_SLV4_DO_OFFSET) << IMU_WORD))
	BL		SerialSendData

	; enable I2C slave 4 transfer
	MOV		R0, #(((IMU_WRITE | I2C_SLV4_CTRL_OFFSET) << IMU_WORD) | (I2C_SLV4_EN))
	BL		SerialSendData

	; wait for transfer to complete by reading the I2C master status
WriteMagnetRegWait:
	MOV		R0, #I2C_MST_STATUS_OFFSET
	BL		ReadAccelGyroReg
	TST		R0, #I2C_SLV4_DONE		; check whether SLV4 bit is asserted
	BEQ		WriteMagnetRegWait		; if not, wait
	;B		WriteMagnetRegTransferDone		; if so, we're done

WriteMagnetRegTransferDone:
WriteMagnetRegDone:	
	POP		{LR, R4, R5}					; restore return address and used registers
	BX		LR						; return

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
	PUSH	{LR, R4}						; save return address and used registers

	; save variables
	MOV		R4, R0	; register address

	; configure an I2C slave for the magnetometer for reading
	MOV		R0, #(((IMU_WRITE | I2C_SLV4_ADDR_OFFSET) << IMU_WORD) | (I2C_SLV4_READ | MAG_ADDR))
	BL		SerialSendData

	; set register address we want to read
	ORR		R0, R4, #((IMU_WRITE | I2C_SLV4_REG_OFFSET) << IMU_WORD)
	BL		SerialSendData

	; enable I2C slave 4 transfer
	MOV		R0, #(((IMU_WRITE | I2C_SLV4_CTRL_OFFSET) << IMU_WORD) | (I2C_SLV4_EN))
	BL		SerialSendData

	; wait for transfer to complete by reading the I2C master status
ReadMagnetRegWait:
	MOV		R0, #I2C_MST_STATUS_OFFSET
	BL		ReadAccelGyroReg
	TST		R0, #I2C_SLV4_DONE		; check whether SLV4 bit is asserted
	BEQ		ReadMagnetRegWait		; if not, wait
	;B		ReadMagnetRegTransferDone		; if so, we're done

ReadMagnetRegTransferDone:
	; read the register value
	MOV		R0, #((IMU_WRITE | I2C_SLV4_DI_OFFSET) << IMU_WORD)
	BL		SerialSendData
	;B		ReadMagnetRegDone

ReadMagnetRegDone:
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return



; ReadMagnetData
;
; Description:			Reads measurement from the magnetometer.
;
; Arguments:			R0 = measruement register address
; Returns:				R0 = data value
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

ReadMagnetData:
	PUSH	{LR}					; save return address and used registers

	; save variables
	MOV		R4, R0	; register address

; trigger measurement by writing to the CNTL1 register
	MOV		R0, #MAG_CNTL1_OFFSET
	MOV		R1, #MAG_SINGLE_MEASUREMENT
	BL		WriteMagnetReg

; poll the status register
ReadMagnetDataWait:
	MOV		R0, #MAG_ST1_OFFSET
	BL		ReadMagnetReg
	TST		R0, #MAG_ST1_DRDY
	BEQ		ReadMagnetDataWait
	;B		ReadMagnetDataMeasurementDone

ReadMagnetDataMeasurementDone:
	; configure an I2C slave for the magnetometer for reading
	MOV		R0, #(((IMU_WRITE | I2C_SLV4_ADDR_OFFSET) << IMU_WORD) | (I2C_SLV4_READ | MAG_ADDR))
	BL		SerialSendData

	; set register address we want to read
	ORR		R0, R4, #((IMU_WRITE | I2C_SLV4_REG_OFFSET) << IMU_WORD)
	BL		SerialSendData

	; enable I2C slave 4 transfer
	MOV		R0, #(((IMU_WRITE | I2C_SLV4_CTRL_OFFSET) << IMU_WORD) | (I2C_SLV4_EN))
	BL		SerialSendData

	; wait for transfer to complete by reading the I2C master status
ReadMagnetDataWait2:
	MOV		R0, #I2C_MST_STATUS_OFFSET
	BL		ReadAccelGyroReg
	TST		R0, #I2C_SLV4_DONE		; check whether SLV4 bit is asserted
	BEQ		ReadMagnetDataWait2		; if not, wait
	;B		ReadMagnetDataTransferDone		; if so, we're done

ReadMagnetDataTransferDone:
	; read the register value
	MOV		R0, #((IMU_WRITE | I2C_SLV4_DI_OFFSET) << IMU_WORD)
	BL		SerialSendData
	;B		ReadMagnetDataDone

ReadMagnetDataDone:
	POP		{LR}					; restore return address and used registers
	BX		LR						; return



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
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #MAG_XOUT_H_OFFSET		; set register address
	BL		ReadMagnetData				; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8
	MOV		R0, #MAG_XOUT_L_OFFSET		; set register address
	BL		ReadMagnetData				; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return

GetMagY:
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #MAG_YOUT_H_OFFSET		; set register address
	BL		ReadMagnetData				; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8
	MOV		R0, #MAG_YOUT_L_OFFSET		; set register address
	BL		ReadMagnetData				; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return

GetMagZ:
	PUSH	{LR, R4}						; save return address and used registers
	MOV		R0, #MAG_ZOUT_H_OFFSET		; set register address
	BL		ReadMagnetData				; read register
	MOV		R4, R0						; save high byte
	LSL		R4, #8
	MOV		R0, #MAG_ZOUT_L_OFFSET		; set register address
	BL		ReadMagnetData				; read register
	ORR		R0, R4						; combine high and low bytes
	POP		{LR, R4}						; restore return address and used registers
	BX		LR							; return

