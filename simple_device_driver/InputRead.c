#include <stdio.h>
#include <string.h>

#define SIZE 32

int main()
{
  int fd;
  int n;
  char read_buf[ SIZE ];

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

  n = read(fd, read_buf, SIZE );
  printf("read byte = %d\n", n);
  printf("Read String from kerenl: %s\n", read_buf);
  printf("String len: %d\n", strlen(read_buf)) ;


  close(fd);

  return 0;
}
