#ifndef _TCPUTILS_H_
#define _TCPUTILS_H_

#include <sys/socket.h>		// For sockaddr

/*
* tcp_PF_INET_cl_open - Used by a client application to setup a connection to a
 * PF_INET server.  The returned file descriptor will be used for to send data
 * between client and server.
 */
int tcp_PF_INET_cl_open(char * host, char * service, int port);


class tcp_Base_Client
{
private:
	int tcp_cl_open();
protected:
	int client_fd;
	char socketAddr[256]; // 256 bytes for any possible socket address.
public:
	tcp_Base_Client();
	virtual ~tcp_Base_Client();
	virtual int clientProcessing(int read_fd, int write_fd);
	int startup();
};


class tcp_UNIX_Client : public tcp_Base_Client
{
private:
	int tcp_PF_UNIX_cl_open (char * pathname);
public:
	tcp_UNIX_Client(char * pathname);
	virtual ~tcp_UNIX_Client();
};

/*
 * Because of signal handling, there can only be one object of this class in any process.
 * The class sets up signal handlers for process exit (like CTRL C) and for child process
 * exiting (like when a server child finishes it's work).
 * This restriction does not preclude many TCP connections.  There may be a great many
 * server processes that are communicating with a great many clients.  But this class
 * is the abstraction of the single server that spawns all the individual server side
 * processes.
 */
/*
 * NOTE: It's important to declare and define a virtual destructor to the class.
 * With out this, a "vtable" is not created, and we get link errors when I try
 * to build a program that references this library.
 * "Undefined symbols: vtable"
 * I get these errors even if my main program does not use the "tcp_Server" class.
 */
class tcp_Base_Server
{
private:
	static void handleExitServer (int signum);
	static void handleExitedServerChildren (int signum);
	static void setupHandlers();
	static int sigstate;
protected:
	int tcp_bind_setup_fd (int fd, sockaddr * pGenericSocketAddress, socklen_t addrlen);
	static int server_fd;
public:
	tcp_Base_Server();
	virtual ~tcp_Base_Server();
	virtual int serverProcessing(int read_fd, int write_fd);
	int startup();
};

class tcp_INET_Port_Server : public tcp_Base_Server
{
private:
	int tcp_port_srv_open (int port);
public:
	tcp_INET_Port_Server(int p);
};

class tcp_INET_Service_Server : public tcp_Base_Server
{
private:
	int tcp_service_srv_open (char * service);
public:
	tcp_INET_Service_Server(char * s);
};

class tcp_UNIX_Server : public tcp_Base_Server
{
private:
	int tcp_path_srv_open (char * pathname);
public:
	tcp_UNIX_Server(char * pathname);
	virtual ~tcp_UNIX_Server();
};

#endif // _TCPUTILS_H_
