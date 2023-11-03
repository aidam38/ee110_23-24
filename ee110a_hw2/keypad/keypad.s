    .include "keypad_symbols.inc"

    .ref EnqueueEvent

    .def KeypadInit
    .def KeypadScanAndDebounce

KeypadInit:

KeypadScanAndDebounce:

    BL  EnqueueEvent