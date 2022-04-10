#ifndef DBEF750C_562B_4686_A1AF_964059AAE4BF
#define DBEF750C_562B_4686_A1AF_964059AAE4BF

namespace Menu {
    struct MenuState{
        int screen = 0;
        int focus = 0;
        bool selected = false;

        MenuState() {}
    };

    extern MenuState menuState;
}

#endif /* DBEF750C_562B_4686_A1AF_964059AAE4BF */
