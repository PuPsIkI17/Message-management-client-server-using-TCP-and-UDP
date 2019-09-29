# Message-management-client-server-using-TCP-and-UDP

There are 3 components.
<\br>• Sever (unique) was the first component implemented and it makes the connection between the clients
<\br>from the platform, for the purpose of publishing and subscribing to messages.
<\br>• TCP clients are the second component. A TCP client connects to the server, can receive (at any time)
<\br>from the keyboard (interaction with the human user) commands of the subscribe and unsubscribe type and 
<\br>displayed on screen messages received from the server.
<\br>• UDP client publish, by sending to the server, messages in the proposed platform using a predefined 
<\br>protocol. For each TCP client receive from the server those messages from UDP clients, which refer 
<\br>to the topics to which they are subscribed. The architecture also includes a component of SF
<\br>(store & foreward) regarding messages sent when TCP clients are disconnected.

<\br>  The server have the role of a broker (intermediary component) in the message management platform. It
<\br><\br>opens 2 sockets (one TCP and one UDP) on a port received as a parameter and wait for connections/datagrams 
<\br>on all locally available IP addresses. Starting the server will be done using the command:
<\br>./server <PORT_DORIT>

<\br>  To monitor the server activity, the display is required (at the output standard) of the events of connection,
<\br>respectively disconnection of the client, TCP. This is done using form message:
<\br>New client (CLIENT_ID) connected from IP: PORT.
<\br>respectively
<\br>Client (CLIENT_ID) disconnected.
<\br>  The server accept, from the keyboard, only the exit command.

<\br>Funtionality
<\br>  The initialization of the application is given by starting the server, to which later a number can 
<\br>be connected client variable, and TCP / UDP. The server allow the connection / disconnection of (new)
<\br>clients to any moment.
<\br>  Each client is identified by the client_id with which it was started. At one point, there are no 2 clients 
<\br>connected with the same ID.
<\br>  The client ID has 10 ASCII characters.
<\br>  The server keep in mind the topics to which each of the clients is subscribed.
<\br>  The subscription order has a SF (store & forward) parameter. If it is set to 1, it means
<\br>that as a client, you want to make sure that you do not miss any message sent on that topic. Thus, if a TCP client
<\br>he disconnects, then returns, he will have to receive from the server all the messages that were not sent to him
<\br>already, even though they were sent / published during the time it was disconnected, according to a
<\br>store & forward policies. This applies only to subscriptions made with SF = 1, active before
<\br>by posting the message. For SF = 0, messages sent when the client is offline will be lost, but the client
<\br>will remain subscribed to that topic.

<\br>Commands
<\br>TCP clients can receive from the keyboard one of the following commands:
<\br>• subscribe - announcement, to the server that a client is interested in a particular topic; the command has the following
<\br>format: subscribe topic SF where:
<\br>- Topic represents the topic to which the client is to subscribe;
<\br>- SF may have the value 0 or 1 (explained in Section 4).
<\br>• unsubscribe - announcement, to the server that a client is no longer interested in a particular topic; the command has
<\br><\br>the following format: unsubcribe topic where topic represents the topic from which the client is following
<\br>to subscribe;
<\br>• exit - the command will be used to close the client.
