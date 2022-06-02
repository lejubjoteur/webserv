# Webserv

## Step 1: Implementing TCP

### 1. Create a socket

```
int socket(int domain, int type, int protocol);
```

With POSIX variable:

- _AF_INET_ for IPV4 addresses as **domain**
- _SOCK_STREAM_ for TCP as communication **type**
- protocol should be 0 for Internet Protocol

### 2. Identify the socket

Assigning a transport address to the socket (a port number in IP networking). In sockets, this operation is called binding an address and the bind system call is used for this.

```
int bind(int socket, const struct sockaddr *address, socklen_t address_len);
```

sockaddr_in needs to be initialized:

```
memset((char *)&address, 0, sizeof(address));
address.sin_family = AF_INET;
address.sin_addr.s_addr = htonl(INADDR_ANY);
address.sin_port = htons(PORT);
```

### 3. On the server wait for an incoming connection

The listen system call tells a socket that it should be capable of accepting incoming connections.

```
int listen(int socket, int backlog);
```

Backlog being the maximum numbers of accepted connections.
The accept system call grabs the first connection request on the queue of pending connections (set up in listen) and creates a new socket for that connection.

```
The accept system call grabs the first connection request on the queue of pending connections (set up in listen) and creates a new socket for that connection.
```

### 4. Send and receive messages

### 5. Close the socket

---

### Ressources:

[HTTP Server: Everything you need to know to Build a simple HTTP server from scratch](https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa)
