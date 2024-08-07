/** ****************************************************************************************************
 * @file 			TerminalEmulatorSupport.c
 * @brief			File supports terminal emulation functions.  To be used on any console applications
 * that supports the ANSI Escape Code sequence.  Recommend using Tera Term (free) terminal Emulation software
 * ****************************************************************************************************
 * @author original	Hab Collector (habco)\n
 *
 * @version          1.0
 *
 * @param Development_Environment \n
 * Hardware:        STM32 \n
 * IDE:             STMCubeIDE VER 1.9.0 \n
 * Compiler:        GCC \n
 * Editor Settings: 1 Tab = 4 Spaces, Recommended Courier New 11
 *
 * @note            ANSI Escape works with printf() - printf() must be set up to target your intended console output
 *                  See: https://en.wikipedia.org/wiki/ANSI_escape_code
 *
 *                  Control of a terminal emulators - colors, screen and cursor actions
 *                  For use with applications such as Tera Term (recommended)
 *
 * @copyright		Applied Concepts, Inc
 ********************************************************************************************************/
#include "TerminalEmulatorSupport.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


/*******************************************************************************************************
* @brief Prints via printf() text in the desired fore and background color.  After printing the default
* fore and background colors are resumed.
*
* @author original: Hab Collector \n
*
* @param ForeGroundColor: Text color
* @param BackGroundColor: Text background color
* @param UserString: String to print
* @param ...: User string variable arguments
*
* @return void
*
* STEP 1: Set text color
* STEP 2: Set text background color
* STEP 3: Print text as desired
* STEP 4: Reset to normal color palette
********************************************************************************************************/
#ifdef USE_STATIC_STORAGE
static STATIC_STORAGE char String[MAX_PRINT_MESSAGE_SIZE];
#else
static char String[MAX_PRINT_MESSAGE_SIZE];
#endif
void printColor(Type_PrintColor ForeGroundColor, Type_PrintColor BackGroundColor, const char *UserString, ... )
{
	va_list UserArguments;
	va_start(UserArguments, UserString);
	vsprintf(String, UserString, UserArguments);
	va_end(UserArguments);

	// STEP 1: Set text color
	printf("\033[38;5;%dm", ForeGroundColor);

	// STEP 2: Set text background color
	printf("\033[48;5;%dm", BackGroundColor);

	// STEP 3: Print text as desired
	printf("%s", String);

	// STEP 4: Reset to normal color palette
	printf(RESET_COLORS_TO_DEFAULT);

} // END OF printColor



/*******************************************************************************************************
* @brief Clears the terminal screen
*
* @author original: Hab Collector \n
*
********************************************************************************************************/
void terminal_ClearScreen(void)
{
	printf(ERASE_DISPLAY);
	fflush(stdout);

} // END OF terminal_ClearScreen



/*******************************************************************************************************
* @brief Move the cursor back one space - this is equivlent to a back space
*
* @author original: Hab Collector \n
*
********************************************************************************************************/
void terminal_CursorBackSpace(void)
{
	printf(CURSOR_BACK_SPACE);
	fflush(stdout);

} // END OF terminal_CursorBackSpace



/*******************************************************************************************************
* @brief Set default text and background color for the terminal
*
* @author original: Hab Collector \n
*
********************************************************************************************************/
void terminal_SetDefaultColorPalette(void)
{
	printf(RESET_COLORS_TO_DEFAULT);

} // END OF terminal_SetDefaultForegroundColor



#ifdef USE_EXTENDED_TERMINAL_FUNCTIONS
// The following function set the text forecolor: Use with caution color varies based on display type and app
// Better to use printColor()
void terminal_SetDefaultForegroundColor(void)
{
	printf(DEFAULT_FOREGROUND_COLOR);
	fflush(stdout);

}

void terminal_SetRedForeground(void)
{
	printf(SET_RED_FOREGROUND);
	fflush(stdout);
}

void terminal_SetBrightRedForeground(void)
{
	printf(SET_BRIGHT_RED_FOREGROUND);
	fflush(stdout);
}

void terminal_SetGreenForeground(void)
{
	printf(SET_GREEN_FOREGROUND);
	fflush(stdout);
}

void terminal_SetBrightGreenForeground(void)
{
	printf(SET_BRIGHT_GREEN_FOREGROUND);
	fflush(stdout);
}

void terminal_SetYellowForeground(void)
{
	printf(SET_YELLOW_FOREGROUND);
	fflush(stdout);
}

void terminal_SetBrightYellowForeground(void)
{
	printf(SET_BRIGHT_YELLOW_FOREGROUND);
	fflush(stdout);
}

void terminal_SetBlueForeground(void)
{
	printf(SET_BLUE_FOREGROUND);
	fflush(stdout);
}

void terminal_SetBrightBlueForeground(void)
{
	printf(SET_BRIGHT_BLUE_FOREGROUND);
	fflush(stdout);
}


// The following functions set the text background color: Use with caution color varies based on display type and app
// Better to use printColor()
void terminal_SetGreenBrackground(void)
{
	printf(SET_GREEN_BACKGROUND);
}

void terminal_SetBrightGreenBrackground(void)
{
	printf(SET_BRIGHT_GREEN_BACKGROUND);
}


// The following functions are direct print just that text to a specific color
void printRed(const char *String)
{
	terminal_SetRedForeground();
	printf("%s", String);
	terminal_SetDefaultForegroundColor();
}

void printBrightRed(const char *String)
{
	terminal_SetBrightRedForeground();
	printf("%s", String);
	terminal_SetDefaultForegroundColor();
}

void printGreen(const char *String)
{
	terminal_SetGreenForeground();
	printf("%s", String);
	terminal_SetDefaultForegroundColor();
}

void printBrightGreen(const char *String)
{
	terminal_SetBrightGreenForeground();
	printf("%s", String);
	terminal_SetDefaultForegroundColor();
}

void printYellow(const char *String)
{
	terminal_SetYellowForeground();
	printf("%s", String);
	terminal_SetDefaultForegroundColor();
}

void printBrightYellow(const char *String)
{
	terminal_SetBrightYellowForeground();
	printf("%s", String);
	terminal_SetDefaultForegroundColor();
}

void printBlue(const char *String)
{
	terminal_SetBlueForeground();
	printf("%s", String);
	terminal_SetDefaultForegroundColor();
}

void printBrightBlue(const char *String)
{
	terminal_SetBrightBlueForeground();
	printf("%s", String);
	terminal_SetDefaultForegroundColor();
}
#endif
