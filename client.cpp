/* NIKITA WANI 20162023 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <fstream>
#include <arpa/inet.h>
using namespace std;

ofstream logfile;
int client(int sockfd)
{
    int size,n,i,j,k,pos;
    //option;
    char option;
    char buffer[1024];
    char buffer2[1024];
    char buffer3[1024];
    char mirror[1024][1024];
    string name,name2,ip,path,substr2;

    struct sockaddr_in serv_addr2;
    struct hostent *server2;
    int serversock;

    printf("\nSelect:\n1.Search\n2.Share\n3.Exit\n");
    bzero(buffer,1024);
    cin>>option;
    if(option=='1')
    {
        printf("Type string to search: ");
        bzero(buffer,1024);
        cin>>buffer;
        sprintf(buffer2,"search#@#%s",buffer);
        n = write(sockfd,buffer2,strlen(buffer2));
        bzero(buffer,1024);
        n=recv(sockfd,buffer,sizeof(buffer),0);
        if(n<0)
            cout<<"ERROR reading from socket";
        j=0;
        k=0;
        string s=buffer;
        if(s.empty())
        {
            cout<<"No mirrors\n";
        }
        else
        {
            size=sizeof(buffer);
            for(i=0;i<size;i++)
            {
                mirror[j][k]=buffer[i];
                k++;   
                if(buffer[i]=='\n')
                {
                    j++;
                    k=0;
                }
            }
            
            cout<<"\nSelect a mirror:"<<endl;
            for(i=0;i<j;i++)
            {
                cout<<i+1<<". ";
                name2=mirror[i];
                pos=name2.find_first_of(" ");
                name=name2.substr(0,pos);
                substr2=name2.substr(pos+1);

                pos= substr2.find_first_of(" ");
                ip=substr2.substr(0,pos);
                path=substr2.substr(pos+1);
                
                cout<<name<<"   "<<ip<<endl;
            }
            cin>>j;
            while(j>i || j<1)
            {
                cout<<"Invalid choice\n";
                cin>>j;
            }
            name2=mirror[j-1];
            pos=name2.find_first_of(" ");
            name=name2.substr(0,pos);
            substr2=name2.substr(pos+1);

            pos= substr2.find_first_of(" ");
            ip=substr2.substr(0,pos);
            path=substr2.substr(pos+1);
            serversock = socket(AF_INET, SOCK_STREAM, 0);
            if (serversock < 0) 
                cout<<"ERROR opening socket";
            //const char *ipadd=ip.c_str();
            //server2 = gethostbyname("localhost");
            char ipadd[100];
            memset(ipadd,'\0',100);
            strcpy(ipadd,ip.c_str());
            server2 = gethostbyname(ipadd);

            if (server2 == NULL) 
            {
                cout<<"ERROR, no such host\n";
                exit(0);
            }

            bzero((char *) &serv_addr2, sizeof(serv_addr2));

            serv_addr2.sin_family = AF_INET;
            bcopy((char *)server2->h_addr,(char *)&serv_addr2.sin_addr.s_addr,server2->h_length);
    
            serv_addr2.sin_port = htons(24000);

            if (connect(serversock,(struct sockaddr *) &serv_addr2,sizeof(serv_addr2)) < 0) 
                cout<<"ERROR connecting";


            bzero(buffer3,1024);
            pos=path.find("\n");
            path=path.substr(0,pos);
            path=path+name;
            strncpy(buffer3,path.c_str(),sizeof(buffer3));
            n = write(serversock,buffer3,strlen(buffer3)); 


            FILE *fp;
            int n1;//flag=1;
            path=name;
            bzero(buffer3,1024);
            strncpy(buffer3,path.c_str(),sizeof(buffer3));
        
            fp=fopen(buffer3,"wb");
            bzero(buffer2,1024);
            while ((n1=recv(serversock,buffer2,1024,0))>0)
            {
                fwrite(buffer2, sizeof(char), n1, fp);  
            }
            //cout<<"Download complete!\n";
            fclose(fp);
            bzero(buffer2,1024); 
        }
    }
    else if(option=='2')
    {
        printf("Path :");
        bzero(buffer,1024);
        cin>>buffer;
        sprintf(buffer2,"share#@#%s",buffer);
        n = write(sockfd,buffer2,strlen(buffer2));
        cout<<"File Shared Successfully on Repo!"<<endl;
    }
    else if(option=='3')
    {
        exit(0);
    }
    else
    {
        printf("Invalid option\n");
           //continue;
    }
    if (n < 0) 
        cout<<"ERROR writing to socket";
    bzero(buffer,1024);
    bzero(buffer2,1024);

    return 0;
}

int clientmain(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if (argc < 3) 
    {
       cout<<"usage "<<argv[0]<< " hostname port\n";
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        cout<<"ERROR opening socket";

    server = gethostbyname(argv[1]);
    
    if (server == NULL) 
    {
        cout<<"ERROR, no such host\n";
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    
    serv_addr.sin_port = htons(portno);

    // To establish a connection to the server.
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        cout<<"ERROR connecting";
    
     while(1)
    {
        client(sockfd);
        
    }
    close(sockfd);
    return 0;
}

int servermain()
{
    int sockfd, newsockfd; 
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[1024];
    char buffer2[1024];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        cout<<"ERROR opening socket";
     
    bzero((char *) &serv_addr, sizeof(serv_addr));
     
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(24000);
     
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        cout<<"ERROR on binding";
    listen(sockfd,5);
     
    clilen = sizeof(cli_addr);
    string ip;

    while (1) 
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        ip=inet_ntoa(cli_addr.sin_addr);
        if (newsockfd < 0) 
            cout<<"ERROR on accept";
        bzero(buffer,1024);
        // buffer contains path and filename
        read(newsockfd,buffer,1024);
        ofstream logfile;
        logfile.open ("client.log", ios_base::app);
    
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

        //char buff[1];
        FILE *f; 
        f=fopen(buffer,"rb");
        logfile<<day<<'-'<<Month<<'-'<<Year<<' '<<Hour<<'-'<<Min<<'-'<<Sec<<':'<<"Download request from "<<ip<<"\n";
                   
        if(f!=NULL)
        {
            bzero(buffer2,1024);
            int n1; 
            while (!feof(f)) 
            {
                n1=fread(buffer2,sizeof(char), 1024, f);
                send(newsockfd,buffer2,n1,0);
            }
            cout<<"Download complete!\n";
            bzero(buffer2,1024);
            fclose(f);
        }
        else
        {
            cout<<"File does not exist"<<"\n";
        }
        logfile<<day<<'-'<<Month<<'-'<<Year<<' '<<Hour<<'-'<<Min<<'-'<<Sec<<':'<<"File sent to "<<ip<<"\n";
                  
        close(newsockfd);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int pid=fork();
    if(pid==0)
    {
        // For downloading a file
        servermain();
    }
    else
    {
        clientmain(argc,argv);
    }
    return 0;
}
