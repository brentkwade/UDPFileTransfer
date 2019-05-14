#include <stdio.h> // For printf()
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> // sockaddr_in, inet_addr
#include <netinet/in.h>
#include <sys/socket.h> // socket(), connect(), send(), recv()
#include "udp.h"        // Header File With Definitions

/*
Server:
    - Server starts and waits for incoming connection request
    - Server receives fileName
    - If file not present, server alerts client by sending back errorMsg
    - If file is present, server reads file into bugger and sends buffer content to client
      until file-end is reached (marked by EOF)
    - Create socket with socket()
    - Bind socket to address using bind()
    - Listen for connections with listen()
    - Accept connection with accept()
    - Send & receive data using read() and write() system calls
*/

// File Writing
void writeFile(int sockfd, FILE *fp);

ssize_t total = 0;

int main(int argc, char *argv[])
{

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Socket Error Handling
    if (sockfd < 0)
    {
        perror("Error: Trouble Assigning Socket");
        return 0;
    }

    // Local Address Structure
    struct sockaddr_in clientAddress;
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));  // Zero Out Structure
    serverAddress.sin_family = AF_INET;                // Internet Address Family
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Incoming Interface
    serverAddress.sin_port = htons(SERVERPORT);        // Local Port

    // Binding To Local Address Error Handling
    //if (
    bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    //perror("Error: Binding Not Working");
    //exit(1);
    //}

    socklen_t addrlen = sizeof(clientAddress);

    char fileName[BUFFSIZE] = {0};

    // Receive File Error Handling
    if (recvfrom(sockfd, fileName, BUFFSIZE, 0, (struct sockaddr *)&clientAddress, &addrlen) < 0)
    {
        perror("Error: Can't Retrieve fileName");
        exit(1);
    }

    // Open File Handling
    FILE *fp = fopen(fileName, "wb");

    // Open File Error Handling
    if (fp == NULL)
    {
        perror("Error: Unable To Open Designated File");
        exit(1);
    }

    char addr[INET_ADDRSTRLEN];

    printf("Address %s Is Sending File '%s'\nBeginning Retrieval...\n", inet_ntop(AF_INET, &clientAddress.sin_addr, addr, INET_ADDRSTRLEN), fileName);

    writeFile(sockfd, fp);

    printf("File '%s' Received\nFile '%s' Byte Size: %ld\nTerminating Process...\n", fileName, fileName, total);

    fclose(fp);
    close(sockfd);

    return 0;
}

// Write File
void writeFile(int sockfd, FILE *fp)
{

    ssize_t n;

    char buff[MAX_LINE] = {0};

    struct sockaddr_in clientAddress;
    socklen_t addrlen = sizeof(clientAddress);

    while ((n = recvfrom(sockfd, buff, MAX_LINE, 0, (struct sockaddr *)&clientAddress, &addrlen)) > 0)
    {

        total += n;

        // Receiving File Error Handling
        if (n < 0)
        {
            perror("Error: Trouble Receiving File");
            exit(1);
        }

        // Writing File Error Handling
        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Error: Trouble Writing File");
            exit(1);
        }

        memset(buff, 0, MAX_LINE);
    }
}