#include<stdio.h>
#include<string.h>

int main()
{
  int fd;
  int n;
  char Message[] = {"My Message That Send"};

  fd = open("/dev/my_char_device", 0666);
  if(fd < 0)
  {
    printf("Device Open Error\n");
    return;
  }
  else
  {
    printf("Device Open Success.\n");
  }

  n = write(fd, Message, strlen(Message) );
  printf("Byte write = %d\n", n);
  close(fd);

  return 0;
}
