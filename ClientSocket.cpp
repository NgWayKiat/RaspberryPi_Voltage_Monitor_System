#include "ClientSocket.h"


void connect()
{
    int sockfd;
    int n;
    char buffer[1024] = {0};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0)
    {
        cout << "Open Socket Error." << endl;
    }

    sockaddr_in client_host_address;
    client_host_address.sin_family = AF_INET;
    client_host_address.sin_addr.s_addr = inet_addr("10.60.241.105");
    client_host_address.sin_port = htons(6000);

    if (connect(sockfd,(struct sockaddr *) &client_host_address,sizeof(client_host_address)) == -1)
    {
        cout << "Error on connection" << endl;
        return;
    }

    const char* message = "Hello from client!";
    send(sockfd, message, strlen(message), 0);
    cout << "Message sent" << endl;

    recv(sockfd, buffer, 1024, 0);
    cout << "Received from server: " << buffer << endl;

    // 5. Close the socket
    close(sockfd);
}