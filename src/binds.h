#ifndef BINDS_H
#define BINDS_H

#include <ncurses.h>

/* Flags to be or'ed in */
#define KEY_CTRL	0x10000000
#define KEY_META    	0x20000000
#define KEY_CTLX    	0x40000000
#define	KEY_SPEC	0x80000000

/* Control sequences */
#define CTRL_DOWN	527
#define CTRL_UP		571
#define CTRL_LEFT	547
#define CTRL_RIGHT	565
#define CTRL_A		1
#define CTRL_B		2
#define CTRL_C		3
#define CTRL_D		4
#define CTRL_E		5
#define CTRL_F		6
#define CTRL_G		7
#define CTRL_H		8
#define CTRL_I		9
#define CTRL_J		10
#define CTRL_K		11
#define CTRL_L		12
#define CTRL_M		13
#define CTRL_N		14
#define CTRL_O		15
#define CTRL_P		16
#define CTRL_Q		17
#define CTRL_R		18
#define CTRL_S		19
#define CTRL_T		20
#define CTRL_U		21
#define CTRL_V		22
#define CTRL_W		23
#define CTRL_X		24
#define CTRL_Y		25
#define CTRL_Z		26

/* Binds */
#define BIND_EXIT 	KEY_F(1)
#define BIND_SAVE_EXIT 	KEY_F(3)

#endif
