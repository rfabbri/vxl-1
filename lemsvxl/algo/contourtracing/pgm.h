/************************************************************************
 *                  *
 *       Copyright 19xx, Brown University, Providence, RI    *
 *                  *
 *  Permission to use and modify this software and its documentation  *
 *  for any purpose other than its incorporation into a commercial  *
 *  product is hereby granted without fee. Recipient agrees not to  *
 *  re-distribute this software or any modifications of this    *
 *  software without the permission of Brown University. Brown    *
 *  University makes no representations or warrantees about the    *
 *  suitability of this software for any purpose.  It is provided  *
 *  "as is" without express or implied warranty. Brown University  *
 *  requests notification of any modifications to this software or  *
 *  its documentation. Notice should be sent to:      *
 *                    *
 *  To:                  *
 *        Software Librarian            *
 *        Laboratory for Engineering Man/Machine Systems,    *
 *        Division of Engineering, Box D,        *
 *        Brown University            *
 *        Providence, RI 02912            *
 *        (401) 863-2118            *
 *        Software_Librarian@lems.brown.edu        *
 *                    *
 *  We will acknowledge all electronic notifications.      *
 *                   *
 ************************************************************************/

/***
   NAME
     pgm
   PURPOSE
     
   NOTES
     
   HISTORY
     tek - Jan 30, 1997: Created.
***/

#ifndef _PGM_H_
#define _PGM_H_

typedef struct {
  char *type;
  int h,w,maxval;
  unsigned char *im;
}PgmImage;


void read_pgmimage(char *file, PgmImage *pgmim);
void write_pgmimage(char *file, PgmImage *pgmim);
void InitPgm(PgmImage *pgmim);

#endif /* _PGM_H_  */
