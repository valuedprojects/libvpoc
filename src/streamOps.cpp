// StreamOps.c
//
// Functions to handle stream reading of bytes from a stream socket.

#include "streamOps.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

using namespace std;

int StreamOps::sRead (unsigned char * Buffer, unsigned int Length, char * Description)
{
  unsigned int BytesToGet = Length;
  unsigned int BytesStillNeeded = Length;
  int status;

  char ErrBuff[128];
  
  while (BytesStillNeeded)
	{
	  // Debugging...
	  // Notify if we don't get all the bytes asked for in the call the 1st time...
	  if (Length != BytesStillNeeded)
		  printf("Must complete read. %d of %d bytes remain.\n",
				  BytesStillNeeded, BytesToGet);
	  if ( (status=read(streamfd, Buffer+(BytesToGet-BytesStillNeeded), BytesStillNeeded)) < 0 )
		{
		  // Error condition on read.
		  sprintf(ErrBuff, "Problem reading \"%s\".", Description);
		  perror (ErrBuff);
		  return status;
		}
	  else if (status == 0)
		{
		  // The peer disconnected the connection
		  printf ("Peer closed connection\n");
		  return 0;
		}
	  else
		{
		  // We got some bytes.  Fill Buffer:
		  BytesStillNeeded -= status;
		}
	}

  return (BytesToGet-BytesStillNeeded);
}


int StreamOps::sReadDelimited (unsigned char * Buffer, unsigned int Length, unsigned char Delimiter, char * Description)
{
  // Get one byte at a time until the character is found.
  // We can make this more efficient if we force every user of sockets
  // to go through these routines to get their data.
  // To make this more efficient, we would have to hold the buffers of data locally.

  unsigned int position = 0;
  int status = STREAMOPSBUFFERFULL;

  // Manager a case of 0 length - not a leagal call, but we need to handle it.
  if (0 == Length)
	return status;
  
  while (position < (Length-1))
	{
	  if ( (status = sRead (Buffer+position, 1, Description)) != 1 )
		{
		  // This is an annomalous condition, so break the loop and exit the routine.
		  break;
		}
	  if (Delimiter == *(Buffer+position))
		{
		  // This is the indication that we have read enough bytes.
		  // Set "status" to the number of bytes used in the "Buffer"
		  // (this includes the delimiter byte).
		  status = position + 1;
		  break;
		}
	  position++;
	}
  if (position == (Length-1))
	{
	  // We've run out of space in the buffer, so the last byte in the Buffer will
	  // be the caller specified delimiter.
	  *(Buffer+position) = Delimiter;
	  status = STREAMOPSBUFFERFULL;
	}

  return status;
}

