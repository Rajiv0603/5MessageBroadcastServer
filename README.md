# 5MessageBroadcastServer
Simple Message Broadcast Server System using Internet Domain Socket Programming in C

------------------------------------------------------------------------------------

In this assignment you will use Internet-domain stream sockets to implement a simple message broadcast server system. The broadcast server should implement the following functions:

1) Register a system. The name of a machine (e.g. ‘camelot’) which will be using the broadcast server is registered with the server. In order to use the server, the machine must first register with the server. Attempts to sendmessages by unregistered clients will be rejected. The server maintains a textfile containing the registered clients that it reads at start up. The list of clients is maintained between runs of the server. An attempt to register a client which is already registered returns a code indicating that the client is already registered (do not register a client twice). There is only one client process per machine.

2) Deregister a system. A client (e.g. ‘camelot’) deregisters when it will no longer be using the server. The server removes the client from its list of registered users.

3) Broadcast a message. A client sends a message to all registered users. If the source machine is not registered, the server sends an error code to the client. Otherwise, the server broadcasts the message to all registered machines (except the machine which is the source). If a communication to a client fails (the client is not running), the client is deregistered.

4)QuitThe server sends a quit message to all registered clients (which shut down) and then shuts down itself.

Broadcast messages contain several fields: sender (e.g. ‘joe@merlin’ – the server must verify that the message sender field matches the actual sender, otherwise an error message is generated); subject (up to 80 characters); message body (variable length, max. 512 characters).

You should write a client program which has a menu allowing the user to register the system, deregister the system, compose/send a message, or send a quit message (4 menu choices). The compose/send command allows the user to fill in each of the message fields, verifies the format of each field, and then sends the message. A message indicating success or failure of the command is printed.  All communication between clients and server is via Internet-domain stream sockets.

Note that the server does not check for validity of individual users, only validity of the source  machine (whether they are registered). Any number of clients should be able to run simultaneously and your server runs until the quit message is received. The name of the machine the server is running on is passed in on the command line to the client program. The client program keeps running (displaying the menu) until a quit message is received from the server.
