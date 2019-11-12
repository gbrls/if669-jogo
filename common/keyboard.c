#include "keyboard.h"
#include <allegro5/allegro.h>

// c: confirmation bit, t type(up or down)
// xxxx: keycode
// xxxx0ctt
unsigned char encodeKey(int type, int key) {
    unsigned char ans = 0;

    switch (key) {
    case ALLEGRO_KEY_LEFT:
        ans |= (KEY_BYTE_L);
        ans |= type|CONFIRMATION_BIT;
        break;

    case ALLEGRO_KEY_RIGHT:
        ans |= (KEY_BYTE_R);
        ans |= type|CONFIRMATION_BIT;
        break;

    case ALLEGRO_KEY_UP:
        ans |= (KEY_BYTE_U);
        ans |= type|CONFIRMATION_BIT;
        break;

    case ALLEGRO_KEY_DOWN:
        ans |= (KEY_BYTE_D);
        ans |= type|CONFIRMATION_BIT;
        break;

    default:
        break;
    }

    return ans;
}