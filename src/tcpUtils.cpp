/*
 * tcpUtils.cpp
 * 
 * Experimenting with a C implementation of a TCP socket connection utility.
 * The functions in this code allow establishing a TCP connection with INET
 * protocol (i.e. host / port), or UNIX protocol (i.e. a pathname).
 *
 */
#include "tcpUtils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>       // for internet struct sockaddr_in
#include <netdb.h>
#include <arpa/inet.h>        // for inet_addr function

#include <sys/un.h>           // for PF_UNIX sockets

#include <signal.h>           // for signal handling - exitted connections.
#include <sys/wait.h>

#include <stdio.h>
#include <string.h>           // for memset
#include <stdlib.h>           // for "exit"
#include <unistd.h>           // for "fork"



static int tcp_bind_setup_fd (int fd, sockaddr * pGenericSocketAddress)
{
	socklen_t addrlen;
	// Find size of socket:
	if (pGenericSocketAddress->sa_family == PF_INET)
		addrlen = sizeof(sockaddr_in);
	else if (pGenericSocketAddress->sa_family == PF_UNIX)
		addrlen = sizeof(sockaddr_un);
	else
	{
		fprintf(stderr, "%s: Unexpected Protocol Family: %d\n", __FUNCTION__, pGenericSocketAddress->sa_family);
		return (-1);
	}
	// Bind the file descriptor from the "socket" call, to the address
	// structure...
	if ( bind (fd, pGenericSocketAddress, addrlen) < 0 )
	{
		char buff[256];
		sprintf(buff, "%s: bind() :", __FUNCTION__);
		perror(buff);
		return (-1);
	}

	// Set up the socket to listen for up to 5 connections at a time.
	// The actuall listening only happens when the "accept" is called.
	if (listen(fd, 5) < 0)
	{
		char buff[256];
		sprintf(buff, "%s: listen() :", __FUNCTION__);
		perror(buff);
		return (-1);
	}

	// Finally return the file descriptor for the socket...
	return (fd);
}

/*
 * tcp_PF_UNIX_srv_open - Used by an application to set up a file descriptor for
 * a PF_UNIX (i.e. filename) style server socket.
 */
int tcp_PF_UNIX_srv_open (char * pathName)
{
	int fd;
	struct sockaddr_un srv_addr;

	// Initialize the address structure...
	memset(&srv_addr, 0, sizeof(srv_addr));

	// Set the Family in the address structure...
	srv_addr.sun_family = PF_UNIX;

	// For UNIX style, set the path for the socket.
	strcpy(srv_addr.sun_path,pathName);
	printf("srv_addr.sun_path:%s\n", srv_addr.sun_path);

	// Define the server socket...
	if ( (fd = socket (PF_UNIX, SOCK_STREAM, 0)) <  0 )
	{
		char buff [256];
		sprintf(buff, "%s: socket():", __FUNCTION__);
		perror(buff);
		return (-1);
	}

	// Finally return the file descriptor for the socket...
	return (tcp_bind_setup_fd(fd, (sockaddr*)(&srv_addr)));
}



/*
 * tcp_PF_INET_srv_open - Used by an application to set up a file descriptor for
 * a PF_INET (i.e. internet: host/port) style server socket.
 */
int tcp_PF_INET_srv_open (char * service, int port)
{
	int fd;
	struct sockaddr_in srv_addr;

	// Initialize the address structure...
	memset(&srv_addr, 0, sizeof(srv_addr));

	// Set the Family in the address structure...
	srv_addr.sin_family = PF_INET;

	// Set the port in the address structure...
	if (port)
	{
		// Non-zero port, take the port from the parameter
		srv_addr.sin_port=htons(port);
	}
	else if (service)
	{
		// Non-null service, then find the service and use this to fill in the
		// port number:
		struct servent * sp;
		if ( (sp=getservbyname(service, "tcp")) == NULL )
		{
			// Error in attempting to find the named service.
			fprintf(stderr, "%s: unknown service - %s\n", __FUNCTION__, service);
			return (-1);
		}
		srv_addr.sin_port = sp->s_port;

	}
	else
	{
		// No port or service provided.
		fprintf(stderr, "%s: No port of service provided\n", __FUNCTION__);
		return(-1);
	}

	// The server address is any address.  It's the client that needs
	// to know who it is requesting a connection from.
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Define the server socket...
	if ( (fd = socket (PF_INET, SOCK_STREAM, 0)) <  0 )
	{
		char buff [256];
		sprintf(buff, "%s: socket():", __FUNCTION__);
		perror(buff);
		return (-1);
	}

	// Finally return the file descriptor for the socket...
	return (tcp_bind_setup_fd(fd, (sockaddr*)(&srv_addr)));
}


/*
 * tcp_PF_INET_cl_open - Used by a client application to setup a connection to a
 * PF_INET server.  The returned file descriptor will be used for to send data
 * between client and server.
 */
int tcp_PF_INET_cl_open(char * host, char * service, int port)
{
	int fd;
	unsigned long inaddr;
	struct servent * sp;
	struct hostent * hp;
	struct sockaddr_in srv_addr;   // A 16 byte overlay on the general sockaddr struct.

	// Clear the structure...
	memset ( (char*)&srv_addr, 0, sizeof(srv_addr) );

	// Set Protocol Family for the address structure...
	srv_addr.sin_family = PF_INET;

	// Set the port in the address structure...
	if (port)
	{
		// Non-zero port, take the port from the parameter
		srv_addr.sin_port=htons(port);
	}
	else if (service)
	{
		// Non-null service, then find the service and use this to fill in the
		// port number:
		if ( (sp=getservbyname(service, "tcp")) == NULL )
		{
			// Error in attempting to find the named service.
			fprintf(stderr, "%s: unknown service - %s\n", __FUNCTION__, service);
			return (-1);
		}
		srv_addr.sin_port = sp->s_port;
	}
	else
	{
		// No port or service provided.
		fprintf(stderr, "%s: No port of service provided\n", __FUNCTION__);
		return(-1);
	}

	// Attempt to interpret the "host" parameter:
	if ( host )
	{
		if ( (inaddr=inet_addr(host)) != INADDR_NONE )
		{
			// The host string is an IP address.
			memcpy( (char*)&srv_addr.sin_addr, (char*)&inaddr, sizeof(inaddr) );
		}
		else
		{
			// See if the host text string is a name in to be found with gethostbyname
			if ( (hp=gethostbyname(host)) == NULL )
			{
				// This is an error, cannot find the hostname
				fprintf(stderr, "%s: hostname: %s, cannot be found\n", __FUNCTION__, host);
				return (-1);
			}
			srv_addr.sin_addr.s_addr=*((unsigned long *)(hp->h_addr));
		}
	}
	else
	{
		fprintf(stderr, "%s: No host specified\n", __FUNCTION__);
		return (-1);
	}

	// Define the server socket...
	if ( (fd = socket (AF_INET, SOCK_STREAM, 0)) <  0 )
	{
		char buff [256];
		sprintf(buff, "%s: socket():", __FUNCTION__);
		perror(buff);
		return (-1);
	}

	// Attempt to connect to the server
	if ( connect (fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr) ) < 0 )
	{
		char buff[256];
		sprintf(buff, "%s: connect(): ", __FUNCTION__);
		perror(buff);
		return (-1);
	}

	// Return the now connected File Descriptor.
	return (fd);
}


/*
 * tcp_PF_UNIX_cl_open - Used by a client application to setup a connection to a
 * PF_UNIX server.  The returned file descriptor will be used for to send data
 * between client and server.
 */
int tcp_PF_UNIX_cl_open(char * pathName)
{
	int fd;
	struct sockaddr_un srv_addr;   // pathname overlay on the general sockaddr struct.

	// Clear the structure...
	memset ( (char*)&srv_addr, 0, sizeof(srv_addr) );

	// Set Protocol Family for the address structure...
	srv_addr.sun_family = PF_UNIX;

	// For UNIX style, set the path for the socket.
	strcpy(srv_addr.sun_path,pathName);

	// Define the server socket...
	if ( (fd = socket (PF_UNIX, SOCK_STREAM, 0)) <  0 )
	{
		char buff [256];
		sprintf(buff, "%s: socket():", __FUNCTION__);
		perror(buff);
		return (-1);
	}

	// Attempt to connect to the server
	if ( connect (fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr) ) < 0 )
	{
		char buff[256];
		sprintf(buff, "%s: connect(): ", __FUNCTION__);
		perror(buff);
		return (-1);
	}

	// Return the now connected File Descriptor.
	return (fd);
}




int server_fd;

/*
A special flag is used to allow the "accept" system call to continue when a signal
is received.  Normally, the "accept" system call will abort with a failure return
code when a signal is received.  If the HandleExitedServerChilden" signal handler
was executed, then the failed "accept" call really did not fail, and we loop back
around to re-try the accept.
 */
#define CHILDINTERRUPTION 1
#define TERMINTERRUPTION 2
static int sigstate = 0;


/*
 * Signal handler.  The server will start child processes for each
 * connection established.  We need a signal handler for when these
 * processes exit.  The processes end, but stay in the process list
 * as "zombie" processes.
 *
 * iMac notes: I can comment out the SIGCHLD handler when I write this program for the
 * iMac LINUX.  It looks like SIGCHLD does not interrupt the "accept" call on the
 * iMac LINUX.  I did an experiment using "ps".  Noting how the "zombie" processes
 * continue, if this signal handler is NOT installed, and noting how this signal handler
 * cleans up the processes when it is installed, I determined that SIGCHLD is infact
 * sent to the Server program when a child server connection exits, but, the signal
 * does NOT destroy the "accept" call like it does on a sun system.
 */
static void HandleExitedServerChildren (int signum);
static void HandleExitedServerChildren (int signum)
{
	pid_t pid;
	int childStatus;

	printf("Cleaning up an exited server connection child process\n");
	// Wait for ANY child.
	// TODO
	// Wait for only the child we care about.
	while ( (pid=waitpid(-1, &childStatus, WNOHANG)) > 0 )
	{
		if (WIFEXITED(childStatus))
			printf("pid %d exited with \"exit()\" call, status: %d.\n", pid, WEXITSTATUS(childStatus));
		else
			// TODO
			// Logic to cover other cases of why a child process would signal out...
			printf("pid %d signalled out for some strange reason\n", pid);
	}

	// Set global flag so that an "accept" that was interrupted by this signal will
	// be started again.
	sigstate = CHILDINTERRUPTION;
}


void HandleExitServer (int signum)
{
	printf("Close down the tcpServer program\n");
	close(server_fd);
	sigstate = TERMINTERRUPTION;
}


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
int tcp_ServerStartup (int portNum, char * pathName, int (*serverProcessing)(int read_fd, int write_fd))
{
	// New and old actions for signal handling...
	struct sigaction new_SIGCHLD_action[1], old_SIGCHLD_action[1];
	struct sigaction new_SIGINT_action[1], old_SIGINT_action[1];

	// File descriptors for server socket, and for a client connection socket.
	int connection_fd;

	// Process ID for child process for connection
	int child_pid;

	// Set up the signal handling...
	new_SIGCHLD_action->sa_handler = HandleExitedServerChildren;
	sigemptyset (&new_SIGCHLD_action->sa_mask);
	new_SIGCHLD_action->sa_flags = 0;
	sigaction (SIGCHLD, new_SIGCHLD_action, old_SIGCHLD_action);

	new_SIGINT_action->sa_handler = HandleExitServer;
	sigemptyset (&new_SIGINT_action->sa_mask);
	new_SIGINT_action->sa_flags = 0;
	sigaction (SIGINT, new_SIGINT_action, old_SIGINT_action);

	// Open the tcp server socket...
	if (pathName)
		server_fd = tcp_PF_UNIX_srv_open(pathName);
	else if (portNum)
		server_fd = tcp_PF_INET_srv_open(0, portNum);
	else
	{
		// Usage problem.  We need either a port of a pathname.
		fprintf(stderr, "Neither port, nor pathname was provided.\n");
		exit (-1);
	}

	if ( server_fd == -1 )
	{
		// Something bad happened when setting up the TCP socket.
		fprintf(stderr, "Problem setting up server socket\n");
		exit (-1);
	}

	// Address of the client peer.
	// Each connection will contain a different client peer.
	struct sockaddr peerAddr;
	socklen_t peerAddrLength;
	peerAddrLength = sizeof(sockaddr_in);

	// The main server loop.  Wait for connections dealing with problems in the
	// accept call along the way...
	while (1)
	{
		/*
		 * This "accept loop" logic is here because the accept() system call
		 * may get interrupted for non-fatal reasons.
		 * If one of the servers child processes exits, this will "SIGCHLD"
		 * the accept(), causing it to return.  An exiting child process is
		 * no reason to stop the parent server process from accepting more
		 * connections.  So the loop is here so that we can continue when a
		 * non-fatal signal happens.
		 */
		int accept_one = 0;
		while (!accept_one)
		{
			if ( (connection_fd=accept(server_fd, &peerAddr, &peerAddrLength)) < 0 )
			{
				/*
				 * The accept() call returned w/o connecting for some reason, let's
				 * go through the possibilities:
				 */
				if (sigstate == CHILDINTERRUPTION)
				{
					/*
					 * This was a SIGCHLD interruption. This is no reason to
					 * stop accepting connections from further clients.
					 *
					 * See note above about running under iMac Darwin.
					 * Looks like the SIGCHLD does not destroy the "accept()" call
					 * as it does on other Linux systems.
					 */
					sigstate=0;
				}
				else if (sigstate == TERMINTERRUPTION)
				{
					/*
					 * This is a standard user termination (like CTRL-C), and
					 * so we stop the parent server with a clean exit condition.
					 */
					printf("%s: Standard Termination.\n", __FUNCTION__);
					if (pathName)
						unlink(pathName);
					exit(0);
				}
				else
				{
					// This is an honest failure in the accept() system call.
					// Give diagnostics feedback and exit the program.
					char buff[256];
					sprintf(buff, "accept()");
					perror(buff);
					exit(-1);
				}
			}
			else
			{
				// Good connection.  Exit from the inner "accept" loop.
				printf("%s: connection_fd: %d\n", __FUNCTION__, connection_fd);
				accept_one=1;
			}
		}

		// Here... Let's take a look at the peerAddr.
		// We should see PF_INET type socket addresses.
		printf("peerAddrLength:%d, peerAddr->sa_family:%d\n", peerAddrLength, peerAddr.sa_family);

		// We now have a connection fd. Now, we need to fork a child process with
		// this new socket fd.

		printf("About to fork a child for a server connection\n");
		if ( (child_pid=fork()) == -1 )
		{
			// Problem in the fork call...
			char buff[256];
			sprintf(buff, "fork():");
			perror(buff);
			exit (-1);
		}

		// Handle the different cases of a fork,
		// am I the child, or the parent process...
		switch (child_pid)
		{
		case 0 :
		{
			// If I get here, I am the child connection process.  I close the
			// server file descriptor, and start my processing of the new
			// communication connection path.
			close(server_fd);
			int serverProcessingStatus = serverProcessing(connection_fd, connection_fd);
			// serverProcessing functions end based on the rules of the protocol
			// of data exchange.  Make sure the status of the process exit is
			// preserved so that the parent server process can reflect this.
			exit(serverProcessingStatus);
			break;
		}
		default :
		{
			// If I get here, I am the parent server process.  Continue around and
			// enter another "accept" loop.
			break;
		}
		}

		printf("Looping back to accept another connection\n");

	}

	// Main loop should not exit, but we need to return and int.
	return (0);
}
