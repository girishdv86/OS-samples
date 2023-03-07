#include <stdio.h>
#include <grp.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define NULL_POINTER NULL
#define OK 0

#define XCS_MAXUSRNAME 8
#define XCS_MAXGRPNAME XCS_MAXUSRNAME
#define DFTBUF_XCSGETGRGID \
          XCS_MAXGRPNAME + 1                  /* gr_name          */\
          + 1 + 1                             /* dummy gr_passwd  */\
          + 10 + 1                            /* length of userid */\
          + ((XCS_MAXUSRNAME + 1)*4000)       /* users in group   */\
          + 255                               /* contingency!     */

#define DFTBUF_XCSGETGRENT DFTBUF_XCSGETGRGID

struct group * xcsGetgrent( struct group *result
                                   , char         *buffer
                                   , int           buflen
                                   , struct group        **grfp 
                                   , int        *pRetCode )
{
  int saverrno = 0;
  int counter = 0;
  int Ret = 0;

#if defined(_REENTRANT)

  printf("Reentrant getgrent_r\n");
  do
  {
    errno = 0;
    saverrno = 0;
    Ret = 0;
    if( (Ret=getgrent_r(result, buffer, buflen, grfp)) != 0 )
    {
      saverrno = errno;
    }
  } while ( (saverrno == EINTR) && (counter++ < 10));

  if (Ret != 0 || *grfp == NULL_POINTER)
  {
    /* if(saverrno == ERANGE)
      *pRetCode=saverrno;
    else
      *pRetCode=ENOENT; */

    *pRetCode = (saverrno == ERANGE) ? saverrno : ENOENT;
    result = NULL_POINTER;
    printf("SAVED ERRNO = %d\n", saverrno);
  }
  else
  {
    *pRetCode = OK;
  }

#else /* of _REENTRANT */

  do
  {
    errno = 0;
    saverrno = 0;
    result = getgrent();
    if (result == NULL) 
    {
      saverrno=errno;
      printf("SAVED ERRNO = %d\n", saverrno);
    }
  } while ((saverrno == EINTR) && (counter++ < 10));
  
  if( result == NULL_POINTER )
    *pRetCode = saverrno;
  else
    *pRetCode = OK;

#endif /* of _REENTRANT */

  errno = saverrno;
  return(result);
}

int main( int argc, char *argv[] )
{
  struct group *pCurrentGrp = NULL;
  int reasonCode = 0;  
  int noOfTimes=0, i=0;
  char **pEntry;
  char *pGrpbuf = NULL_POINTER;
  FILE     **grfp;
  int error=0;
  struct group CurrentGrp, *grpp;
  int buflen = DFTBUF_XCSGETGRGID;
  int Retry = 0;

  if( argc > 1 )
  {
    printf( "PRINCIPAL[%s]\n", argv[1] );
  }

#if defined(_REENTRANT)
   printf("Reentrant getgrent_r\n");
#endif

  pGrpbuf =  malloc(buflen);
  if( pGrpbuf == NULL )
  {
    printf("malloc failure : size[%d]\n", buflen);
    exit(1);
  }

  do
  {
    Retry = 0;
    pCurrentGrp = xcsGetgrent(&CurrentGrp, pGrpbuf, buflen, &grpp, &error);
    noOfTimes++;

    if(error == ERANGE)
    {
      Retry = 1;
      printf( "ERANGE - Reallocating buffer...\n" );
      free( pGrpbuf );
      buflen += DFTBUF_XCSGETGRENT;
      pGrpbuf =  malloc(buflen);
      if( pGrpbuf == NULL )
      {
        printf("malloc failure : size[%d]\n", buflen);
        break;
      }
      continue;
    }

    if (pCurrentGrp == NULL)
    {   
      reasonCode=errno;
      break;
    }
    else
    {
      printf("------------------------------------------------------------------------------------\n");
      printf("Called getgrent() sucessfully %d time(s) - Group is '%.12s'\n",noOfTimes,pCurrentGrp->gr_name);
    }

      /*-------------------------------------------------------------*/
      /* Search all the users in this group for the principal.       */
      /*-------------------------------------------------------------*/
      pEntry = pCurrentGrp -> gr_mem;
      for( i = 0 ; pEntry[i] ; i++ )
      {
        /* Is it the user we're looking for? */
        printf("[%d] user[%s]\n", i, pEntry[i]);
        if( argc > 1 )
        {
          if ( strcmp( (char *) pEntry[i], argv[1]) == 0 )
          {
            printf("FOUND PRINCIPAL[%s] GROUP[%s] \n", argv[1], pCurrentGrp->gr_name);
          }
        }
      }
  }while( ((pCurrentGrp != NULL_POINTER)
         &&(pCurrentGrp->gr_mem != NULL_POINTER)
         &&(pCurrentGrp->gr_name != NULL_POINTER)
         &&(error == OK)) || Retry );

  if ( (reasonCode == ENOENT) || (reasonCode == 0) )
  {
    printf("\nExited getgrent loop normaly after %d iteration(s) with reasonCode == %d.\n",noOfTimes,reasonCode);
  }
  else
  {
    printf("\nExited getgrent loop ABNORMALLY after %d iteration(s)\n",noOfTimes);
    printf("Error code: %d\n",reasonCode);
  }
 
  endgrent();

  return 0;
}

