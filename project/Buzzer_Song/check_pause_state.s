    .global check_pause_state
    .text

check_pause_state:
    ; Check if P2.1 is LOW (button pressed)
    MOV &P2IN, R12     ; Load P2IN register into R12
    BIT #BIT1, R12     ; Test bit 1
    JZ button_pressed  ; If bit is 0, jump (pressed)
    
    MOV #0, R12        ; Not pressed, return 0
    RET

button_pressed:
    MOV #1, R12        
