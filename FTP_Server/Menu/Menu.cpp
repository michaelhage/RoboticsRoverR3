#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string.h>
#include <arpa/inet.h>
#include <ctime>
using namespace std;

#define MAXLINE 1000
#define MAXSEND 50

//#define THROTTLE 0.02 //throttle multiplier; limits bandwidth
//#define MAXBAND 32 //32kbps 

void client_FTP(string, int, string);
int create_socket(int,char *);

// This main function is used for testing purposes
int main(){

    //
    string ipAddress = "127.0.0.1";
    int portNumber = 15000;
    
    client_FTP(ipAddress, portNumber);

    //char buf[MAXLINE];
    /*while(strcmp(buf, "ZZZ\n") != 0){
        cout<<"ftp>";
        fgets(buf, MAXLINE, stdin);
        cout<<buf << "\n";
        //Some Selection
        if(strcmp("PIC\n", buf) == 0){
            cout<<"You Have Selected \n";
        }
        //Another Selection
        else if(strcmp("B\n", buf) == 0){
            cout<<"You Have Selected B\n";
        }
        else if(strcmp("ZZZ\n", buf) == 0){

        }
        //Error
        else{
            cout<<"Selection Error, Send 'ZZZ' to quit\n";
        }
    }*/

    exit(0);
}

//This is a file downloader
void client_FTP(string ipAddress, int portNumber, string command){
    
    int sockfd;
    struct sockaddr_in servaddr;
    char sendLine[MAXLINE], recvLine[MAXLINE];
    char *ipTemp = new char[ipAddress.length() + 1];

    cout<<"This is the IP Address: "<<ipAddress<<"\n";
    cout<<"This is the Port Number: "<<portNumber<<"\n";

    //Creation of the socket should remain unchanged

    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    cerr<<"Problem in creating the socket"<<endl;
    exit(2);
    }

    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;

    servaddr.sin_addr.s_addr= inet_addr(ipTemp);//Adds IP Address

    servaddr.sin_port = htons(portNumber); //convert to big-endian order

    //Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
    cerr<<"Problem in connecting to the server"<<endl;
    exit(3);
    }

    strcpy(sendLine,"get blue.jpg");

    send(sockfd, sendLine, MAXLINE, 0);
    char *token,*dummy;
    dummy=sendLine;
    token=strtok(dummy," ");

    //sends request to download file, which will be main function
    if (strcmp("PUO",token)==0)  {
        
        char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
        int data_port,datasock,lSize,num_blks,num_last_blk,i;
        FILE *fp;
        
        recv(sockfd, port, MAXLINE,0);
        data_port=atoi(port);
        datasock=create_socket(data_port,ipTemp);
        token=strtok(NULL," \n");
        recv(sockfd,message,MAXLINE,0);
        if(strcmp("1",message)==0){
            if((fp=fopen(token,"w"))==NULL)
                cout<<"Error in creating file\n";
            else
            {
                recv(sockfd, char_num_blks, MAXLINE,0);
                num_blks=atoi(char_num_blks);
                for(i= 0; i < num_blks; i++) { 
                    recv(datasock, buffer, MAXLINE,0);
                    fwrite(buffer,sizeof(char),MAXLINE,fp);
                    //cout<<buffer<<endl;
                }
                recv(sockfd, char_num_last_blk, MAXLINE,0);
                num_last_blk=atoi(char_num_last_blk);
                if (num_last_blk > 0) { 
                    recv(datasock, buffer, MAXLINE,0);
                    fwrite(buffer,sizeof(char),num_last_blk,fp);
                    //cout<<buffer<<endl;
                }
                fclose(fp);
                cout<<"File download done."<<endl;
            }
        }
        else{
            cerr<<"Error in opening file. Check filename\nUsage: put filename"<<endl;
        }
    }

    // PULL NEW File
    else if (strcmp("PUN",token)==0)  {
        
        char port[MAXLINE], buffer[MAXLINE],char_num_blks[MAXLINE],char_num_last_blk[MAXLINE],message[MAXLINE];
        int data_port,datasock,lSize,num_blks,num_last_blk,i;
        FILE *fp;
        
        recv(sockfd, port, MAXLINE,0);
        data_port=atoi(port);
        datasock=create_socket(data_port,ipTemp);
        //token=strtok(NULL," \n");
        
        //Create Timestamp, will use for filename
        //Will implement better method eventually, maybe
        std::time_t timestamp = std::time(nullptr);

        string filename = to_string(timestamp) + ".jpg"

        
        recv(sockfd,message,MAXLINE,0);
        if(strcmp("1",message)==0){


            if((fp=fopen(filename,"w"))==NULL)
                cout<<"Error in creating file\n";
            else
            {
                recv(sockfd, char_num_blks, MAXLINE,0);
                num_blks=atoi(char_num_blks);
                for(i= 0; i < num_blks; i++) { 
                    recv(datasock, buffer, MAXLINE,0);
                    fwrite(buffer,sizeof(char),MAXLINE,fp);
                    //cout<<buffer<<endl;
                }
                recv(sockfd, char_num_last_blk, MAXLINE,0);
                num_last_blk=atoi(char_num_last_blk);
                if (num_last_blk > 0) { 
                    recv(datasock, buffer, MAXLINE,0);
                    fwrite(buffer,sizeof(char),num_last_blk,fp);
                    //cout<<buffer<<endl;
                }
                fclose(fp);
                cout<<"File download done."<<endl;
            }
        }
        else{
            cerr<<"Error in opening file. Check filename\nUsage: put filename"<<endl;
        }
    }
}

int create_socket(int port,char *addr)
{
    int sockfd;
    struct sockaddr_in servaddr;


    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
    cerr<<"Problem in creating the socket"<<endl;
    exit(2);
    }

    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(addr);
    servaddr.sin_port =  htons(port); //convert to big-endian order

    //Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
    cerr<<"Problem in creating data channel"<<endl;
    exit(3);
    }

    return(sockfd);
}
