#ifndef INC_COLORS_H_
#define INC_COLORS_H_

#define BLACK    0xFF000000
#define WHITE    0xFFFFFFFF

#define RED      0xFFFF0000
#define GREEN    0xFF00FF00
#define BLUE     0xFF0000FF

#define YELLOW   0xFFFFFF00
#define ORANGE   0xFFFFA500
#define PINK     0xFFFF1493
#define MAGENTA  0xFFFF00FF

// player
#define C_TRANS     0x00000000
#define C_FIRE_RED  0xFFFF0000
#define C_FIRE_DK   0xFF8B0000
#define C_CORE_GOLD 0xFFFFD700
#define C_CORE_WHT  0xFFFFFFFF

#define C_SHP_DK    0xFF203040
#define C_SHP_MAIN  0xFF406080
#define C_SHP_LGHT  0xFF7090B0

// button
#define C_NEON  0xFF99FF33
#define C_TOXIC 0xFF006600
// Stan normalny
#define N_FACE  C_NEON
#define N_SIDE  C_TOXIC
// Stan wcisniety
#define P_FACE  C_TOXIC
#define P_SIDE  C_NEON

// enemy
#define C_ALIEN_DK   0xFF0A200A  // Ciemny obrys
#define C_ALIEN_MAIN 0xFF206010  // Główny pancerz
#define C_ALIEN_LGT  0xFF50A020  // Jasne krawędzie
#define C_TOXIC_GLW  0xFF90FF00  // Aura
#define C_ENG_PURP   0xFF600080  // Ciemne wnętrze
#define C_ENG_CORE   0xFFC060FF  // Rdzeń

// bullet
#define C_AURA_L    0x30FF4500
#define C_AURA_M    0x80FF6000
#define C_FIRE_DK   0xFF8B0000
#define C_FIRE_RED  0xFFFF0000
#define C_PLAS_ORG  0xFFFF4500
#define C_PLAS_BRT  0xFFFFAB00
#define C_CORE_GOLD 0xFFFFD700
#define C_CORE_WHT  0xFFFFFFFF

// enemy bullet
#define C_TOXIC2     0xFF004400 // Ciemna zieleń
#define C_ACID2      0xFF32CD32 // Kwaśna zieleń
#define C_NEON2      0xFF00FF00 // Jaskrawa zieleń

// Arrow button
#define BTN_ALPHA 0xB0 // Przeroczystosc

#define C_NVY  (BTN_ALPHA << 24 | 0x000080) // Ciemny Granat (Obrys)
#define C_BLU  (BTN_ALPHA << 24 | 0x0040FF) // Niebieski
#define C_SKY  (BTN_ALPHA << 24 | 0x0080FF) // Błękitny
#define C_CYN  (BTN_ALPHA << 24 | 0x00FFFF) // Cyjan (Środek)


#endif
