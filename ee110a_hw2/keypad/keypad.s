    .include "keypad_symbols.inc"

    .ref EnqueueEvent

    .def KeypadInit
    .def KeypadScanAndDebounce

KeypadInit:
	BX	LR

KeypadScanAndDebounce:

    BL  EnqueueEvent
    BX 	LR
