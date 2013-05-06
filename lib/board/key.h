#ifndef _KEY_H
#define _KEY_H

/**
	* @brief	Initializes the Key GPIO
	*/
extern void InitKey(void);

/**
	* @brief	return the number of key presses since last time it was checked
	* @return count of keys presses 
	*/
extern unsigned KeyPressCount(void);

/**
	* @brief	wait for key pressed, ignored any previous unchecked pressses
	*/
extern void WaitForKeyPress(void);


#endif
