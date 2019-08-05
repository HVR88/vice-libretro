
Mvk MVk[NPLGN*NLIGN*2]={

	// 0
	{ "1"  ,"!"   ,RETROK_1},
	{ "2"  ,"\""  ,RETROK_2},
	{ "3"  ,"#"   ,RETROK_3},
	{ "4"  ,"$"   ,RETROK_4},
	{ "5"  ,"%"   ,RETROK_5},
	{ "6"  ,"&"   ,RETROK_6},
	{ "7"  ,"/"   ,RETROK_7},
	{ "8"  ,"("   ,RETROK_8},
	{ "9"  ,")"   ,RETROK_9},
	{ "0"  ," "   ,RETROK_0},

	// 10
	{ "q"  ,"Q"   ,RETROK_q},
	{ "w"  ,"W"   ,RETROK_w},
	{ "e"  ,"E"   ,RETROK_e},
	{ "r"  ,"R"   ,RETROK_r},
	{ "t"  ,"T"   ,RETROK_t},
	{ "y"  ,"Y"   ,RETROK_y},
	{ "u"  ,"U"   ,RETROK_u},
	{ "i"  ,"I"   ,RETROK_i},
	{ "o"  ,"O"   ,RETROK_o},
	{ "p"  ,"P"   ,RETROK_p},

	// 20 
	{ "a"  ,"A"   ,RETROK_a},
	{ "s"  ,"S"   ,RETROK_s},
	{ "d"  ,"D"   ,RETROK_d},
	{ "f"  ,"F"   ,RETROK_f},
	{ "g"  ,"G"   ,RETROK_g},
	{ "h"  ,"H"   ,RETROK_h},
	{ "j"  ,"J"   ,RETROK_j},
	{ "k"  ,"K"   ,RETROK_k},	
	{ "l"  ,"L"   ,RETROK_l},
	{ "Pnd","Pnd" ,RETROK_INSERT},

	// 30
	{ "z"  ,"Z"   ,RETROK_z},
	{ "x"  ,"X"   ,RETROK_x},
	{ "c"  ,"C"   ,RETROK_c},
	{ "v"  ,"V"   ,RETROK_v},
	{ "b"  ,"B"   ,RETROK_b},
	{ "n"  ,"N"   ,RETROK_n},
	{ "m"  ,"M"   ,RETROK_m},
	{ ","  ,"<"   ,RETROK_COMMA},
	{ "."  ,">"   ,RETROK_PERIOD},
	{ "/"  ,"/"   ,RETROK_SLASH},

	// 40
	{ "<-" ,"<-"  ,RETROK_BACKQUOTE},
	{ "CTR","CTR" ,RETROK_TAB},
	{ "+"  ,"+"   ,RETROK_MINUS},
	{ "-"  ,"-"   ,RETROK_EQUALS},
	{ "F1" ,"F2"  ,RETROK_F1},
	{ "F3" ,"F4"  ,RETROK_F3},
	{ "RST","RST" ,RETROK_PAGEUP},
	{ ":"  ,"["   ,RETROK_SEMICOLON},
	{ ";"  ,"]"   ,RETROK_QUOTE},
	{ "="  ,"="   ,RETROK_BACKSLASH},

	// 50
	{ "R/S","R/S" ,RETROK_ESCAPE},	
	{ "@"  ,"@"   ,RETROK_LEFTBRACKET},
	{ "*"  ,"*"   ,RETROK_RIGHTBRACKET},	
	{ "^"  ,"^"   ,RETROK_DELETE},	
	{ "F5" ,"F6"  ,RETROK_F5},
	{ "F7" ,"F8"  ,RETROK_F7},
	{ "CLR","CLR" ,RETROK_HOME},
	{ "DEL","DEL" ,RETROK_BACKSPACE},
	{ "CrU","CrU" ,RETROK_UP},
	{ "RET","RET" ,RETROK_RETURN},

	// 60
	{ "C=" ,"C="  ,RETROK_LCTRL},
	{ "ShL","ShL" ,-10},
	{ "LSh","LSh" ,RETROK_LSHIFT},
	{ "Spc","Spc" ,RETROK_SPACE},
	{ "Joy","Joy" ,-4},
	{ "StB","StB" ,-5},
	{ "RSh","RSh" ,RETROK_RSHIFT},
	{ "CrL","CrL" ,RETROK_LEFT},
	{ "CrD","CrD" ,RETROK_DOWN}, 
	{ "CrR","CrR" ,RETROK_RIGHT},
};

#define CORE_OPTION_KEYS { \
{ "---", NULL }, \
{ "RETROK_BACKSPACE", NULL }, \
{ "RETROK_TAB", NULL }, \
{ "RETROK_CLEAR", NULL }, \
{ "RETROK_RETURN", NULL }, \
{ "RETROK_PAUSE", NULL }, \
{ "RETROK_ESCAPE", NULL }, \
{ "RETROK_SPACE", NULL }, \
{ "RETROK_EXCLAIM", NULL }, \
{ "RETROK_QUOTEDBL", NULL }, \
{ "RETROK_HASH", NULL }, \
{ "RETROK_DOLLAR", NULL }, \
{ "RETROK_AMPERSAND", NULL }, \
{ "RETROK_QUOTE", NULL }, \
{ "RETROK_LEFTPAREN", NULL }, \
{ "RETROK_RIGHTPAREN", NULL }, \
{ "RETROK_ASTERISK", NULL }, \
{ "RETROK_PLUS", NULL }, \
{ "RETROK_COMMA", NULL }, \
{ "RETROK_MINUS", NULL }, \
{ "RETROK_PERIOD", NULL }, \
{ "RETROK_SLASH", NULL }, \
{ "RETROK_0", NULL }, \
{ "RETROK_1", NULL }, \
{ "RETROK_2", NULL }, \
{ "RETROK_3", NULL }, \
{ "RETROK_4", NULL }, \
{ "RETROK_5", NULL }, \
{ "RETROK_6", NULL }, \
{ "RETROK_7", NULL }, \
{ "RETROK_8", NULL }, \
{ "RETROK_9", NULL }, \
{ "RETROK_COLON", NULL }, \
{ "RETROK_SEMICOLON", NULL }, \
{ "RETROK_LESS", NULL }, \
{ "RETROK_EQUALS", NULL }, \
{ "RETROK_GREATER", NULL }, \
{ "RETROK_QUESTION", NULL }, \
{ "RETROK_AT", NULL }, \
{ "RETROK_LEFTBRACKET", NULL }, \
{ "RETROK_BACKSLASH", NULL }, \
{ "RETROK_RIGHTBRACKET", NULL }, \
{ "RETROK_CARET", NULL }, \
{ "RETROK_UNDERSCORE", NULL }, \
{ "RETROK_BACKQUOTE", NULL }, \
{ "RETROK_a", NULL }, \
{ "RETROK_b", NULL }, \
{ "RETROK_c", NULL }, \
{ "RETROK_d", NULL }, \
{ "RETROK_e", NULL }, \
{ "RETROK_f", NULL }, \
{ "RETROK_g", NULL }, \
{ "RETROK_h", NULL }, \
{ "RETROK_i", NULL }, \
{ "RETROK_j", NULL }, \
{ "RETROK_k", NULL }, \
{ "RETROK_l", NULL }, \
{ "RETROK_m", NULL }, \
{ "RETROK_n", NULL }, \
{ "RETROK_o", NULL }, \
{ "RETROK_p", NULL }, \
{ "RETROK_q", NULL }, \
{ "RETROK_r", NULL }, \
{ "RETROK_s", NULL }, \
{ "RETROK_t", NULL }, \
{ "RETROK_u", NULL }, \
{ "RETROK_v", NULL }, \
{ "RETROK_w", NULL }, \
{ "RETROK_x", NULL }, \
{ "RETROK_y", NULL }, \
{ "RETROK_z", NULL }, \
{ "RETROK_DELETE", NULL }, \
{ "RETROK_KP0", NULL }, \
{ "RETROK_KP1", NULL }, \
{ "RETROK_KP2", NULL }, \
{ "RETROK_KP3", NULL }, \
{ "RETROK_KP4", NULL }, \
{ "RETROK_KP5", NULL }, \
{ "RETROK_KP6", NULL }, \
{ "RETROK_KP7", NULL }, \
{ "RETROK_KP8", NULL }, \
{ "RETROK_KP9", NULL }, \
{ "RETROK_KP_PERIOD", NULL }, \
{ "RETROK_KP_DIVIDE", NULL }, \
{ "RETROK_KP_MULTIPLY", NULL }, \
{ "RETROK_KP_MINUS", NULL }, \
{ "RETROK_KP_PLUS", NULL }, \
{ "RETROK_KP_ENTER", NULL }, \
{ "RETROK_KP_EQUALS", NULL }, \
{ "RETROK_UP", NULL }, \
{ "RETROK_DOWN", NULL }, \
{ "RETROK_RIGHT", NULL }, \
{ "RETROK_LEFT", NULL }, \
{ "RETROK_INSERT", NULL }, \
{ "RETROK_HOME", NULL }, \
{ "RETROK_END", NULL }, \
{ "RETROK_PAGEUP", NULL }, \
{ "RETROK_PAGEDOWN", NULL }, \
{ "RETROK_F1", NULL }, \
{ "RETROK_F2", NULL }, \
{ "RETROK_F3", NULL }, \
{ "RETROK_F4", NULL }, \
{ "RETROK_F5", NULL }, \
{ "RETROK_F6", NULL }, \
{ "RETROK_F7", NULL }, \
{ "RETROK_F8", NULL }, \
{ "RETROK_F9", NULL }, \
{ "RETROK_F10", NULL }, \
{ "RETROK_F11", NULL }, \
{ "RETROK_F12", NULL }, \
{ "RETROK_F13", NULL }, \
{ "RETROK_F14", NULL }, \
{ "RETROK_F15", NULL }, \
{ "RETROK_NUMLOCK", NULL }, \
{ "RETROK_CAPSLOCK", NULL }, \
{ "RETROK_SCROLLOCK", NULL }, \
{ "RETROK_RSHIFT", NULL }, \
{ "RETROK_LSHIFT", NULL }, \
{ "RETROK_RCTRL", NULL }, \
{ "RETROK_LCTRL", NULL }, \
{ "RETROK_RALT", NULL }, \
{ "RETROK_LALT", NULL }, \
{ "RETROK_RMETA", NULL }, \
{ "RETROK_LMETA", NULL }, \
{ "RETROK_LSUPER", NULL }, \
{ "RETROK_RSUPER", NULL }, \
{ "RETROK_HELP", NULL }, \
{ NULL, NULL }, \
}

const char* keyDesc[] = {
"---",//0,
"RETROK_BACKSPACE",//8,
"RETROK_TAB",//9,
"RETROK_CLEAR",//12,
"RETROK_RETURN",//13,
"RETROK_PAUSE",//19,
"RETROK_ESCAPE",//27,
"RETROK_SPACE",//32,
"RETROK_EXCLAIM",//33,
"RETROK_QUOTEDBL",//34,
"RETROK_HASH",//35,
"RETROK_DOLLAR",//36,
"RETROK_AMPERSAND",//38,
"RETROK_QUOTE",//39,
"RETROK_LEFTPAREN",//40,
"RETROK_RIGHTPAREN",//41,
"RETROK_ASTERISK",//42,
"RETROK_PLUS",//43,
"RETROK_COMMA",//44,
"RETROK_MINUS",//45,
"RETROK_PERIOD",//46,
"RETROK_SLASH",//47,
"RETROK_0",//48,
"RETROK_1",//49,
"RETROK_2",//50,
"RETROK_3",//51,
"RETROK_4",//52,
"RETROK_5",//53,
"RETROK_6",//54,
"RETROK_7",//55,
"RETROK_8",//56,
"RETROK_9",//57,
"RETROK_COLON",//58,
"RETROK_SEMICOLON",//59,
"RETROK_LESS",//60,
"RETROK_EQUALS",//61,
"RETROK_GREATER",//62,
"RETROK_QUESTION",//63,
"RETROK_AT",//64,
"RETROK_LEFTBRACKET",//91,
"RETROK_BACKSLASH",//92,
"RETROK_RIGHTBRACKET",//93,
"RETROK_CARET",//94,
"RETROK_UNDERSCORE",//95,
"RETROK_BACKQUOTE",//96,
"RETROK_a",//97,
"RETROK_b",//98,
"RETROK_c",//99,
"RETROK_d",//100,
"RETROK_e",//101,
"RETROK_f",//102,
"RETROK_g",//103,
"RETROK_h",//104,
"RETROK_i",//105,
"RETROK_j",//106,
"RETROK_k",//107,
"RETROK_l",//108,
"RETROK_m",//109,
"RETROK_n",//110,
"RETROK_o",//111,
"RETROK_p",//112,
"RETROK_q",//113,
"RETROK_r",//114,
"RETROK_s",//115,
"RETROK_t",//116,
"RETROK_u",//117,
"RETROK_v",//118,
"RETROK_w",//119,
"RETROK_x",//120,
"RETROK_y",//121,
"RETROK_z",//122,
"RETROK_DELETE",//127,
"RETROK_KP0",//256,
"RETROK_KP1",//257,
"RETROK_KP2",//258,
"RETROK_KP3",//259,
"RETROK_KP4",//260,
"RETROK_KP5",//261,
"RETROK_KP6",//262,
"RETROK_KP7",//263,
"RETROK_KP8",//264,
"RETROK_KP9",//265,
"RETROK_KP_PERIOD",//266,
"RETROK_KP_DIVIDE",//267,
"RETROK_KP_MULTIPLY",//268,
"RETROK_KP_MINUS",//269,
"RETROK_KP_PLUS",//270,
"RETROK_KP_ENTER",//271,
"RETROK_KP_EQUALS",//272,
"RETROK_UP",//273,
"RETROK_DOWN",//274,
"RETROK_RIGHT",//275,
"RETROK_LEFT",//276,
"RETROK_INSERT",//277,
"RETROK_HOME",//278,
"RETROK_END",//279,
"RETROK_PAGEUP",//280,
"RETROK_PAGEDOWN",//281,
"RETROK_F1",//282,
"RETROK_F2",//283,
"RETROK_F3",//284,
"RETROK_F4",//285,
"RETROK_F5",//286,
"RETROK_F6",//287,
"RETROK_F7",//288,
"RETROK_F8",//289,
"RETROK_F9",//290,
"RETROK_F10",//291,
"RETROK_F11",//292,
"RETROK_F12",//293,
"RETROK_F13",//294,
"RETROK_F14",//295,
"RETROK_F15",//296,
"RETROK_NUMLOCK",//300,
"RETROK_CAPSLOCK",//301,
"RETROK_SCROLLOCK",//302,
"RETROK_RSHIFT",//303,
"RETROK_LSHIFT",//304,
"RETROK_RCTRL",//305,
"RETROK_LCTRL",//306,
"RETROK_RALT",//307,
"RETROK_LALT",//308,
"RETROK_RMETA",//309,
"RETROK_LMETA",//310,
"RETROK_LSUPER",//311,
"RETROK_RSUPER",//312,
"RETROK_MODE",//313,
"RETROK_COMPOSE",//314,
"RETROK_HELP",//315,
"RETROK_PRINT",//316,
"RETROK_SYSREQ",//317,
"RETROK_BREAK",//318,
"RETROK_MENU",//319,
"RETROK_POWER",//320,
"RETROK_EURO",//321,
"RETROK_UNDO",//322,
NULL
};

int keyVal[] = {
0,
8,
9,
12,
13,
19,
27,
32,
33,
34,
35,
36,
38,
39,
40,
41,
42,
43,
44,
45,
46,
47,
48,
49,
50,
51,
52,
53,
54,
55,
56,
57,
58,
59,
60,
61,
62,
63,
64,
91,
92,
93,
94,
95,
96,
97,
98,
99,
100,
101,
102,
103,
104,
105,
106,
107,
108,
109,
110,
111,
112,
113,
114,
115,
116,
117,
118,
119,
120,
121,
122,
127,
256,
257,
258,
259,
260,
261,
262,
263,
264,
265,
266,
267,
268,
269,
270,
271,
272,
273,
274,
275,
276,
277,
278,
279,
280,
281,
282,
283,
284,
285,
286,
287,
288,
289,
290,
291,
292,
293,
294,
295,
296,
300,
301,
302,
303,
304,
305,
306,
307,
308,
309,
310,
311,
312,
313,
314,
315,
316,
317,
318,
319,
320,
321,
322,
-1
};

