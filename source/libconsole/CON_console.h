#ifndef CON_console_H
#define CON_console_H


#define CON_CHARS_PER_LINE   128
#define CON_BLINK_RATE       500
#define CON_CHAR_BORDER      4
#define CON_DEFAULT_PROMPT	"]"
#define CON_LINE_SCROLL	2
#define CON_SCROLL_INDICATOR "^"
#define CON_INS_CURSOR "_"
#define CON_OVR_CURSOR "|"


#ifdef __cplusplus
extern "C"
{
#endif

/* This is a struct for each consoles data */
typedef struct console_information_td
{
	char **ConsoleLines;		// List of all the past lines
	char **CommandLines;		// List of all the past commands
	int TotalConsoleLines;		// Total number of lines in the console
	int ConsoleScrollBack;		// How much the users scrolled back in the console
	int TotalCommands;		// Number of commands in the Back Commands
	int FontNumber;			// This is the number of the font for the console
	int LineBuffer;			// The number of visible lines in the console (autocalculated)
	int VChars;			// The number of visible characters in one console line (autocalculated)
	int BackX, BackY;		// Background images x and y coords
	char* Prompt;			// Prompt displayed in command line
	char Command[CON_CHARS_PER_LINE];	// current command in command line = lcommand + rcommand
	char RCommand[CON_CHARS_PER_LINE];	// left hand side of cursor
	char LCommand[CON_CHARS_PER_LINE];	// right hand side of cursor
	char VCommand[CON_CHARS_PER_LINE];	// current visible command line
	int CursorPos;			// Current cursor position in CurrentCommand 
	int Offset;			// CommandOffset (first visible char of command) - if command is too long to fit into console
	int InsMode;			// Insert or Overwrite characters?
	SDL_Surface *ConsoleSurface;	// Surface of the console
	SDL_Surface *OutputScreen;	// This is the screen to draw the console to
	SDL_Surface *BackgroundImage;	// Background image for the console
	SDL_Surface *InputBackground;	// Dirty rectangle to draw over behind the users background
	int DispX, DispY;		// The top left x and y coords of the console on the display screen
	unsigned char ConsoleAlpha;	// The consoles alpha level
	int CommandScrollBack;		// How much the users scrolled back in the command lines
} ConsoleInformation;



void    CON_Events(SDL_Event *event);
void    CON_DrawConsole(ConsoleInformation *console);
ConsoleInformation  *CON_Init(const char *FontName, SDL_Surface *DisplayScreen, int lines, SDL_Rect rect);
void    CON_Destroy(ConsoleInformation *console);
void    CON_Free(ConsoleInformation *console);
void    CON_Out(ConsoleInformation *console, const char *str, ...);
void    CON_Alpha(ConsoleInformation *console, unsigned char alpha);
int CON_Background(ConsoleInformation *console, const char *image, int x, int y);
void    CON_Position(ConsoleInformation *console, int x, int y);
int CON_Resize(ConsoleInformation *console, SDL_Rect rect);
void    CON_NewLineConsole(ConsoleInformation *console);
void    CON_NewLineCommand(ConsoleInformation *console);
void    CON_UpdateConsole(ConsoleInformation *console);
void    CON_Topmost(ConsoleInformation *console);
void    CON_SetPrompt(ConsoleInformation *console, char* newprompt);

void	Cursor_Left(ConsoleInformation *console);
void	Cursor_Right(ConsoleInformation *console);
void	Cursor_Home(ConsoleInformation *console);
void	Cursor_End(ConsoleInformation *console);
void	Cursor_Del(ConsoleInformation *console);
void	Cursor_BSpace(ConsoleInformation *console);
void	Cursor_Add(ConsoleInformation *console, SDL_Event *event);

void	Clear_Command(ConsoleInformation *console);
void	Clear_History(ConsoleInformation *console);


//Console Commands

typedef struct CommandInfo_td
{
	void			(*CommandCallback)(ConsoleInformation *console, char *Parameters);
	char			*CommandWord;
	struct CommandInfo_td	*NextCommand;
} CommandInfo;


void    CON_SendFullCommand(int sendOn);
void    CON_CommandExecute(ConsoleInformation *console);
void    CON_AddCommand(void (*CommandCallback)(ConsoleInformation *console, char *Parameters), const char *CommandWord);
void    CON_TabCompletion(ConsoleInformation *console);
void    CON_ListCommands(ConsoleInformation *console);
void    CON_DestroyCommands();


#ifdef __cplusplus
};
#endif

#endif


