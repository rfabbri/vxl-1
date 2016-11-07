//-----------------------------------------------------
//Biliana Kaneva, Smith College
//cone.cpp
//
//This file contains the member functions of the Cone class
//
//display()
//-----------------------------------------------------

/*--------------------------------------------------------------------
This code is Copyright 2000 by Biliana Kaneva & Joseph O'Rourke.  
It may be freely redistributed in its entirety provided that this 
copyright notice is not removed.  Question to orourke@cs.smith.edu.
--------------------------------------------------------------------*/

#include "cone.h"

//-----------------------------------------------------
//dislay: prints out the cone data structure
//        used for debugging purposes
//-----------------------------------------------------
void Cone::display()
{
  vcl_cout << "%Cone info" << vcl_endl;
  vcl_cout << "%Face " << currface << " Gl_edge "<< gl_edge << vcl_endl;
  vcl_cout << "% Level " << level << vcl_endl;
  vcl_cout << "%Parent  LChild " << 
    ((l!=NULL)?l->currface:-1) <<" RChild " << 
    ((r!=NULL)?r->currface:-1) << vcl_endl;
  vcl_cout << "%" << f << vcl_endl;
  vcl_cout << "%Bounding vertices " << left << " " << right << vcl_endl;
  vcl_cout << "%Next " << ((next!=NULL)?next->currface:-1) 
       << " Previous "<< ((prev!=NULL)?prev->currface:-1) << vcl_endl;

  vcl_cout << vcl_endl;
}
