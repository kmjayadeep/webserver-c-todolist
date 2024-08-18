#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "webserver.h"

#define PORT 10000
#define BUFFER_SIZE 1024

int main() {
  webserver *ws = webserver_create(PORT);
  webserver_run(ws);
}
