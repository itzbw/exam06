#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

void ft_error(){
  write(2, "Fatal error/n", 12);
  exit(1);
}

void send_all(char *buff, int server, int client, int size){
  for (int i = 0; i <= size ; i++){
    if (i != client && i != server){
      send(i, buff, strlen(buff), 0);
    }
  }
}

int main (int argc, char **argv){
  int server, client, size, read;
  int db[65535] = {0};
  int limit = 0;
  struct sockaddr_in serv_addr;
  fd_set old_fd, new_fd;
  char buff[450000];
  char buff2[400000];

  if (argc != 2){
    write(2, "Wrong numbers of arguements\n", 26);
    exit(1);
  }

  server = socket(AF_INET, SOCK_STREAM, 0);
  if (server == -1)
    ft_error();

  bzero(&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(2130706433);
  serv_addr.sin_port = htons(atoi(argv[1]));

  if(bind(server,(const struct sockaddr *)&serv_addr, sizeof(serv_addr)) !=0)
    ft_error();

  if(listen(server, 10) != 0)
    ft_error();

  FD_ZERO(&old_fd);
  FD_ZERO(&new_fd);
  FD_SET(server, &new_fd);
  size = server;

  while(1){
    old_fd = new_fd;
    if (select(size + 1, &old_fd, NULL, NULL, NULL) < 0)
      ft_error();
    for (int id = 0; id <= size; id++){
        if (FD_ISSET(id, &old_fd) == 0)
          continue;
        if (id == server){
          if ((client = accept(server, NULL, NULL)) < 0)
            ft_error();
          if (client > size)
            size = client;
          db[client]=limit++;
          FD_SET(client, &new_fd);
          sprintf(buff, "server: client %d just arrived\n", db[client]);
          send_all(buff, server, client, size);
          printf("server: client %d just arrived\n", db[client]);
      }else {
        bzero(buff, 450000);
        bzero(buff2, 400000);
        read = 1;
        while (read == 1 && ( !buff2[0] || buff2[strlen(buff2) - 1]!='\n'))
          read = recv(id, &buff2[strlen(buff2)],1, 0);
        if (read <= 0){
          sprintf(buff, "server: client %d just left\n", db[id] );
          send_all(buff, server, id, size);
          printf("server: client %d just left\n", db[id] );
          FD_CLR(id, &new_fd);
          close(id);
        }else{
          sprintf(buff, "client %d: %s", db[id], buff2);
          send_all(buff, server, id, size);
          printf("client %d: %s", db[id], buff2);
        }
      }
    }
  }
  return 0;
}