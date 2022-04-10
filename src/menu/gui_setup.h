#ifndef FF598DDB_0591_4EAB_94CB_E11D7C92EB21
#define FF598DDB_0591_4EAB_94CB_E11D7C92EB21

    #include "../../lib/gui/graphics/ssd1306_graphics.h"
    #include "globals.h"

    #define REAL_W 128
    #define REAL_H 64
    #define HALF_RES false
    #define ROWS_IN_RAM 8
    #define DRAW_CYCLES 1
    #define WIDTH HALF_RES ? REAL_W/2 : REAL_W
    #define HEIGHT HALF_RES ? REAL_H/2 : REAL_H
    #define DISPLAY_ROTATION GFX::DisplayRotation::D0

    extern GFX::SSD1306Graphics g;

    void initGFX();

#endif /* FF598DDB_0591_4EAB_94CB_E11D7C92EB21 */
