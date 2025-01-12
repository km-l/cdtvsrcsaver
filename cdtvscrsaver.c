#include <stdio.h>
#include <string.h>
#include <cdtv/bookmark.h>
#include <cdtv/cdtvprefs.h>
#include <cdtv/screensaver.h>
#include <exec/types.h>
#include <exec/execbase.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include <proto/exec.h>

#define OS_20 37

static UBYTE VersTag[] = "\0$VER: cdtvscrsaver 0.91 (01.12.2025) (c) Jan.2025 by km-l";

extern struct ExecBase *SysBase;
struct Library *PlayerPrefsBase;
struct CDTVPrefs cdtvPrefs;

VOID Quit(UBYTE *s);
VOID CheckOS(UWORD ver);

int main()
{
    CheckOS(OS_20);

    PlayerPrefsBase = OpenLibrary("playerprefs.library", 0);
    if(!PlayerPrefsBase)
	    Quit("Playerprefs library will not open.");

    UBYTE i, bookmark;

    bookmark = ReadBookMark(&cdtvPrefs, sizeof(cdtvPrefs), BID_CDTVPREFS);
    if(!bookmark)
        Quit("Reading of the bookmark failed.");

    LONG params[5];
    struct RDArgs *rd;
	UBYTE template[40];
	
	strcpy(template, "HELP/S,TIME/K/N,INFO/S,INSTALL/S,KILL/S");
	
	for(i=0; i<5; i++)
		params[i] = 0;
	
	rd = ReadArgs(template, params, NULL);

    if(rd)
	{
		if(params[0])
			printf("Command line tool for managing the CDTV screensaver.\n\nTIME - sets the screensaver activation time in minutes.\nINFO - shows the current screensaver activation time.\nHELP - shows these tips.\nINSTALL - install the screensaver.\nKILL - disable the screensaver.\n\n");

		if(params[1])
		{	
			LONG *savertime;
			savertime = (LONG*)params[1];
			cdtvPrefs.SaverTime = *savertime;
			bookmark = WriteBookMark(&cdtvPrefs, sizeof(cdtvPrefs), BID_CDTVPREFS, 0);
            if(!bookmark)
                Quit("Bookmark saving failed.");
			ConfigureCDTV(&cdtvPrefs);
			printf("The screensaver activation time is set to %d min.\n", cdtvPrefs.SaverTime);
		}

		if(params[2])
		{
			if(FindPort("CDTV Screen Saver"))
				printf("The screensaver is active.\n");
			else
				printf("The screensaver is inactive.\n");
			printf("The current screensaver activation time is set to %d min.\n", cdtvPrefs.SaverTime);
		}

		if(params[3])
		{
			if(FindPort("CDTV Screen Saver"))
				printf("The screensaver is already running.\n");
			else
			{
				InstallScreenSaver();
				printf("The screensaver has been activated.\n");
			}
		}

		if(params[4])
		{
			ScreenSaverCommand(SCRSAV_DIE);
			printf("The screensaver has been disabled.\n");	
		}
		FreeArgs(rd);
	}
	else
		printf("Format: cdtvsaver HELP/S,TIME/K/N,INFO/S,INSTALL/S,KILL/S\n");

    Quit(0);
}

VOID Quit(UBYTE *s)
{
    if(PlayerPrefsBase)
    	CloseLibrary(PlayerPrefsBase);
	if(s)
    {
        printf("ERROR: %s\n", s);
        exit(40);
    }
    else
        exit(0);
}

VOID CheckOS(UWORD ver)
{
    if(SysBase->LibNode.lib_Version < ver)
	{
        printf("This program requires OS2.0 or higher.\n");
    	exit(0);
	}
}
