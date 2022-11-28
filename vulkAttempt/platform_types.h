#pragma once

#include "defines.h"
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>  
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>  
#include <sys/time.h>


typedef struct {
        Display* display;
        xcb_connection_t* connection;
        xcb_window_t window;
        xcb_screen_t* screen;
        xcb_atom_t wm_protocols;
        xcb_atom_t wm_delete_win;
        u32 win_height;
        u32 win_width;
} platform_state;
