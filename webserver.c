#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "webserver.h"

struct _webserver {
  struct sockaddr_in addr;
  int port;
  int server_fd;
};

webserver* webserver_create(int port) {
  int server_fd;
  int opt = 1;
  webserver *ws = malloc(sizeof(*ws));
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    return NULL;
  }

  // Force re-use the port/addr when rapidly restarting server
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    close(server_fd);
    return NULL;
  }

  struct sockaddr_in addr;
  addr.sin_family =  AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  ws->server_fd = server_fd;
  ws->addr = addr;
  ws->port = port;
  return ws;
}

int webserver_run(webserver* ws) {
  // Bind the socket to the network address and port
  if (bind(ws->server_fd, (struct sockaddr *)&(ws->addr), sizeof(ws->addr)) < 0) {
    perror("bind failed");
    close(ws->server_fd);
    return 0;
  }
 
  if (listen(ws->server_fd, 3) < 0) {
    perror("listen failed");
    close(ws->server_fd);
    return 0;
  }

  printf("web server running on port : %d\n", ws->port);

  return 1;
}
