#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dbseg_jseg_xv.h" 

/***********************************/
void FatalError (identifier)
      char *identifier;
{
  fprintf(stderr, "%s\n", identifier);
  exit(-1);
}


/***************************************************/
char *BaseName(fname)
     char *fname;
{
  char *basname;

  /* given a complete path name ('/foo/bar/weenie.gif'), returns just the 
     'simple' name ('weenie.gif').  Note that it does not make a copy of
     the name, so don't be modifying it... */

  basname = (char *) strrchr(fname, '/');
  if (!basname) basname = fname;
  else basname++;

  return basname;
}

  
/***************************************************/
void xvbcopy(src, dst, len)
     char *src, *dst;
     size_t  len;
{
  /* Modern OS's (Solaris, etc.) frown upon the use of bcopy(),
   * and only want you to use memcpy().  However, memcpy() is broken,
   * in the sense that it doesn't properly handle overlapped regions
   * of memory.  This routine does, and also has its arguments in
   * the same order as bcopy() did, without using bcopy().
   */

  /* determine if the regions overlap
   *
   * 3 cases:  src=dst, src<dst, src>dst
   *
   * if src=dst, they overlap completely, but nothing needs to be moved
   * if src<dst and src+len>dst then they overlap
   * if src>dst and src<dst+len then they overlap
   */

  if (src==dst || len<=0) return;    /* nothin' to do */
  
  if (src<dst && src+len>dst) {  /* do a backward copy */
    src = src + len - 1;
    dst = dst + len - 1;
    for ( ; len>0; len--, src--, dst--) *dst = *src;
  }

  else {  /* they either overlap (src>dst) or they don't overlap */
    /* do a forward copy */
    for ( ; len>0; len--, src++, dst++) *dst = *src;
  }
}
    

/***************************************************/
void xvbzero(s, len)
     char   *s;
     size_t  len;
{
  for ( ; len>0; len--) *s++ = 0;
}


/***************************************************/
FILE *xv_fopen(fname, mode)
     char *fname, *mode;
{
  FILE *fp;

#ifndef VMS
  fp = fopen(fname, mode);
#else
  fp = fopen(fname, mode, "ctx=stm");
#endif

  return fp;
}


#ifdef __cplusplus
}
#endif







