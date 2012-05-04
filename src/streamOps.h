#ifndef _STREAMOPS_H_
#define _STREAMOPS_H_

// streamOps.h
//
// Connected STREAM sockets do not return bytes in a regular way.
// Sometimes, when you ask for 10 bytes on a read, you get 6, and
// the next read you do will deliver you the other 4.
//
// The "StreamRead" abstracts the complications in handling such
// reading.  Supply the number of bytes, and the StreamRead will
// deliver the bytes into a buffer.
//
// This is the C++ version of streamops.
// A streamops object constructs with a file descriptor.
//

#define STREAMOPSBUFFERFULL -2
#define STREAMOPSBUFFEREMPTY 0

class StreamOps
{
public:
	StreamOps(int fd) {streamfd = fd;};
	int sRead(unsigned char * Buffer, unsigned int Length, char * Description);
	int sReadDelimited(unsigned char * Buffer, unsigned int Length, unsigned char Delimiter, char * Description);
private:
	int streamfd;
};


#endif // _STREAMOPS_H_


