#include <grp.h>
#include <stdio.h>

main()
{
  struct group *grp;
  short int    lp;

  if (NULL == (grp = getgrnam("mqm")))
     perror("getgrnam() error.");
  else
  {
     printf("The group name is: %s\n", grp->gr_name);
     printf("The gid        is: %u\n", grp->gr_gid);
	 printf("Group member is: %s\n", *(grp->gr_mem));
	 printf("Group member is: %s\n", *(grp->gr_mem)++);
     for (lp = 1; NULL != *(grp->gr_mem); lp++, (grp->gr_mem)++)
        printf("Group member %d is: %s\n", lp, *(grp->gr_mem));
  }

}
