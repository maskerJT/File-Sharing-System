# File Sharing System

Implemented LAN file sharing system using socket programming.
A central repo server which would handle the access to central share repository(repo.txt). Central repository is a simple file which maintains a map of machine ip and file path from which file can be downloaded.
A client which doubles up as a server for other nodes to download file. It provides a menu driven interface to do the following: 

1. Search for a file to download 
2. Share a file on central repository 

## To Run:

Compile: 
  
    g++ server.cpp -o server
    g++ client.cpp -o client
    
Execute:

    ./server <serverPort>
    ./client <serverIPAddress> <serverPort>
