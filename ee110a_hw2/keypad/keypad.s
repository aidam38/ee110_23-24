    .include "keypad_debounce.inc"

    .ref EnqueueEvent

    .def KeypadInit
    .def KeypadScanAndDebounce

KeypadInit:

KeypadScanAndDebounce:

    BL  EnqueueEvent