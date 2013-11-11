#ifndef I386_DEFINES_H
#define I386_DEFINES_H

//round towards nearest/even
// even means: 2.5->2, 3.5->4
static _inline void dtol (double d, long *a)
{
   _asm
   {
      mov eax, a
      fld qword ptr d
      fistp dword ptr [eax]
   }
}

#endif


