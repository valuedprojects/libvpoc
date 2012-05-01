//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "HexOps.h"

#include <stdio.h>
#include <ctype.h>

#ifndef ui8
#define ui8 unsigned char
#endif

#ifndef ui32
#define ui32 int
#endif

#ifndef printorspace
#define printorspace(A) (isprint(A) || isspace(A))
#endif

// Take a pointer to memory, a file pointer, a filename
// take an address to a block of space (must be 64 bytes).
// put a Hex representation of the memory in text.
// Takes a pointer into memory, and creates a 64 character line that shows
// the Hex dump of that area.  A 0-0x0f value can be supplied to indicate
// the 1st character after "memArea" that will be represented (characters
// before "offset" in the line will be represented with "--" rather than
// with their hex values).  "Address" is the label at the begining of the
// line that represents the start address of the data shown in the line.

void formatByteHexLine (
    ui8 * memArea,        // Pointer into memory
    const ui8 offset,           // 0-0x0f offset from memArea to start dump.
    char * formattedOutput,     // Where to place to formatted dump
    ui32 AddressLable           // Label to use for the 1st address
    )
{
    char * p = formattedOutput;
    ui8 * m = memArea;

    // Format the Address
    sprintf(p,"%08X:",AddressLable);
    p += 9;

    for (ui8 i=0; i<0x10; i++)
    {
        if (!(i%4))
            sprintf(p++," ");

        if (i >= offset)
            sprintf(p,"%02X",*m);
        else
            sprintf(p,"--");

        m++; p+=2;
    }

    // Produce a character representation of the data into the
    // formatted text area.
    m = memArea;
    sprintf(p++, " ");
    for (int i=0; i<0x10; i++)
        {
            if (i<offset)
                sprintf(p++,"\200");
            else if (printorspace(*m))
                sprintf(p++,"%c",*m);
            else
                sprintf(p++, ".");
            m++;
        }
}

//---------------------------------------------------------------------------
#pragma package(smart_init)
 