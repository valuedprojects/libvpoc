#ifndef _TCPUTILS_H_
#define _TCPUTILS_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/*
* tcp_PF_UNIX_srv_open - Used by an application to set up a file descriptor for
* a PF_UNIX (i.e. filename) style server socket.
*/
int tcp_PF_UNIX_srv_open (char * pathName);

/*
* tcp_PF_INET_srv_open - Used by an application to set up a file descriptor for
* a PF_INET (i.e. internet: host/port) style server socket.
*/
int tcp_PF_INET_srv_open (char * service, int port);

/*
* tcp_PF_INET_cl_open - Used by a client application to setup a connection to a
 * PF_INET server.  The returned file descriptor will be used for to send data
 * between client and server.
 */
int tcp_PF_INET_cl_open(char * host, char * service, int port);

/*
* tcp_PF_UNIX_cl_open - Used by a client application to setup a connection to a
* PF_UNIX server.  The returned file descriptor will be used for to send data
* between client and server.
*/
int tcp_PF_UNIX_cl_open(char * pathName);

// TODO
// Provide more comments about this function.
// Why does the "serverProcessing" function prototype have 2 fd parameters when,
// in a socket connection, you only get 1 back and you can use that one for
// both writing and reading?  Because, we want the tcp_ServerStartup function to
// be able to use a "Processing" function that may also have been used with some
// other type of IPC stream protocol, like, maybe, a "pipe".  And in general,
// streaming mechanisms will have a "read" fd, and a "write" fd.
// The tcp_ServerStartup function will give the same socket fd as both parameters
// when tcp_ServerStartup calls the serverProcessing function pointer.
int tcp_ServerStartup (int portNum, char * pathName, int (*serverProcessing)(int read_fd, int write_fd));

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif // _TCPUTILS_H_
