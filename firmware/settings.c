#include "config.h"
#include "settings.h"
#include "minfat.h"
#include "statusword.h"
#include "menu.h"
#include "user_io.h"
#include "spi.h"

/* If settings files are supported, we provide a default implementation of loading and saving configs,
   which can be replaced in overrides.c */

extern fileTYPE file; /* Import from main.c */

int scandouble=0;
void ToggleScandoubler()
{
	scandouble^=1;
	SPI(0xff);
	EnableIO();
	SPI(UIO_BUT_SW); // Set "DIP switch" for scandoubler
	SPI(scandouble<<4);
	DisableIO();
}

void AutoScandoubler()
{
	if(FileOpen(&file,AUTOSCANDOUBLER_FILENAME))
		ToggleScandoubler();
}

#ifdef CONFIG_SETTINGS

__weak int configtocore(char *buf)
{
	unsigned int *b=(unsigned int *)buf;
	statusword=*b++;
	scandouble=1^(*b++); // Invert retrieved scandoubler setting since we invert it again when setting it. 
	ToggleScandoubler();
	sendstatus();
	return(1);
}

__weak void coretoconfig(char *buf)
{
	unsigned int *b=(unsigned int *)buf;
	*b++=statusword;
	*b++=scandouble;
}

__weak int loadsettings(const char *filename)
{
	if(filename && FileOpen(&file,filename))
	{
		if(FileReadSector(&file,sector_buffer))
		{
			return(configtocore(sector_buffer));	
		}
	}
	return(0);
}

__weak int savesettings(const char *filename)
{
	if(filename && FileOpen(&file,filename))
	{
		coretoconfig(sector_buffer);	
		if(FileWriteSector(&file,sector_buffer))
			return(1);
	}
	return(0);
}

#endif

