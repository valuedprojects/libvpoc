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
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

#define STREAMOPSBUFFERFULL -2
#define STREAMOPSBUFFEREMPTY 0
extern int StreamRead (int streamfd, unsigned char * Buffer, unsigned int Length, char * Description);
extern int StreamReadDelimited (int streamfd, unsigned char * Buffer, unsigned int Length, unsigned char Delimiter, char * Description);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // _STREAMOPS_H_


