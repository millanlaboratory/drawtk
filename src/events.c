/*
    Copyright (C) 2009-2011  EPFL (Ecole Polytechnique Fédérale de Lausanne)
    Laboratory CNBI (Chair in Non-Invasive Brain-Machine Interface)
    Nicolas Bourdaud <nicolas.bourdaud@epfl.ch>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "drawtk.h"
#include "dtk_event.h"
#include "window.h"

#include <SDL/SDL_keysym.h>

static 
enum dtk_keysym convsym[SDLK_LAST] = {
	[SDLK_UNKNOWN] = DTKK_UNKNOWN,
	[SDLK_BACKSPACE] = DTKK_BACKSPACE,
	[SDLK_TAB] = DTKK_TAB,
	[SDLK_CLEAR] = DTKK_CLEAR,
	[SDLK_RETURN] = DTKK_RETURN,
	[SDLK_PAUSE] = DTKK_PAUSE,
	[SDLK_ESCAPE] = DTKK_ESCAPE,
	[SDLK_SPACE] = DTKK_SPACE,
	[SDLK_EXCLAIM] = DTKK_EXCLAIM,
	[SDLK_QUOTEDBL] = DTKK_QUOTEDBL,
	[SDLK_HASH] = DTKK_HASH,
	[SDLK_DOLLAR] = DTKK_DOLLAR,
	[SDLK_AMPERSAND] = DTKK_AMPERSAND,
	[SDLK_QUOTE] = DTKK_QUOTE,
	[SDLK_LEFTPAREN] = DTKK_LEFTPAREN,
	[SDLK_RIGHTPAREN] = DTKK_RIGHTPAREN,
	[SDLK_ASTERISK] = DTKK_ASTERISK,
	[SDLK_PLUS] = DTKK_PLUS,
	[SDLK_COMMA] = DTKK_COMMA,
	[SDLK_MINUS] = DTKK_MINUS,
	[SDLK_PERIOD] = DTKK_PERIOD,
	[SDLK_SLASH] = DTKK_SLASH,
	[SDLK_0] = DTKK_0,
	[SDLK_1] = DTKK_1,
	[SDLK_2] = DTKK_2,
	[SDLK_3] = DTKK_3,
	[SDLK_4] = DTKK_4,
	[SDLK_5] = DTKK_5,
	[SDLK_6] = DTKK_6,
	[SDLK_7] = DTKK_7,
	[SDLK_8] = DTKK_8,
	[SDLK_9] = DTKK_9,
	[SDLK_COLON] = DTKK_COLON,
	[SDLK_SEMICOLON] = DTKK_SEMICOLON,
	[SDLK_LESS] = DTKK_LESS,
	[SDLK_EQUALS] = DTKK_EQUALS,
	[SDLK_GREATER] = DTKK_GREATER,
	[SDLK_QUESTION] = DTKK_QUESTION,
	[SDLK_AT] = DTKK_AT,
	                                 
	[SDLK_LEFTBRACKET] = DTKK_LEFTBRACKET,
	[SDLK_BACKSLASH] = DTKK_BACKSLASH,
	[SDLK_RIGHTBRACKET] = DTKK_RIGHTBRACKET,
	[SDLK_CARET] = DTKK_CARET,
	[SDLK_UNDERSCORE] = DTKK_UNDERSCORE,
	[SDLK_BACKQUOTE] = DTKK_BACKQUOTE,
	[SDLK_a] = DTKK_a,
	[SDLK_b] = DTKK_b,
	[SDLK_c] = DTKK_c,
	[SDLK_d] = DTKK_d,
	[SDLK_e] = DTKK_e,
	[SDLK_f] = DTKK_f,
	[SDLK_g] = DTKK_g,
	[SDLK_h] = DTKK_h,
	[SDLK_i] = DTKK_i,
	[SDLK_j] = DTKK_j,
	[SDLK_k] = DTKK_k,
	[SDLK_l] = DTKK_l,
	[SDLK_m] = DTKK_m,
	[SDLK_n] = DTKK_n,
	[SDLK_o] = DTKK_o,
	[SDLK_p] = DTKK_p,
	[SDLK_q] = DTKK_q,
	[SDLK_r] = DTKK_r,
	[SDLK_s] = DTKK_s,
	[SDLK_t] = DTKK_t,
	[SDLK_u] = DTKK_u,
	[SDLK_v] = DTKK_v,
	[SDLK_w] = DTKK_w,
	[SDLK_x] = DTKK_x,
	[SDLK_y] = DTKK_y,
	[SDLK_z] = DTKK_z,
	[SDLK_DELETE] = DTKK_DELETE,
	                                 
	[SDLK_WORLD_0] = DTKK_WORLD_0,
	[SDLK_WORLD_1] = DTKK_WORLD_1,
	[SDLK_WORLD_2] = DTKK_WORLD_2,
	[SDLK_WORLD_3] = DTKK_WORLD_3,
	[SDLK_WORLD_4] = DTKK_WORLD_4,
	[SDLK_WORLD_5] = DTKK_WORLD_5,
	[SDLK_WORLD_6] = DTKK_WORLD_6,
	[SDLK_WORLD_7] = DTKK_WORLD_7,
	[SDLK_WORLD_8] = DTKK_WORLD_8,
	[SDLK_WORLD_9] = DTKK_WORLD_9,
	[SDLK_WORLD_10] = DTKK_WORLD_10,
	[SDLK_WORLD_11] = DTKK_WORLD_11,
	[SDLK_WORLD_12] = DTKK_WORLD_12,
	[SDLK_WORLD_13] = DTKK_WORLD_13,
	[SDLK_WORLD_14] = DTKK_WORLD_14,
	[SDLK_WORLD_15] = DTKK_WORLD_15,
	[SDLK_WORLD_16] = DTKK_WORLD_16,
	[SDLK_WORLD_17] = DTKK_WORLD_17,
	[SDLK_WORLD_18] = DTKK_WORLD_18,
	[SDLK_WORLD_19] = DTKK_WORLD_19,
	[SDLK_WORLD_20] = DTKK_WORLD_20,
	[SDLK_WORLD_21] = DTKK_WORLD_21,
	[SDLK_WORLD_22] = DTKK_WORLD_22,
	[SDLK_WORLD_23] = DTKK_WORLD_23,
	[SDLK_WORLD_24] = DTKK_WORLD_24,
	[SDLK_WORLD_25] = DTKK_WORLD_25,
	[SDLK_WORLD_26] = DTKK_WORLD_26,
	[SDLK_WORLD_27] = DTKK_WORLD_27,
	[SDLK_WORLD_28] = DTKK_WORLD_28,
	[SDLK_WORLD_29] = DTKK_WORLD_29,
	[SDLK_WORLD_30] = DTKK_WORLD_30,
	[SDLK_WORLD_31] = DTKK_WORLD_31,
	[SDLK_WORLD_32] = DTKK_WORLD_32,
	[SDLK_WORLD_33] = DTKK_WORLD_33,
	[SDLK_WORLD_34] = DTKK_WORLD_34,
	[SDLK_WORLD_35] = DTKK_WORLD_35,
	[SDLK_WORLD_36] = DTKK_WORLD_36,
	[SDLK_WORLD_37] = DTKK_WORLD_37,
	[SDLK_WORLD_38] = DTKK_WORLD_38,
	[SDLK_WORLD_39] = DTKK_WORLD_39,
	[SDLK_WORLD_40] = DTKK_WORLD_40,
	[SDLK_WORLD_41] = DTKK_WORLD_41,
	[SDLK_WORLD_42] = DTKK_WORLD_42,
	[SDLK_WORLD_43] = DTKK_WORLD_43,
	[SDLK_WORLD_44] = DTKK_WORLD_44,
	[SDLK_WORLD_45] = DTKK_WORLD_45,
	[SDLK_WORLD_46] = DTKK_WORLD_46,
	[SDLK_WORLD_47] = DTKK_WORLD_47,
	[SDLK_WORLD_48] = DTKK_WORLD_48,
	[SDLK_WORLD_49] = DTKK_WORLD_49,
	[SDLK_WORLD_50] = DTKK_WORLD_50,
	[SDLK_WORLD_51] = DTKK_WORLD_51,
	[SDLK_WORLD_52] = DTKK_WORLD_52,
	[SDLK_WORLD_53] = DTKK_WORLD_53,
	[SDLK_WORLD_54] = DTKK_WORLD_54,
	[SDLK_WORLD_55] = DTKK_WORLD_55,
	[SDLK_WORLD_56] = DTKK_WORLD_56,
	[SDLK_WORLD_57] = DTKK_WORLD_57,
	[SDLK_WORLD_58] = DTKK_WORLD_58,
	[SDLK_WORLD_59] = DTKK_WORLD_59,
	[SDLK_WORLD_60] = DTKK_WORLD_60,
	[SDLK_WORLD_61] = DTKK_WORLD_61,
	[SDLK_WORLD_62] = DTKK_WORLD_62,
	[SDLK_WORLD_63] = DTKK_WORLD_63,
	[SDLK_WORLD_64] = DTKK_WORLD_64,
	[SDLK_WORLD_65] = DTKK_WORLD_65,
	[SDLK_WORLD_66] = DTKK_WORLD_66,
	[SDLK_WORLD_67] = DTKK_WORLD_67,
	[SDLK_WORLD_68] = DTKK_WORLD_68,
	[SDLK_WORLD_69] = DTKK_WORLD_69,
	[SDLK_WORLD_70] = DTKK_WORLD_70,
	[SDLK_WORLD_71] = DTKK_WORLD_71,
	[SDLK_WORLD_72] = DTKK_WORLD_72,
	[SDLK_WORLD_73] = DTKK_WORLD_73,
	[SDLK_WORLD_74] = DTKK_WORLD_74,
	[SDLK_WORLD_75] = DTKK_WORLD_75,
	[SDLK_WORLD_76] = DTKK_WORLD_76,
	[SDLK_WORLD_77] = DTKK_WORLD_77,
	[SDLK_WORLD_78] = DTKK_WORLD_78,
	[SDLK_WORLD_79] = DTKK_WORLD_79,
	[SDLK_WORLD_80] = DTKK_WORLD_80,
	[SDLK_WORLD_81] = DTKK_WORLD_81,
	[SDLK_WORLD_82] = DTKK_WORLD_82,
	[SDLK_WORLD_83] = DTKK_WORLD_83,
	[SDLK_WORLD_84] = DTKK_WORLD_84,
	[SDLK_WORLD_85] = DTKK_WORLD_85,
	[SDLK_WORLD_86] = DTKK_WORLD_86,
	[SDLK_WORLD_87] = DTKK_WORLD_87,
	[SDLK_WORLD_88] = DTKK_WORLD_88,
	[SDLK_WORLD_89] = DTKK_WORLD_89,
	[SDLK_WORLD_90] = DTKK_WORLD_90,
	[SDLK_WORLD_91] = DTKK_WORLD_91,
	[SDLK_WORLD_92] = DTKK_WORLD_92,
	[SDLK_WORLD_93] = DTKK_WORLD_93,
	[SDLK_WORLD_94] = DTKK_WORLD_94,
	[SDLK_WORLD_95] = DTKK_WORLD_95,
	                                 
	[SDLK_KP0] = DTKK_KP0,
	[SDLK_KP1] = DTKK_KP1,
	[SDLK_KP2] = DTKK_KP2,
	[SDLK_KP3] = DTKK_KP3,
	[SDLK_KP4] = DTKK_KP4,
	[SDLK_KP5] = DTKK_KP5,
	[SDLK_KP6] = DTKK_KP6,
	[SDLK_KP7] = DTKK_KP7,
	[SDLK_KP8] = DTKK_KP8,
	[SDLK_KP9] = DTKK_KP9,
	[SDLK_KP_PERIOD] = DTKK_KP_PERIOD,
	[SDLK_KP_DIVIDE] = DTKK_KP_DIVIDE,
	[SDLK_KP_MULTIPLY] = DTKK_KP_MULTIPLY,
	[SDLK_KP_MINUS] = DTKK_KP_MINUS,
	[SDLK_KP_PLUS] = DTKK_KP_PLUS,
	[SDLK_KP_ENTER] = DTKK_KP_ENTER,
	[SDLK_KP_EQUALS] = DTKK_KP_EQUALS,
	                                 
	[SDLK_UP] = DTKK_UP,
	[SDLK_DOWN] = DTKK_DOWN,
	[SDLK_RIGHT] = DTKK_RIGHT,
	[SDLK_LEFT] = DTKK_LEFT,
	[SDLK_INSERT] = DTKK_INSERT,
	[SDLK_HOME] = DTKK_HOME,
	[SDLK_END] = DTKK_END,
	[SDLK_PAGEUP] = DTKK_PAGEUP,
	[SDLK_PAGEDOWN] = DTKK_PAGEDOWN,
	                                 
	[SDLK_F1] = DTKK_F1,
	[SDLK_F2] = DTKK_F2,
	[SDLK_F3] = DTKK_F3,
	[SDLK_F4] = DTKK_F4,
	[SDLK_F5] = DTKK_F5,
	[SDLK_F6] = DTKK_F6,
	[SDLK_F7] = DTKK_F7,
	[SDLK_F8] = DTKK_F8,
	[SDLK_F9] = DTKK_F9,
	[SDLK_F10] = DTKK_F10,
	[SDLK_F11] = DTKK_F11,
	[SDLK_F12] = DTKK_F12,
	[SDLK_F13] = DTKK_F13,
	[SDLK_F14] = DTKK_F14,
	[SDLK_F15] = DTKK_F15,
	                                 
	[SDLK_NUMLOCK] = DTKK_NUMLOCK,
	[SDLK_CAPSLOCK] = DTKK_CAPSLOCK,
	[SDLK_SCROLLOCK] = DTKK_SCROLLOCK,
	[SDLK_RSHIFT] = DTKK_RSHIFT,
	[SDLK_LSHIFT] = DTKK_LSHIFT,
	[SDLK_RCTRL] = DTKK_RCTRL,
	[SDLK_LCTRL] = DTKK_LCTRL,
	[SDLK_RALT] = DTKK_RALT,
	[SDLK_LALT] = DTKK_LALT,
	[SDLK_RMETA] = DTKK_RMETA,
	[SDLK_LMETA] = DTKK_LMETA,
	[SDLK_LSUPER] = DTKK_LSUPER,
	[SDLK_RSUPER] = DTKK_RSUPER,
	[SDLK_MODE] = DTKK_MODE,
	[SDLK_COMPOSE] = DTKK_COMPOSE,
	                                 
	[SDLK_HELP] = DTKK_HELP,
	[SDLK_PRINT] = DTKK_PRINT,
	[SDLK_SYSREQ] = DTKK_SYSREQ,
	[SDLK_BREAK] = DTKK_BREAK,
	[SDLK_MENU] = DTKK_MENU,
	[SDLK_POWER] = DTKK_POWER,
	[SDLK_EURO] = DTKK_EURO,
	[SDLK_UNDO] = DTKK_UNDO,
};


static 
void fill_keyevt(union dtk_event *d_evt, const SDL_Event* s_evt)
{
	d_evt->key.state = (s_evt->key.state==SDL_PRESSED)?
		            DTK_KEY_PRESSED : DTK_KEY_RELEASED;
	d_evt->key.sym = convsym[s_evt->key.keysym.sym];
	d_evt->key.mod = s_evt->key.keysym.mod;
}

static 
void fill_mouseevt(union dtk_event *d_evt, const SDL_Event* s_evt)
{
	d_evt->mouse.button = s_evt->button.button;
	d_evt->mouse.state = s_evt->button.state;
	d_evt->mouse.x = s_evt->button.x;
	d_evt->mouse.y = s_evt->button.y;
}


API_EXPORTED
int dtk_process_events(struct dtk_window* wnd)
{
	DTKEvtProc handler = wnd->evthandler;
	SDL_Event sevt;
	union dtk_event evt;
	int ret = 1;
	int type;

	while (SDL_PollEvent(&sevt)) {
		switch (sevt.type) {
		case SDL_QUIT:
			if (handler) 
				ret = handler(wnd, DTK_EVT_QUIT, NULL);
			else
				ret = 0;
			break;

		case SDL_VIDEOEXPOSE:
			if (handler)
				ret = handler(wnd, DTK_EVT_REDRAW, NULL);
			break;

		case SDL_VIDEORESIZE:
			resize_window(wnd, sevt.resize.w, sevt.resize.h, 0);
			init_opengl_state(wnd);
			break;

		case SDL_KEYUP:
		case SDL_KEYDOWN:
			if (handler) {
				fill_keyevt(&evt, &sevt);
				ret = handler(wnd, DTK_EVT_KEYBOARD, &evt);
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEMOTION:
			if (handler) {
				fill_mouseevt(&evt, &sevt);
				type = (sevt.type == SDL_MOUSEMOTION) ? 
				        DTK_EVT_MOUSEMOTION : 
					DTK_EVT_MOUSEBUTTON;
				ret = handler(wnd, type ,&evt);
			}
			break;
		}
		if (!ret)
			return 0;
	}
	return 1;
}

