/*
Wes Davis wad104
Jeff Carmical jc4047
Maxwell Young - Data Communication
Programming Assignment 1
There are multiple else and cout statements that are commented out, these were used for debugging purposes.
References:
https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/ 
https://www.bogotobogo.com/cplusplus/sockets_server_client.php 
https://www.geeksforgeeks.org/socket-programming-cc/ 
https://www.thecrazyprogrammer.com/2017/06/socket-programming.html 
https://tutorialspoint.dev/language/cpp/socket-programming-cc 
https://www.youtube.com/watch?v=WDn-htpBlnU 
https://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html
https://www.thomas-krenn.com/en/wiki/Cmd_commands_under_Windows
https://docs.microsoft.com/en-us/windows/terminal/command-line-arguments?tabs=windows
https://www.binarytides.com/udp-socket-programming-in-winsock/
https://docs.microsoft.com/en-us/samples/microsoft/windows-universal-samples/datagramsocket/
https://askubuntu.com/questions/61408/what-is-a-command-to-compile-and-run-c-programs
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <netdb.h> 
#include <errno.h>
#include <arpa/inet.h>   // if you want to use inet_addr() function
#include <string.h>
#include <unistd.h>
using namespace std;

//error function that will display error message
void error(const char *msg){
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[]){
    //Stage 1 Begins Here
    //Declare variables used for clientside of the program
    int mysocket, n_port, r_port, numChars;
    socklen_t len;
    struct hostent *s;
    s = gethostbyname(argv[1]);
    struct sockaddr_in serverAddy, server1, from;

    //searching for the negotiation port
    if(argc < 3){
        exit(0);
    }

    //create a negotiation socket
    n_port = atoi(argv[2]);
    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mysocket < 0){
        error("Error with finding a Negotiation socket");
    }
    else{
        //cout << "Success! A negotiation socket has been created!" << endl;
    }

    //create information for talking to the server
    if(s == NULL){
        cout << "Error: Unable to find host" << endl;
        exit(0);
    }
    else{
        //cout << "Server has been found" << endl;
    }

    //configure variables used for the server address
    bzero((char *) &serverAddy, sizeof(serverAddy));
    serverAddy.sin_family = AF_INET;
    bcopy((char *) s->h_addr, (char *)&serverAddy.sin_addr.s_addr, s->h_length);
    serverAddy.sin_port = htons(n_port);

    if(connect(mysocket, (struct sockaddr *) &serverAddy, sizeof(serverAddy)) < 0){
        error("Unable to connect");
    }
    else{
        //cout << "Success! Socket is now connected" << endl;
    }

    //declare a value "1998" to be used for connecting with server
    char initiateCode[5] = "1998";
    numChars = write(mysocket, &initiateCode, sizeof(initiateCode));
    if(numChars < 0){
        error("Error: Failure to send to socket");
    }
    
    //reading the r_port from server program
    numChars = read(mysocket, &r_port, 255);
    if(numChars < 0){
        error("Unable to read random port from server.");
    }

    close(mysocket);
    //End of Stage 1

    //Stage 2 begins here (File Transfer)
    mysocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(mysocket < 0){
        //error("Error: Unable to create socket");
    }
    else{
        //cout << "Socket has been created!" << endl;
    }

    struct hostent *hp;
    memset((char *)&server1, 0, sizeof(server1));
    server1.sin_family = AF_INET;
    hp = gethostbyname(argv[1]);
    bcopy((char *)hp->h_addr, (char *)&server1.sin_addr, hp->h_length);
    server1.sin_port = htons(r_port);
    len = sizeof(server1);

    //create function to read and open text files
    fstream textfile;
    textfile.open(argv[3], fstream::in);
    if(!textfile.is_open()){
        cout << "File did not open." << endl;
    }
        filebuf* text_string = textfile.rdbuf();
    char w = text_string->sbumpc();

    //create an array for the file contents
    char sendArray[5];
    char recvArray[5];

    //this portion will send the array to the server
    int i = 0;
    usleep(2000);
    while(w != EOF){
        while((w != EOF) && (i < 4)){
            sendArray[i] = w;
            w = text_string->sbumpc();
            i++;
        }

        if(w == EOF){
            while(i < 4){
                sendArray[i++] = 0;
            }
        }
        i = 0;

        numChars = sendto(mysocket, sendArray, sizeof(sendArray), 0, (const struct sockaddr *)&server1, sizeof(server1));
        numChars = recvfrom(mysocket, recvArray, sizeof(recvArray), 0, (struct sockaddr*)&from, &len);
        cout << recvArray << endl;
    }
    textfile.close();
    close(mysocket);
    //End of Stage 2
}