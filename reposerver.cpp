/* NIKITA WANI 20162023 */
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
/*
struct sockaddr_in
{
  short   sin_family; // must be AF_INET 
  u_short sin_port;
  struct  in_addr sin_addr;
  char    sin_zero[8]; // Not used, must be zero 
}
*/
 
map<string,vector<pair<string,string> > > loadmap()
{

    string line,substr2,path,ip,name;
    size_t pos;
    map<string,vector<pair<string,string> > > mymap;
    vector<pair <string,string> > v;
    ifstream myfile ("repo.txt");

    if(myfile.is_open())
    {
        while(getline(myfile,line))
        {
            pos= line.find_first_of(" ");
            name=line.substr(0,pos);
            substr2=line.substr(pos+1);

            pos= substr2.find_first_of(" ");
            ip=substr2.substr(0,pos);
            path=substr2.substr(pos+1);

            mymap[name].push_back(make_pair(ip,path));
        }
        myfile.close();
    }   
    else
        cout<<"Error opening file"<<endl;
    
    return mymap;
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    // serv_addr will contain the server address,cli_addr will contain client address
    struct sockaddr_in serv_addr, cli_addr;
    string a,b;
    size_t pos;

    int n;
    char buffer[1024]; 
    char mirror[1024]={'\0'};
    string name,name2,name3,filename,path,ip,entry;   
    map<string,vector<pair<string,string> > > mymap;
    vector<pair <string,string> > v;
    

    bzero(buffer,1024);
    if (argc < 2) 
    {
        cout<<"ERROR, no port provided\n";
        exit(1);
    }
    // Creates a new socket
    // Three arguments: Address domain, type of socket and protocol
    // AF_INET: Internet domain
    // SOCK_STREAM: stream socket in which characters are read in a continuous stream
    // 0: TCP for stream sockets and UDP for datagram sockets
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        cout<<"ERROR opening socket";
     
    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    portno = atoi(argv[1]);
    
    // code for the address family 
    serv_addr.sin_family = AF_INET;
    // IP address of the machine
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Convert into network byte order, host to network conversion for short ints
    serv_addr.sin_port = htons(portno);
     
    //  Bind socket to an address i.e., the current host address and port number on which the server will run
    // Three Arguments: socket file descriptor, address to which is bound and the address size
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        cout<<"ERROR on binding";
    // Listen on the socket for connections
    // Two arguments: socket file descriptor and backlog queue size
    listen(sockfd,5);
     
    clilen = sizeof(cli_addr);

    while (1) 
    {
    	// accept() system call causes the process to block until a client connects to the server.
    	// All communication on this connection is done using the new file descriptor
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            cout<<"ERROR on accept";
        pid = fork();
        if (pid < 0)
            cout<<"ERROR on fork";
        
        if (pid == 0) 
        {
        	// Child process: Process the connection on the new socket created and close the old socket
            close(sockfd);
            while(1)
            {
                n = read(newsockfd,buffer,1024);
                if (n < 0) 
                    cout<<"ERROR reading from socket";
                
                name=buffer;
                ofstream logfile;
                logfile.open ("repo.log", ios_base::app);
                time_t rawtime;
                struct tm * localTime;
                time (&rawtime);
                localTime = localtime(&rawtime);
                int day=localTime->tm_mday;
                int Month = localTime->tm_mon + 1;
                int Year = localTime->tm_year + 1900;
                int Hour = localTime->tm_hour;
                int Min = localTime->tm_min;
                int Sec = localTime->tm_sec;
                if(name.substr(0,6)=="search")
                {
                    name2=name.substr(9);
                    mymap=loadmap(); 
                
                    ip=inet_ntoa(cli_addr.sin_addr);
                    logfile<<day<<'-'<<Month<<'-'<<Year<<' '<<Hour<<'-'<<Min<<'-'<<Sec<<':'<<"Search request from "<<ip<<"\n";
                    for (map<string,vector<pair<string,string> > >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
                    {
                        v=mymap[it->first];
                        if(!name2.find(it->first))
                        {   
                            for(vector<pair<string,string> >::iterator it2=v.begin();it2!=v.end();it2++)
                            {
                                strcat(mirror,it->first.c_str());
                                strcat(mirror," ");
                                strcat(mirror,it2->first.c_str());
                                strcat(mirror," ");
                                strcat(mirror,it2->second.c_str());
                                strcat(mirror,"\n");
                            }
                        }
                    }
                    n=send(newsockfd,mirror,sizeof(buffer),0);
                    if (n < 0) 
                        cout<<"ERROR writing to socket";
                    bzero(buffer,1024);
                    bzero(mirror,1024);
                    logfile<<day<<'-'<<Month<<'-'<<Year<<' '<<Hour<<'-'<<Min<<'-'<<Sec<<':'<<"Search response send to "<<ip<<"\n";
              
                }
                else if(name.substr(0,8)=="share#@#")
                {
                    logfile<<day<<'-'<<Month<<'-'<<Year<<' '<<Hour<<'-'<<Min<<'-'<<Sec<<':'<<"Share request from "<<ip<<"\n";
                    name=buffer;
                    name2=name.substr(8);
                    pos=name2.find_last_of("/\\");
                    path=name2.substr(0,pos+1);
                    filename=name2.substr(pos+1);
                    ip=inet_ntoa(cli_addr.sin_addr);
                    mymap[filename].push_back(make_pair(ip,path));
                    ofstream myfile ( "repo.txt", ios_base::app );
                    myfile<<filename<<' '<<ip<<' '<<path<<'\n';
                    bzero(buffer,1024);
                    myfile.close();
                    logfile<<day<<'-'<<Month<<'-'<<Year<<' '<<Hour<<'-'<<Min<<'-'<<Sec<<':'<<"Share ack send to "<<ip<<"\n";
                    
                }
            }
            exit(0);
        }
      	
        else {
        	// Parent process: Close the socket connection created for child process and continue to listen to client requests
            close(newsockfd);
        }
    } 
    close(sockfd);
    return 0;
}