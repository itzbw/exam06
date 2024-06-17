#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>


#include <sys/select.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

// Error Handling 
void ft_error (){
  write (2, "Fatal error\n", 12);
  exit(1);
}

//Broadcasting Function
//This function sends a message to all connected clients except the sender (client) and the server (server)
void send_all(char *buff, int server, int client, int size){
  for (int i = 0; i <= size ; i++){
    if (i != client && i != server){
      send(i, buff, strlen(buff), 0);
    }
  }
}

int main (int argc, char **argv){
  int server, client, size, has_read;
  int db[65535] = {0};
  int limit = 0;
  fd_set old_fd, new_fd;
  struct sockaddr_in servaddr;
  char buff[450000];
  char buff2[400000];


// Check if the number of arguments is correct
// If not, print an error message and exit
  if (argc != 2){
    write (2, "Wrong number of arguments\n", 26);
    exit(1);
  }

// Create a socket for the server. AF_INET specifies the address family (IPv4), 
//SOCK_STREAM specifies a stream socket (TCP).
  server = socket(AF_INET, SOCK_STREAM, 0);
  if (server == -1)
    ft_error();

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	servaddr.sin_port = htons(atoi(argv[1]));

  // Bind the server socket to the specified address and port
  if(bind(server, (const struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
    ft_error();
// Make the server listen for incoming connections (with a backlog of 10)
  if (listen(server, 10) != 0)
    ft_error();

  FD_ZERO(&old_fd);
  FD_ZERO(&new_fd);
  FD_SET(server, &new_fd);
  size = server;

  while(1){
    old_fd = new_fd;
    //Monitor multiple file descriptors to see if they have any pending I/O operations
    if (select(size+ 1, &old_fd, NULL, NULL, NULL) < 0)
      ft_error();
    // Handling New Connections and Messages
    for(int id = 0; id <= size ; id++){
      // Check if the file descriptor id is set in old_fd
      
      // If id is the server, accept a new connection and set the new client's file descriptor in new_fd
      if (id == server){ // rememebr!!
      // 
        if ((client = accept(server, NULL, NULL)) < 0)
          ft_error();
        if (client > size)
          size = client;
        db[client] = limit++;
        FD_SET(client, &new_fd);
        sprintf(buff, "server: client %d just arrived\n", db[client]);
        send_all(buff, server, client, size);
      }else { // Handling Client Messages
      bzero(buff, 450000);
      bzero(buff2, 400000);
      has_read = 1;

      while (has_read == 1 && (!buff2[0] || (buff2[strlen(buff2) - 1] != '\n')))
        has_read = recv(id, &buff2[strlen(buff2)],1, 0);
      if (has_read <= 0){
        sprintf(buff, "server: client %d just left\n", db[id]);
        send_all(buff, server, id, size);
        FD_CLR(id, &new_fd);
        close(id);

      }else{
        sprintf(buff, "client %d: %s", db[id], buff2);
        send_all(buff, server, id, size);

      }
    }

      

    }
  } return 0;

  }





