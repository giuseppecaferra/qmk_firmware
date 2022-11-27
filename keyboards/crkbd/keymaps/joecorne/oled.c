#pragma once
#include <stdio.h>
#include "keycodes.h"

extern uint8_t is_master;

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;
    }
    return rotation;
}

void oled_render_layer_state(void) {
    oled_set_cursor(0, 1);
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_ln_P(PSTR("DEFAULT"), false);
            break;
        case _FUNC:
            oled_write_ln_P(PSTR("FUNCTIONS"), false);
            break;
        case _LOWER:
            oled_write_ln_P(PSTR("NUMERIC"), false);
            break;
        case _RAISE:
            oled_write_ln_P(PSTR("SYMBOLS"), false);
            break;
        case _MOVES:
            oled_write_ln_P(PSTR("MOVES"), false);
            break;
        case _ADJUST:
            oled_write_ln_P(PSTR("ADJUST"), false);
            break;
    }
}
void oled_render_wpm(void) {
    oled_set_cursor(0, 0);
    // This way it's more size efficient than calling sprintf
    oled_write_P(PSTR("WPM: "), false);
    oled_write(get_u8_str(get_current_wpm(), ' '), false);
    // This way it's more size efficient than calling sprintf
}

void oled_render_logo(void) {
    static const char PROGMEM crkbd_logo[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 0x90, 0x91, 0x92, 0x93, 0x94, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0};
    oled_write_P(crkbd_logo, false);
}

// Keylock State
void render_keylock_status(uint8_t led_usb_state) {
      oled_set_cursor(0, 3);
    oled_write_P(PSTR(" "), false);
    oled_write_P(led_usb_state & (1 << USB_LED_NUM_LOCK) ? PSTR("NUML") : PSTR("    "), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(led_usb_state & (1 << USB_LED_CAPS_LOCK) ? PSTR("CAPS") : PSTR("     "), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(led_usb_state & (1 << USB_LED_SCROLL_LOCK) ? PSTR("SCRL") : PSTR("     "), false);
    oled_write_P(PSTR(" "), false);
}

// Keylogger
#define KEYLOGGER_LENGTH 8
static uint32_t oled_timer                       = 0;
static char     keylog_str[KEYLOGGER_LENGTH + 1] = {0};

static const char PROGMEM code_to_name[0xFF] = {
    //   0    1    2    3    4    5    6    7    8    9    A    B    c    D    E    F
    182, ' ',  ' ', ' ', 'a',  'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', // 0x
    'm', 'n',  'o', 'p', 'q',  'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', // 1x
    '3', '4',  '5', '6', '7',  '8', '9', '0', 20,  19,  17,  29,  22,  '-', '=', '[', // 2x
    ']', '\\', '#', ';', '\'', '`', ',', '.', '/', 188, 149, 150, 151, 152, 153, 154, // 3x
    155, 156,  157, 158, 159,  181, 191, 190, ' ', ' ', 185, 183, 16,  186, 184, 26,  // 4x
    27,  25,   24,  189, '/',  '*', '-', '+', ' ', '1', '2', '3', '4', '5', '6', '7', // 5x
    '8', '9',  '0', '.', ' ',  187, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 6x
    ' ', ' ',  ' ', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 7x
    ' ', ' ',  ' ', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 8x
    ' ', ' ',  ' ', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // 9x
    ' ', ' ',  ' ', ' ', ' ',  ' ', ' ', ' ', 214, 215, 216, 217, 218, 219, 220, 221, // Ax
    ' ', ' ',  213, ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // Bx
    ' ', ' ',  ' ', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // Cx
    ' ', ' ',  ' ', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // Dx
    'C', 'S',  'A', 'W', ' ',  'S', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', // Ex
    ' ', ' ',  ' ', ' ', ' ',  ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '       // Fx
};

void add_keylog(uint16_t keycode);

void add_keylog(uint16_t keycode) {
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) || (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX) || (keycode >= QK_MODS && keycode <= QK_MODS_MAX)) {
        keycode = keycode & 0xFF;
    } else if (keycode > 0xFF) {
        keycode = 0;
    }

    for (uint8_t i = (KEYLOGGER_LENGTH - 1); i > 0; --i) {
        keylog_str[i] = keylog_str[i - 1];
    }

    if (keycode < (sizeof(code_to_name) / sizeof(char))) {
        keylog_str[0] = pgm_read_byte(&code_to_name[keycode]);
    }
}

void render_keylogger_status(void) {
    oled_set_cursor(0,2);
    oled_write(keylog_str, false);
}

char    wpm_text[8] = "WPM: ";
int     timer       = 0;
int     y           = OLED_DISPLAY_WIDTH - 1;
uint8_t currwpm     = 0;
uint8_t oldwpm      = 0;

//=============  USER CONFIG PARAMS  ===============
#define WPM_GRAPH_SPEED_MAX 130.0f    // WPM value at the top of the graph window
#define WPM_GRAPH_REFRESH_INTERVAL 80 // how often screen will be refreshed with WPM values; in milliseconds
#define WPM_GRAPH_REFRESH_TIMEOUT 800 // how often screen will be refreshed with WPM values; in milliseconds
#define WPM_GRAPH_LINE_THICKNESS 3    // determines thickness of graph line in pixels

#if defined(OLED_BRIGHTNESS) && OLED_BRIGHTNESS < 100
#    define __WPM_GRAPH_BRIGHTNESS_DEFINED 1

#    define WPM_GRAPH_SPEED_HIGHLIGHT 50.0f // At what WPM screen brightness will start increasing
#    define WPM_GRAPH_BRIGHTNESS_MAX 255    // Max screen brightness
#endif
//=============  END USER PARAMS  ===============

void render_wpm_graph(void) {
    const int elapsed = timer_elapsed(timer);

    // get current WPM value
    oldwpm  = currwpm;
    currwpm = get_current_wpm();

    if (elapsed > WPM_GRAPH_REFRESH_INTERVAL) {
        // main calculation to plot graph line
        y = OLED_DISPLAY_WIDTH - ((currwpm / WPM_GRAPH_SPEED_MAX) * OLED_DISPLAY_WIDTH);

        // first draw actual value line
        for (int i = 0; i < WPM_GRAPH_LINE_THICKNESS; i++) {
            oled_write_pixel(1, y + i, true);
        }

        // then fill in area below the value line
        for (int i = OLED_DISPLAY_WIDTH; i > y && 0 <= i; i--) {
            oled_write_pixel(1, i, true);
        }

        // then move the entire graph one pixel to the right
        oled_pan(false);

        // refresh the timer for the next iteration
        timer = timer_read();
    }

    // formatting for triple digit WPM vs double digits, then print WPM readout,
    // or does not print anything if WPM is 0
    int8_t pos_offset = -1;
    if (100 <= currwpm) {
        pos_offset = 0;
    } else if (10 <= currwpm) {
        pos_offset = 1;
    } else if (0 < currwpm) {
        pos_offset = 2;
    }
    if (0 <= pos_offset) {
        if (currwpm != oldwpm) {
            itoa(currwpm, wpm_text + 4, 10);
        }
        oled_set_cursor(14 + pos_offset, 3);
        oled_write(wpm_text, false);
    }

// brightness adjust when riched WPM_GRAPH_SPEED_HIGHLIGHT
#ifdef __WPM_GRAPH_BRIGHTNESS_DEFINED
    const uint8_t currbr = oled_get_brightness();
    uint8_t       newbr  = currbr;

    if (WPM_GRAPH_SPEED_HIGHLIGHT <= currwpm) {
        const int   brsteps = (WPM_GRAPH_SPEED_MAX - WPM_GRAPH_SPEED_HIGHLIGHT) - (WPM_GRAPH_SPEED_MAX - ((currwpm > WPM_GRAPH_SPEED_MAX) ? WPM_GRAPH_SPEED_MAX : currwpm));
        const float _newbr  = OLED_BRIGHTNESS + (((float)(WPM_GRAPH_BRIGHTNESS_MAX - OLED_BRIGHTNESS) / (WPM_GRAPH_SPEED_MAX - WPM_GRAPH_SPEED_HIGHLIGHT)) * (float)brsteps);

        newbr = (uint8_t)((_newbr > WPM_GRAPH_BRIGHTNESS_MAX) ? WPM_GRAPH_BRIGHTNESS_MAX : _newbr);
    } else if (currbr != OLED_BRIGHTNESS) {
        newbr = OLED_BRIGHTNESS;
    }

    if (currbr != newbr) {
        oled_set_brightness(newbr);
    }

#endif
}



bool oled_task_user(void) {
    if (is_keyboard_master()) {
        oled_render_layer_state();
        oled_render_wpm();
        render_keylogger_status();
        // oled_render_keylog();
    } else {
        render_wpm_graph();
    }
    return false;
}
