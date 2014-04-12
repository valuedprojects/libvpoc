#include "hexDump.h"
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

bool debugOn = false;

void setDebug(bool on)
{
	debugOn = on;
}

void hexdump(void * data, int length)
{
	unsigned char * charData = (unsigned char*)data;
	int BytesLeftToDump = length;
	int BytesDumped = 0;
	int SetStart = 0;
	int LineCharCount = 0; // Use this to track characters on the line so we don't use weird mods and stuff.
	if (debugOn) printf("BytesLeftToDump: %d\n", BytesLeftToDump);
	// Write characters in sets of 32.
	while (BytesLeftToDump)
	{
		printf("%02X",*(charData+BytesDumped));
		++BytesDumped;
		--BytesLeftToDump;
		LineCharCount += 2;
		if (0==BytesLeftToDump || 0==(BytesDumped % 32))
		{
			// Dump character, and insert a linefeed
			for (int i=0;i<(71-LineCharCount+3);i++)
				printf(" ");
			for (int i=SetStart; i<BytesDumped; i++)
				printf("%c",isprint(*(charData+i))?*(charData+i):'.');
			printf("\n");
			if (debugOn) printf("BytesLeftToDump: %d\n", BytesLeftToDump);
			SetStart = BytesDumped;
			LineCharCount = 0;
		}
		else if (0==(BytesDumped%4)) { printf(" "); LineCharCount++; }
	}
}
