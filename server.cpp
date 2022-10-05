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
#include <cstdlib>
#include <sys/types.h>   // defines types (like size_t)
#include <sys/socket.h>  // defines socket class
#include <netinet/in.h>  // defines port numbers for (internet) sockets, some address structures, and constants
#include <time.h>        // used for random number generation
#include <string.h> // using this to convert random port integer to string
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
using namespace std;

//error function that will display error message
void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    //Stage 1: Handshake
    //declare variables
    int mysocket, newmysocket, n_port, r_port, c_len, numChars, s_len;
    socklen_t from_len;
    char buffer[5];
    struct sockaddr_in serverAddy, clientAddy, server, from;

    if(argc < 2){
        cout << "Error: no negotiation port has been provided." << endl;
        exit(1);
    }

    //create a socket
    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    if(mysocket < 0){
        error("Error opening socket");
    }
    else{
        //cout << "Socket has been created!!!" << endl;
    }

    //declare buffer and zero it out
    bzero((char *) &serverAddy, sizeof(serverAddy));
    n_port = atoi(argv[1]);

    //setup the server address
    serverAddy.sin_family = AF_INET;
    serverAddy.sin_port = htons(n_port);
    serverAddy.sin_addr.s_addr = INADDR_ANY;

    //attaching port to socket
    if(bind(mysocket, (struct sockaddr *) &serverAddy, sizeof(serverAddy)) < 0){
        error("Error in attaching");
    }
    else{
        //cout << "Matching port has been attached to the socket." << endl;
    }

    //listen for a possible connection
    listen(mysocket, 5);
    c_len = sizeof(clientAddy);

    //connecting the new socket to the client socket
    newmysocket = accept(mysocket, (struct sockaddr *) &clientAddy, (socklen_t*) &c_len);
    if(newmysocket < 0){
        error("Error on the accept function");
    }
    else{
        //cout << "Socket has been connected" << endl;
    }

    numChars = read(newmysocket, buffer, sizeof(buffer));
    if (numChars < 0){
        error("Error: unable to read socket");
    }
    else{
        //cout << "Success! Reading data now..." << endl;
    }
    
    if(strcmp(buffer, "1998") != 0){
        cout << "Error: Incorrect Code Word! " << buffer << endl;
        exit(0);
    }
    else{
        srand(time(NULL));
        r_port = rand() % (65535-1024) + 1024;
        numChars = write(newmysocket, &r_port, sizeof(r_port));
        cout << "Random port chosen: " << r_port << endl;
    }

    if(numChars < 0){
        error("Error writing to socket");
    }
    close(newmysocket);

    //End of Stage 1

    //Stage 2
    mysocket = socket(AF_INET, SOCK_DGRAM, 0);
    if(mysocket < 0){
        error("Error: socket error");
    }
    else{
        //cout << "Socket has been made!!" << endl;
    }

    s_len = sizeof(server);   
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(r_port);

    if(bind(mysocket, (struct sockaddr *)&server, s_len) < 0){
        error("Error unable to bind to socket.");
    }
    else{
        //cout << "The binding process was a complete success!" << endl;
    }
    
    from_len = sizeof(struct sockaddr_in);

    fstream received;
    received.open("upload.txt", ios::app);
    std::filebuf* outfile = received.rdbuf();
    char recvArray[5];
    while(1){
        numChars = recvfrom(mysocket, recvArray, sizeof(recvArray), 0, (struct sockaddr *)&from, &from_len);

        for(int i = 0; i < 4; i++){
            if(recvArray[i] == 0){
                for(int j = 0; j < sizeof(recvArray); j++){
                    recvArray[j] = toupper(recvArray[j]);
                }
                numChars = sendto(mysocket, recvArray, sizeof(recvArray), 0, (struct sockaddr *)&from, from_len);
                close(mysocket);
                received.close();
                return 0;

            }
            outfile->sputc(recvArray[i]);
        }

        for(int i = 0; i < sizeof(recvArray); i++){
            recvArray[i] = toupper(recvArray[i]);
        }
        numChars = sendto(mysocket, recvArray, sizeof(recvArray), 0, (struct sockaddr *)&from, from_len);
        
    }
    //End of Stage 2
}