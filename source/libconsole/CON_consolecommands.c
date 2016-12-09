/*  CON_consolecommands.c
 *  Written By: Garrett Banuk <mongoose@mongeese.org>
 *  This is free, just be sure to give me credit when using it
 *  in any of your programs.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "SDL/SDL.h"
#include "CON_console.h"
#include "internal.h"


/* Linked list of all the commands. */
static CommandInfo	*Commands = NULL;
static int		SendCommand = 0;


/* executes the command passed in from the string */
void CON_CommandExecute(ConsoleInformation *console)
{
  char Command[CON_CHARS_PER_LINE];
  char *BackStrings = console->Command;
  CommandInfo *CurrentCommand = Commands;

  /* Get the command out of the string */
  if(EOF == sscanf(BackStrings, "%s", Command))
    return;

  CurrentCommand = Commands;
  while(CurrentCommand) {
    if(0 == strcmp(Command, CurrentCommand->CommandWord)) {
      if (SendCommand)
        CurrentCommand->CommandCallback(console, BackStrings);
      else
        CurrentCommand->CommandCallback(console, BackStrings+strlen(Command)+1);
      return;
    }
    CurrentCommand = CurrentCommand->NextCommand;
  }
  //CON_Out(console, "Unknown Command \"%s\"", Command);
  CON_Out(console, "Nieznana komenda: %s ", Command);
}

/* Use this to add commands.
 * Pass in a pointer to a funtion that will take a string which contains the
 * arguments passed to the command. Second parameter is the command to execute
 * on.
 */
void CON_AddCommand(void(*CommandCallback)(ConsoleInformation *console, char *Parameters), const char *CommandWord)
{
  CommandInfo	**CurrentCommand = &Commands;


  while(*CurrentCommand)
    CurrentCommand = &((*CurrentCommand)->NextCommand);

  /* Add a command to the list */
  *CurrentCommand = (CommandInfo*)malloc(sizeof(CommandInfo));
  (*CurrentCommand)->CommandCallback = CommandCallback;
  (*CurrentCommand)->CommandWord = (char*)malloc(strlen(CommandWord)+1);
  strcpy((*CurrentCommand)->CommandWord, CommandWord);
  (*CurrentCommand)->NextCommand = NULL;
}

/* tab completes commands
 * It takes a string to complete and a pointer to the strings length. The strings
 * length will be modified if the command is completed. */
void CON_TabCompletion(ConsoleInformation *console)
{
  int Matches = 0;
  int *location = &console->CursorPos;
  char *CommandLine = console->LCommand;
  char temp[CON_CHARS_PER_LINE];
  CommandInfo	*CurrentCommand;

  // Don't try to complete if nothing is entered
  if(*location == 0)
    return;


  /* Find all the commands that match */
  CurrentCommand = Commands;
  while(CurrentCommand)
    {
      if(0 == strncmp(CommandLine, CurrentCommand->CommandWord, strlen(CommandLine)))
        Matches++;
      CurrentCommand = CurrentCommand->NextCommand;
    }

  /* if we got one match, find it again and execute it */
  if(Matches == 1)
    {
      CurrentCommand = Commands;
      while(CurrentCommand)
	{
	  if(0 == strncmp(CommandLine, CurrentCommand->CommandWord, strlen(CommandLine)))
	    {
	      strcpy(CommandLine, CurrentCommand->CommandWord);
	      CommandLine[strlen(CurrentCommand->CommandWord)] = ' ';
	      *location = strlen(CurrentCommand->CommandWord)+1;
	      CON_UpdateConsole(console);
	      break;
	    }
	  CurrentCommand = CurrentCommand->NextCommand;
	}
    }
  else if(Matches > 1)/* multiple matches so print them out to the user */
    {
      CON_NewLineConsole(console);
      CurrentCommand = Commands;
      while(CurrentCommand)
	{
	  if(0 == strncmp(CommandLine, CurrentCommand->CommandWord, strlen(CommandLine))) {
	    strcpy(temp, "     ");
	    strcpy(&temp[5], CurrentCommand->CommandWord);
	    CON_Out(console, temp);
	  }
	  CurrentCommand = CurrentCommand->NextCommand;
	}
    }
  else if(Matches == 0) {
    // Thats not very interesting..
    //CON_Out(console, "No match");
  }
}


/* Lists all the commands to be used in the console */
void CON_ListCommands(ConsoleInformation *console)
{
  CommandInfo	*CurrentCommand = Commands;

  CON_Out(console, " ");
  while(CurrentCommand)
    {
      CON_Out(console, "  %s", CurrentCommand->CommandWord);
      CurrentCommand = CurrentCommand->NextCommand;
    }

}

/* sends the command word used and the parameters back */
void CON_SendFullCommand(int sendOn)
{
  SendCommand = sendOn;
}

/* This frees all the memory used by the commands */
void CON_DestroyCommands()
{
  CommandInfo	*CurrentCommand = Commands;
  CommandInfo	*temp;

  while(CurrentCommand)
    {
      temp = CurrentCommand;
      CurrentCommand = CurrentCommand->NextCommand;

      free(temp->CommandWord);
      free(temp);
    }
  Commands = NULL;
}


