#include<stdio.h>
#include<unistd.h>
#include<errno.h>

int main(int argc, const char *argv[])
{
int rc;
rc = access(argv[1], F_OK);

 if(rc !=0)
 {
  printf("file not found\n");
  printf("Error Number : %d\n", errno);
  }
  else
 {
 printf("file found\n");
 }
 return 0;
}
