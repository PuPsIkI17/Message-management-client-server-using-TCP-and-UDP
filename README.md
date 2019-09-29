# Message-management-client-server-using-TCP-and-UDP

There are 3 components.
</br>&nbsp;&nbsp;&nbsp;• Sever (unique) was the first component implemented and it makes the connection between the clients
</br>&nbsp;&nbsp;&nbsp;from the platform, for the purpose of publishing and subscribing to messages.
</br>&nbsp;&nbsp;&nbsp;• TCP clients are the second component. A TCP client connects to the server, can receive (at any time)
</br>&nbsp;&nbsp;&nbsp;from the keyboard (interaction with the human user) commands of the subscribe and unsubscribe type and 
</br>&nbsp;&nbsp;&nbsp;displayed on screen messages received from the server.
</br>&nbsp;&nbsp;&nbsp;• UDP client publish, by sending to the server, messages in the proposed platform using a predefined 
</br>&nbsp;&nbsp;&nbsp;protocol. For each TCP client receive from the server those messages from UDP clients, which refer 
</br>&nbsp;&nbsp;&nbsp;to the topics to which they are subscribed. The architecture also includes a component of SF
</br>&nbsp;&nbsp;&nbsp;(store & foreward) regarding messages sent when TCP clients are disconnected.

</br>&nbsp;&nbsp;&nbsp;  The server have the role of a broker (intermediary component) in the message management platform. It
</br>&nbsp;&nbsp;&nbsp;opens 2 sockets (one TCP and one UDP) on a port received as a parameter and wait for connections/datagrams 
</br>&nbsp;&nbsp;&nbsp;on all locally available IP addresses. Starting the server will be done using the command:
</br>&nbsp;&nbsp;&nbsp;./server <PORT_DORIT>

</br>&nbsp;&nbsp;&nbsp;  To monitor the server activity, the display is required (at the output standard) of the events of connection,
</br>&nbsp;&nbsp;&nbsp;respectively disconnection of the client, TCP. This is done using form message:
</br>&nbsp;&nbsp;&nbsp;New client (CLIENT_ID) connected from IP: PORT.
</br>&nbsp;&nbsp;&nbsp;respectively
</br>&nbsp;&nbsp;&nbsp;Client (CLIENT_ID) disconnected.
</br>&nbsp;&nbsp;&nbsp;  The server accept, from the keyboard, only the exit command.

</br>&nbsp;&nbsp;&nbsp;Funtionality
</br>&nbsp;&nbsp;&nbsp;  The initialization of the application is given by starting the server, to which later a number can 
</br>&nbsp;&nbsp;&nbsp;be connected client variable, and TCP / UDP. The server allow the connection / disconnection of (new)
</br>&nbsp;&nbsp;&nbsp;clients to any moment.
</br>&nbsp;&nbsp;&nbsp;  Each client is identified by the client_id with which it was started. At one point, there are no 2 clients 
</br>&nbsp;&nbsp;&nbsp;connected with the same ID.
</br>&nbsp;&nbsp;&nbsp;  The client ID has 10 ASCII characters.
</br>&nbsp;&nbsp;&nbsp;  The server keep in mind the topics to which each of the clients is subscribed.
</br>&nbsp;&nbsp;&nbsp;  The subscription order has a SF (store & forward) parameter. If it is set to 1, it means
</br>&nbsp;&nbsp;&nbsp;that as a client, you want to make sure that you do not miss any message sent on that topic. Thus, if a TCP client
</br>&nbsp;&nbsp;&nbsp;he disconnects, then returns, he will have to receive from the server all the messages that were not sent to him
</br>&nbsp;&nbsp;&nbsp;already, even though they were sent / published during the time it was disconnected, according to a
</br>&nbsp;&nbsp;&nbsp;store & forward policies. This applies only to subscriptions made with SF = 1, active before
</br>&nbsp;&nbsp;&nbsp;by posting the message. For SF = 0, messages sent when the client is offline will be lost, but the client
</br>&nbsp;&nbsp;&nbsp;will remain subscribed to that topic.

</br>&nbsp;&nbsp;&nbsp;Commands
</br>&nbsp;&nbsp;&nbsp;TCP clients can receive from the keyboard one of the following commands:
</br>&nbsp;&nbsp;&nbsp;• subscribe - announcement, to the server that a client is interested in a particular topic; the command has the following
</br>&nbsp;&nbsp;&nbsp;format: subscribe topic SF where:
</br>&nbsp;&nbsp;&nbsp;- Topic represents the topic to which the client is to subscribe;
</br>&nbsp;&nbsp;&nbsp;- SF may have the value 0 or 1 (explained in Section 4).
</br>&nbsp;&nbsp;&nbsp;• unsubscribe - announcement, to the server that a client is no longer interested in a particular topic; the command has
</br>&nbsp;&nbsp;&nbsp;the following format: unsubcribe topic where topic represents the topic from which the client is following
</br>&nbsp;&nbsp;&nbsp;to subscribe;
</br>&nbsp;&nbsp;&nbsp;• exit - the command will be used to close the client.
