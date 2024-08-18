#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 10000
#define BUFFER_SIZE 1024

int handle_client(int socket_fd);

int main() {
  int server_fd;
  int opt = 1;
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Unable to create socket");
    exit(EXIT_FAILURE);
  }

  // Force re-use the port/addr when rapidly restarting server
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in addr;
  addr.sin_family =  AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  // Bind the socket to the network address and port
  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
 
  if (listen(server_fd, 3) < 0) {
    perror("listen failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("web server running on port : %d\n", PORT);

  while(1) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd;

    if((client_fd = accept(server_fd,
                           (struct sockaddr*)&client_addr,
                           &client_addr_len)) < 0) {
      perror("unable to accept connection");
      continue;
    }

    char ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), ip_str, INET_ADDRSTRLEN);

    printf("Got connection from %s:%d\n", ip_str, client_addr.sin_port);
    handle_client(client_fd);
    close(client_fd);

  }

}

int handle_client(int socket_fd) {
  char *buffer = (char*) malloc(BUFFER_SIZE * sizeof(char)+1);
  int total_bytes_received = 0;
  while(1) {
    ssize_t bytes_received = recv(socket_fd,
                           (char*)(buffer+total_bytes_received),
                           BUFFER_SIZE - total_bytes_received,
                           0);
    if(bytes_received < 0){
      perror("Unable to receive data from socket");
      return -1;
    }
    if(bytes_received == 0 ) {
      break; // Client closed connection
    }
    buffer[total_bytes_received+bytes_received] = 0;
    // Detect end of http request header
    if(strstr(buffer+total_bytes_received,"\r\n\r\n")) {
      total_bytes_received += bytes_received;
      break;
    }

    total_bytes_received += bytes_received;
    // If the buffer is full, break
    if (total_bytes_received >= BUFFER_SIZE) {
      break;
    }
  }

  printf("%s", buffer);
  fflush(stdout);

  return total_bytes_received;
}
