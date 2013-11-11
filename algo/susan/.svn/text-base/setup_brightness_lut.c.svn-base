#include "setup_brightness_lut.h"

void setup_brightness_lut(bp,thresh,form)
  uchar **bp;
  int   thresh, form;
{
int   k;
float temp;

  *bp=(unsigned char *)malloc(516);
  *bp=*bp+258;  /* bp points to the middle */

  for(k=-256;k<257;k++)
  {
    temp=((float)k)/((float)thresh);
    temp=temp*temp;
    if (form==6)
      temp=temp*temp*temp;
    temp=100.0*exp(-temp);   /* 0 <= temp <= 100 */
    *(*bp+k)= (uchar)temp;
  }
}
