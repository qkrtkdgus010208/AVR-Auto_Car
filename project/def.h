/*
 * def.h
 *
 * Created: 2026-06-22 오후 1:20:20
 *  Author: kccistc
 */ 


#ifndef DEF_H_
#define DEF_H_

typedef enum 
{
	MANAUL_MODE,	  // 0
	AUTO_MODE,        // 1
	AUTO_MODE_CHECK,  // 2
	DISTANCE_CHECK    // 3
} TypeMode;

typedef enum
{
	FORWARD,	// 0
	LEFT,       // 1
	RIGHT,		// 2
	BACKWARD,   // 3
	STOP		// 4
} Drive_State;

extern TypeMode type_mode;
extern Drive_State cur_drive_state;
extern Drive_State pre_drive_state;

void mode_change(void);

#endif /* DEF_H_ */