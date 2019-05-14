#include <stdio.h> // For printf()
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h> // sockaddr_in, inet_addr
#include <netinet/in.h>
#include <sys/socket.h> // socket(), connect(), send(), recv()
#include "udp.h"        // Header File With Definitions

/*
Client:
    - Client established TCP connection with the server
    - Client sends fileName to the server over TCP connection
    - Once transmission finishes, client verifies the received file is correct
    - Client chooses to download a different file or close TCP connection & exit
    - Create socket with socket()
    - Connect socket to address of server using connect() system call
    - Send & receive data using read() and write() system calls
*/

//Send File
void sendFile(FILE *fp, int sockfd);

ssize_t total = 0;

int main(int argc, char *argv[])
{

    //Test For Correct Number Of Arguments
    if (argc != 3)
    {
        perror("Error: sendFile - filepath <IPaddress>");
        exit(1);
    }
    /*
    if((sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) <0) {
        perror("Socket Assignment Failed.");
    }
*/

    int sockfd = socket(PF_INET, SOCK_DGRAM, 0);

    //Socket Error Handling
    if (sockfd < 0)
    {
        perror("Error: Trouble Assigning Socket");
        return 0;
    }

    //Local Address Structure
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress)); // Zero Out Structure
    serverAddress.sin_family = AF_INET;               // Internet Address Family
    serverAddress.sin_port = htons(SERVERPORT);       // Local Port
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // IPAddress Conversion Error Handling
    if (inet_pton(AF_INET, argv[2], &serverAddress.sin_addr) < 0)
    {
        perror("Error: Trouble With Address Conversion");
        exit(1);
    }

    char *fileName = basename(argv[1]);

    //File Retrieval Error Handling
    if (fileName == NULL)
    {
        perror("Error: Can't Retrieve fileName");
        exit(1);
    }

    char buff[MAX_LINE] = {0};

    strncpy(buff, fileName, strlen(fileName));

    socklen_t addrlen = sizeof(serverAddress);

    // Send File Error Handling
    if (sendto(sockfd, buff, MAX_LINE, 0, (struct sockaddr *)&serverAddress, addrlen) < 0)
    {
        perror("Error: Trouble Sending fileName");
        exit(1);
    }

    // Open File Handling
    FILE *fp = fopen(argv[1], "rb");

    // Open File Error Handling
    if (fp == NULL)
    {
        perror("Error: File Can't Be Opened");
        exit(1);
    }

    // Send File
    sendFile(fp, sockfd);

    printf("File '%s' Sent\nFile '%s' Byte Size: %ld\nTerminating Process...\n", fileName, fileName, total);

    fclose(fp);
    close(sockfd);

    return 0;
}

// Send File
void sendFile(FILE *fp, int sockfd)
{

    int n;

    char sendLine[MAX_LINE] = {0};

    struct sockaddr_in serverAddress;
    socklen_t addrlen = sizeof(serverAddress);

    while ((n = fread(sendLine, sizeof(char), MAX_LINE, fp)) > 0)
    {

        total += n;

        // Read File Error Handling
        if (n != MAX_LINE && ferror(fp))
        {
            perror("Error: Trouble Reading File");
            exit(1);
        }

        // Send File Error Handling
        if (sendto(sockfd, sendLine, n, 0, (struct sockaddr *)&serverAddress, addrlen) == -1)
        {
            perror("Error: Trouble Sending File");
            exit(1);
        }

        memset(sendLine, 0, MAX_LINE);
    }
}