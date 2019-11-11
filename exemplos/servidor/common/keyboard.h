#ifndef LIB_KEYBOARD_H
#define LIB_KEYBOARD_H

#define KEYDOWN_TYPE 0x01
#define KEYUP_TYPE 0x02
#define CONFIRMATION_BIT 0x08


#define KEY_BYTE_U 0x10
#define KEY_BYTE_L 0x20
#define KEY_BYTE_D 0x40
#define KEY_BYTE_R 0x80


unsigned char encodeKey(int type, int key);

#endif