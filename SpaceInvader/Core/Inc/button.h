#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

#include "colors.h"
#include <stdint.h>
#include <stdbool.h>

#define BUTTON_WIDTH  80
#define BUTTON_HEIGHT 50

// Makra pomocnicze
#define X1(c)   c
#define X2(c)   c,c
#define X3(c)   c,c,c
#define X4(c)   c,c,c,c
#define X5(c)   c,c,c,c,c
#define X6(c)   c,c,c,c,c,c
#define X7(c)   X5(c), X2(c)
#define X8(c)   c,c,c,c,c,c,c,c
#define X9(c)   c,c,c,c,c,c,c,c,c
#define X10(c)  X5(c), X5(c)
#define X11(c)  X10(c), c
#define X12(c)  X6(c), X6(c)
#define X13(c)  X10(c), X3(c)
#define X15(c)  X10(c), X5(c)
#define X16(c)  X8(c), X8(c)
#define X18(c)  X9(c), X9(c)
#define X20(c)  X10(c), X10(c)
#define X26(c)  X10(c), X10(c), X6(c)
#define X32(c)  X16(c), X16(c)
#define X37(c)  X20(c), X10(c), X5(c), X2(c)
#define X39(c)  X20(c), X10(c), X9(c)
#define X40(c)  X20(c), X20(c)
#define X42(c)  X40(c), X2(c)
#define X44(c)  X40(c), X4(c)

// Definicja paskow

// Srodek
#define COL_MID \
    X44(N_FACE), X6(N_SIDE)

// Zaokraglenia
#define COL_R1 \
    X2(C_TRANS), X42(N_FACE), X4(N_SIDE), X2(C_TRANS)

#define COL_R2 \
    X5(C_TRANS), X39(N_FACE), N_SIDE, X5(C_TRANS)

#define COL_R3 \
    X9(C_TRANS), X32(N_FACE), X9(C_TRANS)

#define COL_R4 \
    X16(C_TRANS), X18(N_FACE), X16(C_TRANS)

// Srodek wcisniety
#define P_COL_MID \
    X2(C_TRANS), X44(P_FACE), X2(P_SIDE), X2(C_TRANS)

// Zaokrąglenia wcisniete
#define P_COL_R1  X4(C_TRANS), X42(P_FACE), X4(C_TRANS)
#define P_COL_R2  X7(C_TRANS), X37(P_FACE), X6(C_TRANS)
#define P_COL_R3  X11(C_TRANS), X26(P_FACE), X13(C_TRANS)
#define P_COL_R4  X18(C_TRANS), X12(P_FACE), X20(C_TRANS)

const uint32_t BUTTON[BUTTON_WIDTH * BUTTON_HEIGHT] = {
    // Prawa
    COL_R4,
    COL_R4,
    COL_R3, COL_R3,
    COL_R2, COL_R2, COL_R2,
    COL_R1, COL_R1, COL_R1,

    // Środek
    COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID,
    COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID,
    COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID,
    COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID,
    COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID,
    COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID, COL_MID,

    // Lewa
    COL_R1, COL_R1, COL_R1,
    COL_R2, COL_R2, COL_R2,
    COL_R3, COL_R3,
    COL_R4,
    COL_R4
};

const uint32_t BUTTON_PRESS[BUTTON_WIDTH * BUTTON_HEIGHT] = {
    // Prawa
    P_COL_R4, P_COL_R4,
    P_COL_R3, P_COL_R3,
    P_COL_R2, P_COL_R2, P_COL_R2,
    P_COL_R1, P_COL_R1, P_COL_R1,

    // Środek
    P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID,
    P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID,
    P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID,
    P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID,
    P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID,
    P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID, P_COL_MID,

    // Lewa
    P_COL_R1, P_COL_R1, P_COL_R1,
    P_COL_R2, P_COL_R2, P_COL_R2,
    P_COL_R3, P_COL_R3,
    P_COL_R4, P_COL_R4
};

typedef struct {
    int x;
    int y;
    bool isPressed;
    const char *label;
    const char *pressed_label;
    int label_offset;
    int label_xShift;
    uint32_t text_color;
} Button;

#endif
