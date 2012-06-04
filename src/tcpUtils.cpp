/*
 * tcpUtils.cpp -
 * C++ Implementation of tcp connection utilities.
 * This compilation unit contains code for some of the tcp_Server class methods.
 *
 * Ken Lamarche
 * Header comment created 14 May, 2012, but this code is a month old.
 *
 */
 
#include "tcpUtils.h"

#include <iostream>

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

using namespace std;


// tcp_Base_Client methods...

tcp_Base_Client::tcp_Base_Client()
{
	cout << "tcp_Base_Client()" << endl;
};


tcp_Base_Client::~tcp_Base_Client()
{
	cout << "tcp_Base_Client::~tcp_Base_Client()" << endl;
	close (client_fd);
}


int tcp_Base_Client::startup()
{
	
	// Do Client Processing:
	clientProcessing(client_fd, client_fd);
	
	return (0);
}

/*
 * Base version of the client processing.
 * Derived classes could fall back on this logic, and it should work...
 */
int tcp_Base_Client::clientProcessing(int read_fd, int write_fd)
{
	int charCount = 30;
	char C;
	int status;
	char ErrBuff[128];
	C = 'A';
	while (charCount)
	{
		if ( (status=write(write_fd, &C, 1)) < 0 )
		{
			// Error condition on read.
			sprintf(ErrBuff, "Problem reading \"%s\".", "Generic Server Processing");
			perror (ErrBuff);
			return status;
		}
		else if (status == 0)
		{
			// The peer disconnected the connection
			cout << "Peer closed connection" << endl;
			return 0;
		}
		else
		{
			cout << C;
		}
		charCount--;
		C++;
	}
	cout << endl;
	cout << "clientProcessing Exiting" << endl;
	return 8;
}



/*
 * tcp_INET_Client methods...
 */

tcp_INET_Client::tcp_INET_Client(int port)
{
	cout << "tcp_INET_Client(port)" << endl;
	tcp_cl_open(port, (char*)"localhost");
}

tcp_INET_Client::tcp_INET_Client(char * service)
{
	cout << "tcp_INET_Client(service)" << endl;
	tcp_cl_open(service, (char*)"localhost");
}

tcp_INET_Client::tcp_INET_Client(int port, char * hostname)
{
	cout << "tcp_INET_Client(port, hostname)" << endl;
	tcp_cl_open(port, hostname);
}

tcp_INET_Client::tcp_INET_Client(char * service, char * hostname)
{
	cout << "tcp_INET_Client(service, hostname)" << endl;
	tcp_cl_open(service, hostname);
}

tcp_INET_Client::~tcp_INET_Client()
{
	cout << "tcp_INET_Client Destructor" << endl;
}


int tcp_INET_Client::tcp_cl_open (int port, char * host)
{
	unsigned long inaddr;
	struct hostent * hp;
	struct sockaddr_in srv_addr;   // A 16 byte overlay on the general sockaddr struct.
	
	// Clear the structure...
	memset ( (char*)&srv_addr, 0, sizeof(srv_addr) );
	
	// Set Protocol Family for the address structure...
	srv_addr.sin_family = PF_INET;
	
	// Take the port from the parameter
	srv_addr.sin_port=htons(port);
		
	// Attempt to interpret the "host" parameter:
	if ( ! host )
	{
		fprintf(stderr, "%s: No host specified\n", __FUNCTION__);
		return (-1);
	}
	
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
	
	// Define the server socket...
	if ( (client_fd = socket (AF_INET, SOCK_STREAM, 0)) <  0 )
	{
		char buff [256];
		sprintf(buff, "%s: socket():", __FUNCTION__);
		perror(buff);
		return (-1);
	}
	
	// Attempt to connect to the server
	if ( connect (client_fd, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in) ) < 0 )
	{
		char buff[256];
		sprintf(buff, "%s: connect(): ", __FUNCTION__);
		perror(buff);
		return (-1);
	}
	
	return (0);
}


int tcp_INET_Client::tcp_cl_open (char * service, char * host)
{
	unsigned long inaddr;
	struct servent * sp;
	struct hostent * hp;
	struct sockaddr_in srv_addr;   // A 16 byte overlay on the general sockaddr struct.
	
	// Clear the structure...
	memset ( (char*)&srv_addr, 0, sizeof(srv_addr) );

	// A hostname character string must be supplied.
	if ( ! host )
	{
		fprintf(stderr, "%s: No host specified\n", __FUNCTION__);
		return (-1);
	}
	
	// A service character string must be supplied.
	if ( ! service )
	{
		fprintf(stderr, "%s: No service specified\n", __FUNCTION__);
		return (-1);
	}

	// Set Protocol Family for the address structure...
	srv_addr.sin_family = PF_INET;
	
	// Find the service and use this to fill in the port number:
	if ( (sp=getservbyname(service, "tcp")) == NULL )
	{
		// Error in attempting to find the named service.
		fprintf(stderr, "%s: unknown service - %s\n", __FUNCTION__, service);
		return (-1);
	}
	srv_addr.sin_port = sp->s_port;
	
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
	
	// Define the server socket...
	if ( (client_fd = socket (AF_INET, SOCK_STREAM, 0)) <  0 )
	{
		char buff [256];
		sprintf(buff, "%s: socket():", __FUNCTION__);
		perror(buff);
		return (-1);
	}
	
	// Attempt to connect to the server
	if ( connect (client_fd, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr_in) ) < 0 )
	{
		char buff[256];
		sprintf(buff, "%s: connect(): ", __FUNCTION__);
		perror(buff);
		return (-1);
	}
	
	return (0);
}



/*
 * tcp_UNIX_Client methods...
 */


tcp_UNIX_Client::tcp_UNIX_Client(char * pathname)
{
	cout << "tcp_UNIX_Client" << endl;
	tcp_PF_UNIX_cl_open(pathname);
}

tcp_UNIX_Client::~tcp_UNIX_Client()
{
	cout << "tcp_UNIX_Client Destructor" << endl;
}


int tcp_UNIX_Client::tcp_PF_UNIX_cl_open (char * pathname)
{
	struct sockaddr_un * pAddr;   // pathname overlay on the general sockaddr struct.
	
	pAddr = (sockaddr_un *)socketAddr;
	// Clear the structure...
	memset (pAddr, 0, sizeof(struct sockaddr_un));
	
	// Set Protocol Family for the address structure...
	pAddr->sun_family = PF_UNIX;
	
	// For UNIX style, set the path for the socket.
	strcpy(pAddr->sun_path,pathname);
	
	// Define the server socket...
	if ( (client_fd = socket (PF_UNIX, SOCK_STREAM, 0)) <  0 )
	{
		char buff [256];
		sprintf(buff, "%s: socket():", __FUNCTION__);
		perror(buff);
		return (-1);
	}
	
	// Attempt to connect to the server.
	// Would be really good to put this in the Base class, but "connect()" must take
	// a size argument that is consistent with the flavor of socket address struct
	// that is being used.
	if ( connect (client_fd, (struct sockaddr *)socketAddr, sizeof(struct sockaddr_un) ) < 0 )
	{
		char buff[256];
		sprintf(buff, "%s: connect(): ", __FUNCTION__);
		perror(buff);
		return (-1);
	}
}

/*
A special flag is used to allow the "accept" system call to continue when a signal
is received.  Normally, the "accept" system call will abort with a failure return
code when a signal is received.  If the HandleExitedServerChilden" signal handler
was executed, then the failed "accept" call really did not fail, and we loop back
around to re-try the accept.
 */
#define CHILDINTERRUPTION 1
#define TERMINTERRUPTION 2


/*
 * How to manage that only a single tcp_Base_Server can exist in a given process?
 * Because signals are sent to a process, only one such class can be declared
 * in a process.
 */



int tcp_Base_Server::tcp_bind_setup_fd (int fd, sockaddr * pGenericSocketAddress, socklen_t addrlen)
{
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
	// The actual listening only happens when the "accept" is called.
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

///*
// * Base version of tcp_srv_open.
// * Derived classes will have to have their own specialized open logic...
// */
//int tcp_Base_Server::tcp_srv_open ()
//{
//	int fd;
//	cout << "tcp_Base_Server::tcp_srv_open" << endl;
//	return (fd=0);
//}

/*
 * Base version of the server processing.
 * Derived classes could fall back on this logic, and it should work...
 */
int tcp_Base_Server::serverProcessing(int read_fd, int write_fd)
{
	int charCount = 30;
	char C;
	int status;
	char ErrBuff[128];
	while (charCount)
	{
		  if ( (status=read(read_fd, &C, 1)) < 0 )
			{
			  // Error condition on read.
			  sprintf(ErrBuff, "Problem reading \"%s\".", "Generic Server Processing");
			  perror (ErrBuff);
			  return status;
			}
		  else if (status == 0)
			{
			  // The peer disconnected the connection
			  cout << "Peer closed connection" << endl;
			  return 0;
			}
		  else
			{
			  cout << C;
			}
		  charCount--;
	}
	cout << endl;
	cout << "serverProcessing Exiting" << endl;
	return 8;
}


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
void tcp_Base_Server::handleExitedServerChildren (int signum)
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
			cout << "pid " << pid << " exited with \"exit()\" call. Status: "
			<< WEXITSTATUS(childStatus) << endl;
		else
			// TODO
			// Logic to cover other cases of why a child process would signal out...
			printf("pid %d signalled out for some strange reason\n", pid);
	}

	// Set global flag so that an "accept" that was interrupted by this signal will
	// be started again.
	sigstate = CHILDINTERRUPTION;
}


/*
 * This is a "static" member function.
 * Only one exists, because there is only one signal handler per process.
 * What this means is that there really can only be one tcp_Server object
 * per process.
 * If a program tries to declare more than one, then the same connection
 * accept file descriptor would be used for both.  Yuck.
 */
void tcp_Base_Server::handleExitServer (int signum)
{
	cout << "Close down the tcp server program" << endl;
	sigstate = TERMINTERRUPTION;
}


void tcp_Base_Server::setupHandlers()
{
	// New and old actions for signal handling...
	struct sigaction new_SIGCHLD_action[1], old_SIGCHLD_action[1];
	struct sigaction new_SIGINT_action[1], old_SIGINT_action[1];

	// Set up the signal handling...
	new_SIGCHLD_action->sa_handler = handleExitedServerChildren;
	sigemptyset (&new_SIGCHLD_action->sa_mask);
	new_SIGCHLD_action->sa_flags = 0;
	sigaction (SIGCHLD, new_SIGCHLD_action, old_SIGCHLD_action);

	new_SIGINT_action->sa_handler = handleExitServer;
	sigemptyset (&new_SIGINT_action->sa_mask);
	new_SIGINT_action->sa_flags = 0;
	sigaction (SIGINT, new_SIGINT_action, old_SIGINT_action);
}


/*
 * In determining the logic for the base constructor, I'm trying to find
 * the common logic needed for all flavors of server.
 * 1. The signal handlers need to be set up.
 * 2. Logic is needed to setup a socket file descriptor.
 * 3. A "listen" limit is needed on the socket file descriptor.
 *
 * Step 1 & 3 are performed in a common way, so they should be delegated
 * to the base class.
 *
 * But the logic of step 2 is specific to the flavor of server (i.e. INET
 * servers will do step 2 different that UNIX servers...)
 *
 * Further, the parameters needed for step 2 are unique for each server
 * flavor.
 *
 * In order to establish a virtual function for step 2, I need a function
 * with NO parameters.  But when the virtual function is called, perhaps
 * the specialized parameters will have been stored.
 */
tcp_Base_Server::tcp_Base_Server()
{
	setupHandlers();

	cout << "tcp_Base_Server()" << endl;

	/*
	 * So, This code does not work.
	 * I cannot force the base constructor to call a derived class' version of a
	 * virtual function.  This may be because the derive class constructor has not
	 * yet been called, and so C++ really doesn't even know about the derived class
	 * at this point.
	 *
	 * Note that if there's a base version of tcp_srv_open(), it's the base version
	 * that gets called here, not the derived version.
	 */
//	server_fd = tcp_srv_open ();
//	cout << "server_fd" << (int)server_fd << endl;
//	if ( server_fd == -1 )
//	{
//		// Something bad happened when setting up the TCP socket.
//		cout << "Problem setting up server socket" << endl;
//		exit (-1);
//	}
}


tcp_Base_Server::~tcp_Base_Server()
{
	cout << "tcp_Base_Server::~tcp_Base_Server()" << endl;
	close (server_fd);
}


/*
 * Main loop for tcp server.
 * Accept a client connection, spawn a process that defines logic for what
 * data is passed on that connection.
 */
int tcp_Base_Server::startup(void)
{

	// File descriptors for server socket, and for a client connection socket.
	int connection_fd;

	// Process ID for child process for connection
	int child_pid;

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
					cout << __FUNCTION__ << " Standard Termination" << endl;
					return 0;
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
				cout << __FUNCTION__ << ": connection_fd: " << connection_fd << endl;
				accept_one=1;
			}
		}

		// We now have a connection fd. Now, we need to fork a child process with
		// this new socket fd.

		cout << "About to fork a child for a server connection" << endl;
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

		cout << "Looping back to accept another connection" << endl;

	}

	// Main loop should not exit, but we need to return and int.
	return (0);

}


// Allocate the class static variables
int tcp_Base_Server::server_fd = 0;
int tcp_Base_Server::sigstate = 0;



/*
 * Derived Class Method Implementations
 */


/*
 * Establish sockaddr for port / service style socket.  Setup File Descriptor using
 * base "bind" method.
 */
int tcp_INET_Port_Server::tcp_port_srv_open (int port)
{
	int fd;
	struct sockaddr_in srv_addr;

	// Initialize the address structure...
	memset(&srv_addr, 0, sizeof(srv_addr));

	// Set the Family in the address structure...
	srv_addr.sin_family = PF_INET;

	// Set the port in the address structure...
	srv_addr.sin_port=htons(port);

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

	// The socket must be bound to a file descriptor, then set to "listen".
	// If all goes well, the fd is returned.  Else -1 is returned.
	return (tcp_bind_setup_fd(fd, (sockaddr*)(&srv_addr), sizeof(srv_addr)));
}


tcp_INET_Port_Server::tcp_INET_Port_Server(int p)
{
	cout << "tcp_INET_Port_Server(port)" << endl;
	server_fd = tcp_port_srv_open (p);
	cout << "server_fd" << (int)server_fd << endl;
	if ( server_fd == -1 )
	{
		// Something bad happened when setting up the TCP socket.
		cout << "Problem setting up server socket" << endl;
		exit (-1);
	}
}


/*
 * Establish sockaddr for port / service style socket.  Setup File Descriptor using
 * base class "bind" method.
 */
int tcp_INET_Service_Server::tcp_service_srv_open (char * service)
{
	int fd;
	struct sockaddr_in srv_addr;

	// Initialize the address structure...
	memset(&srv_addr, 0, sizeof(srv_addr));

	// Set the Family in the address structure...
	srv_addr.sin_family = PF_INET;

	// Set the port in the address structure...
		struct servent * sp;
		if ( (sp=getservbyname(service, "tcp")) == NULL )
		{
			// Error in attempting to find the named service.
			fprintf(stderr, "%s: unknown service - %s\n", __FUNCTION__, service);
			return (-1);
		}
		srv_addr.sin_port = sp->s_port;

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

	// The socket must be bound to a file descriptor, then set to "listen".
	// If all goes well, the fd is returned.  Else -1 is returned.
	return (tcp_bind_setup_fd(fd, (sockaddr*)(&srv_addr), sizeof(srv_addr)));
}


tcp_INET_Service_Server::tcp_INET_Service_Server(char * s)
{
	cout << "tcp_INET_Service_Server(service)" << endl;
	server_fd = tcp_service_srv_open (s);
	cout << "server_fd" << (int)server_fd << endl;
	if ( server_fd == -1 )
	{
		// Something bad happened when setting up the TCP socket.
		cout << "Problem setting up server socket" << endl;
		exit (-1);
	}
}


/*
 * Establish sockaddr for UNIX "filename" style socket.  Setup File Descriptor using
 * base class "bind" method.
 */
int tcp_UNIX_Server::tcp_path_srv_open (char * pathname)
{
	int fd;
	struct sockaddr_un srv_addr;

	// Initialize the address structure...
	memset(&srv_addr, 0, sizeof(srv_addr));

	// Set the Family in the address structure...
	srv_addr.sun_family = PF_UNIX;

	// For UNIX style, set the path for the socket.
	strcpy(srv_addr.sun_path,pathname);
	cout << "srv_addr.sun_path: " << srv_addr.sun_path << endl;

	// Define the server socket...
	if ( (fd = socket (PF_UNIX, SOCK_STREAM, 0)) <  0 )
	{
		char buff [256];
		sprintf(buff, "%s: socket():", __FUNCTION__);
		perror(buff);
		return (-1);
	}
	// The socket must be bound to a file descriptor, then set to "listen".
	// If all goes well, the fd is returned.  Else -1 is returned.
	return (tcp_bind_setup_fd(fd, (sockaddr*)(&srv_addr), sizeof(srv_addr)));
}


tcp_UNIX_Server::tcp_UNIX_Server(char * pathname)
{
	cout << "tcp_UNIX_Server(pathname)" << endl;
	server_fd = tcp_path_srv_open (pathname);
	cout << "server_fd" << (int)server_fd << endl;
	if ( server_fd == -1 )
	{
		// Something bad happened when setting up the TCP socket.
		cout << "Problem setting up server socket" << endl;
		exit (-1);
	}
}


/*
 * For a AF_UNIX server, the destructor should remove the socket file node
 * from the file system.
 */
tcp_UNIX_Server::~tcp_UNIX_Server()
{
	cout << "~tcp_UNIX_Server()" << endl;
	int status;
	// Enough space for a good long pathname...
	unsigned char buffer[512];
	struct sockaddr_un * pSockAddrUn = (struct sockaddr_un *)buffer;
	socklen_t sockAddrLength;
	sockAddrLength = 512;
	getsockname(server_fd, (struct sockaddr *)pSockAddrUn, &sockAddrLength);
	cout << "sockAddrLength: " << (int)sockAddrLength <<
			"  pSockAddrUn->sun_path: " << pSockAddrUn->sun_path << endl;
	if (-1 == (status=unlink(pSockAddrUn->sun_path)))
		perror("Trouble in unlink");
}

