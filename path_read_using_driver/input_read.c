#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define SIZE 100

int main()
{
  int fd;
  int n;
  char read_buf[ SIZE ];
  read_buf[0] = '\0';

  fd = open("/dev/path_reader_driver", 0666);
  if(fd < 0)
  {
    printf("Device Open Error\n\n");
    return 0;
  }
  else
  {
    printf("Device Open Success.\n");
  }

  n = read(fd, read_buf, SIZE );
  if(n ==0 )
      printf("Error in path reading\n") ;
  else if(n ==-1)  
      printf("Path length bigger than buffer size\n") ;
  else{
    printf("Path length: %d\n", n ) ;
    read_buf[n] = '\0'; 
    printf("Path Read from kerenl: %s\n", read_buf);

  } 
  close(fd);

  return 0;
}
