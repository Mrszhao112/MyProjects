#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//客户端 标准输入读数据进行发送 然后接收数据打印到标准
int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(9000);
  inet_aton("192.168.96.129", &addr.sin_addr);
  int r = connect(fd, (struct sockaddr*)&addr, sizeof(addr));
  if (r == -1) perror("connect"),exit(1);

  char buf[1024] = {};

  while (fgets(buf, 1024, stdin) != NULL) {
    write(fd, buf, strlen(buf));
    memset(buf, 0x00, sizeof(buf));
    int r = read(fd, buf, 1024);
    if (r <= 0) break;
    printf("=> %s\n", buf);

  }
  return 0;
}
