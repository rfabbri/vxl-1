/************************************************************************
 *						*
 *       Copyright 1996, Brown University, Providence, RI		*
 *						*
 *  Permission to use and modify this software and its documentation	*
 *  for any purpose other than its incorporation into a commercial	*
 *  product is hereby granted without fee. Recipient agrees not to	*
 *  re-distribute this software or any modifications of this		*
 *  software without the permission of Brown University. Brown		*
 *  University makes no representations or warrantees about the	*
 *  suitability of this software for any purpose.  It is provided	*
 *  "as is" without express or implied warranty. Brown University	*
 *  requests notification of any modifications to this software or	*
 *  its documentation. Notice should be sent to:			*
 *  						*
 *  To:
 *        Software Librarian
 *        Laboratory for Engineering Man/Machine Systems,
 *        Division of Engineering, Box D,
 *        Brown University
 *        Providence, RI 02912
 *        (401) 863-2118
 *        Software_Librarian@lems.brown.edu
 *
 *  We will acknowledge all electronic notifications.
 *
 ************************************************************************/


#include <Python.h>
#include "import.h"
#include "arrayobject.h"

#include "1d_eno_types.h"

static PyObject *ErrorObject;

#define TRY(E) if(! (E)) return NULL
#define CONCAT(a,b)    a ## b
#define STRINGIFY(a)   #a

#define MODULE_NAME enoimage
#define MODULE_INIT_FUN_NAME CONCAT(init,enoimage)
#define MODULE_NAME_STRING   STRINGIFY(enoimage)

#define HORIZONTAL 0
#define VERTICAL 1

/* ----------------------------------------------------- */

/* Declarations for objects of type eno_interval_image */

typedef struct {
	PyObject_HEAD
	ENO_Interval_Image_t image;
} eiiobject;

staticforward PyTypeObject Eiitype;


/* ---------------------------------------------------------------- */
/* ---------------------------------------------------------------- */

static char eii_find_shocks__doc__[] = 
"Find all shocks in the enoimage and print out their info."
;

static PyObject *
eii_find_shocks(self, args)
      eiiobject *self;
      PyObject *args;
{
    int h,w;
    int xx,yy;
    int cnt = 0;

    TRY(PyArg_ParseTuple(args, ""));
    
    h = self->image.height;
    w = self->image.width;

    for(yy = 0; yy < h; yy++){
        for(xx = 0; xx < w ; xx++){
            if (self->image.horiz[yy * w + xx].shock){
                cnt++;
                fprintf(stdout,"horizontal(%d,%d) ",yy,xx);
                enoshock_print(self->image.horiz[yy * w + xx].shock,stdout);
            }
            if (self->image.vert[yy * w + xx].shock){
                cnt++;
                fprintf(stdout,"vertical(%d,%d) ",yy,xx);
                enoshock_print(self->image.vert[yy * w + xx].shock,stdout);
            }
        }
    }
    return Py_BuildValue("i",cnt);
}

static char eii_find_zerox__doc__[] = 
"Return a list of interval indicies of all zero crossings in the enoimage";

static PyObject *
eii_find_zerox(self, args)
      eiiobject *self;
      PyObject *args;
{
    int h,w;
    int xx,yy;
    int cnt = 0;
    int ret;
    PyObject *l=NULL,*v=NULL;

    TRY(PyArg_ParseTuple(args, ""));
    
    h = self->image.height;
    w = self->image.width;

    TRY(l = PyList_New(0));
    
    for(yy = 0; yy < h; yy++){
        for(xx = 0; xx < w ; xx++){
            if (self->image.horiz[yy * w + xx].zerox ||
                self->image.vert[yy * w + xx].zerox){
                v = Py_BuildValue("(ii)",yy,xx);
                if (v == NULL) goto FIND_ZEROX_ERR;
                ret = PyList_Append(l,v);
                if (ret) goto FIND_ZEROX_ERR;
            }
        }
    }
    return l;
  FIND_ZEROX_ERR:
    Py_XDECREF(v);
    Py_XDECREF(l);
    return NULL;
}

static PyObject *
eii_zeroxtuples_from_intervals(ENO_Interval_t *intervals,
			       int height, int width,
			       int dir)
{
    int cnt,yy,xx;
    double xloc,yloc;
    register ENO_Interval_t *iptr;
    PyObject *ret=NULL,*o=NULL;

    /* find how many zero crossings exist in the intervals */
    cnt = 0;
    iptr = intervals;
    for(yy = 0; yy < height; yy++){
        for(xx = 0; xx < width ; iptr++,xx++){
	  if (iptr->zerox){
	    cnt += iptr->zerox->cnt;
	  }
	}
    }
    if (cnt==0)
      return Py_BuildValue("()");

    TRY(ret = PyTuple_New(cnt));
    
    /* store find how many zero crossings exist in the intervals */
    cnt=0;
    iptr = intervals;
    for(yy = 0; yy < height; yy++){
        for(xx = 0; xx < width ; iptr++,xx++){
	  if (iptr->zerox){
	    int index;

	    /* one of these will be modified in the loop below */
	    yloc = yy; xloc = xx;

	    for(index=0; index < iptr->zerox->cnt; index++){
	      /* if horizontal, zerox.loc is the x coordinate. */
	      if (dir==HORIZONTAL){
		xloc = iptr->zerox->loc[index];
	      }
	      else{
		yloc = iptr->zerox->loc[index];
	      }
	      o = Py_BuildValue("(dd)",yloc,xloc);
	      if (o==NULL) {Py_DECREF(ret); return NULL;}
	      PyTuple_SET_ITEM(ret,cnt,o);
	      cnt++;
	    }
	  }
	}
    }
    
    return ret;
}
    
static char eii_find_zerox_coords__doc__[] = 

"Return h,v which are two tuples containing zero crossings of horizontal and vertical zero crossings in the enoimage. \n\
\n\
Each tuple is of the form ((y1,x1),...,(yn,xn)) where values are\n\
floating point coordinates of the zero crossings, one list for\n\
horizontal and one for vertical zero crossings.\n\
\n\
Note h or v may be None if there are no zero crossings.";

static PyObject *
eii_find_zerox_coords(self, args)
      eiiobject *self;
      PyObject *args;
{
  int h,w;
  PyObject *ret=NULL,*hobj=NULL,*vobj=NULL;

  TRY(PyArg_ParseTuple(args, ""));

  h = self->image.height;
  w = self->image.width;
  hobj = eii_zeroxtuples_from_intervals(self->image.horiz,h,w,HORIZONTAL);
  if (hobj)
    vobj = eii_zeroxtuples_from_intervals(self->image.vert,h,w,VERTICAL);
  
  if ( !hobj && !vobj){
    Py_XDECREF(hobj);
    Py_XDECREF(vobj);
    ret =  NULL;
  }else{
    ret =  Py_BuildValue("(OO)",hobj,vobj);
  }
  return ret;
}

static char eii_print_interval__doc__[] = 
"Print the information about the interval (y,x)."
;

static PyObject *
eii_print_interval(self, args)
      eiiobject *self;
      PyObject *args;
{
    int h,w;
    int x,y;

    TRY(PyArg_ParseTuple(args, "ii", &y,&x));
    h = self->image.height;
    w = self->image.width;
    if ((y < 0) || (y >= h)){
      PyErr_SetString(PyExc_ValueError,"y out of range for this enoimage");
      return NULL;
    }
    if ((x < 0) || (x >= w)){
      PyErr_SetString(PyExc_ValueError,"x out of range for this enoimage");
      return NULL;
    }

    fprintf(stdout,"horizontal interval:\n");
    enointerv_print(&self->image.horiz[y * w + x],stdout);
    
    fprintf(stdout,"vertical interval:\n");
    enointerv_print(&self->image.vert[y * w + x],stdout);

    Py_INCREF(Py_None);
    return Py_None;
}
static char eii_print_interp__doc__[] = 
"Print the interpolation information about the interval (y,x)."
;

static PyObject *
eii_print_interp(self, args)
      eiiobject *self;
      PyObject *args;
{
    int h,w;
    int x,y,tag;

    TRY(PyArg_ParseTuple(args, "iii", &y,&x,&tag));
    h = self->image.height;
    w = self->image.width;
    if ((y < 0) || (y >= h)){
      PyErr_SetString(PyExc_ValueError,"y out of range for this enoimage");
      return NULL;
    }
    if ((x < 0) || (x >= w)){
      PyErr_SetString(PyExc_ValueError,"x out of range for this enoimage");
      return NULL;
    }
    
    if (tag == 1){ /*Print Horizontal interval information */
      eno_just_interp_print(&self->image.horiz[y * w + x].interp,stdout);
    }
    else if (tag ==0){ /*Print Vertical interval information */
      eno_just_interp_print(&self->image.vert[y * w + x].interp,stdout);
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static char eii_compute_shocks__doc__[] = 
"Compute and add shocks to enoimage structure."
;

static PyObject *
eii_compute_shocks(self, args)
eiiobject *self;
PyObject *args;
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    enoimage_compute_shocks(&self->image);
    Py_INCREF(Py_None);
    return Py_None;
}

static char eii_compute_zerox__doc__[] = 
"Compute and add zero crossings (zerox) to enoimage structure."
;

static PyObject *
eii_compute_zerox(self, args)
eiiobject *self;
PyObject *args;
{
    if (!PyArg_ParseTuple(args, ""))
        return NULL;
    enoimage_compute_zerox(&self->image);
    Py_INCREF(Py_None);
    return Py_None;
}


static struct PyMethodDef eii_methods[] = {
  {"compute_shocks",eii_compute_shocks,	1,eii_compute_shocks__doc__},
  {"compute_zerox",eii_compute_zerox,	1,eii_compute_zerox__doc__},
  {"print_interval",eii_print_interval,	1,eii_print_interval__doc__},
  {"print_interp",eii_print_interp,	1,eii_print_interp__doc__},
  {"find_shocks",eii_find_shocks,1,eii_find_shocks__doc__},
  {"find_zerox",eii_find_zerox,1,eii_find_zerox__doc__},
  {"find_zerox_coords",eii_find_zerox_coords,1,eii_find_zerox_coords__doc__},
  {NULL,		NULL}		/* sentinel */
};

/* ---------- */


static eiiobject *
neweiiobject(int height,int width)
{
  eiiobject *self;
  
  self = PyObject_NEW(eiiobject, &Eiitype);
  if (self == NULL)
    return NULL;
  TRY(enoimage_new(&self->image,height,width));
  return self;
}


static void
eii_dealloc(self)
	eiiobject *self;
{
    enoimage_dealloc_data(&self->image);
    PyMem_DEL(self);
}

static PyObject *
eii_repr(self)
	eiiobject *self;
{
  PyObject *s;

  char msg[128];

  sprintf(msg,"<enoimage object at 0x%X, (height,width) = (%d,%d)>",
          self,self->image.height,self->image.width);
  s = PyString_FromString(msg);
  return s;
}

static PyObject *
eii_str(self)
	eiiobject *self;
{
  PyObject *s;
  char msg[128];

  sprintf(msg,"<enoimage object at 0x%X, (height,width) = (%d,%d)>",
          self,self->image.height,self->image.width);
  s = PyString_FromString(msg);
  return s;
}


/* Code to access structure members by accessing attributes */

#include "structmember.h"

#define OFF(x) offsetof(eiiobject, x)

static struct memberlist eii_memberlist[] = {
  { "height", T_INT, OFF(image.height), RO },
  { "width", T_INT, OFF(image.width), RO },
{NULL}	/* Sentinel */
};

static PyObject *
eii_getattr(self, name)
eiiobject *self;
char *name;
{
    PyObject *rv;
    
    rv = PyMember_Get((char *)self, eii_memberlist, name);
    if (rv)
        return rv;
    PyErr_Clear();
    return Py_FindMethod(eii_methods, (PyObject *)self, name);
}


static int
eii_setattr(self, name, v)
	eiiobject *self;
	char *name;
	PyObject *v;
{
  if ( v == NULL ) {
    PyErr_SetString(PyExc_AttributeError, "Cannot delete attribute");
    return -1;
  }
  return PyMember_Set((char *)&self->image, eii_memberlist, name, v);
}

static char Eiitype__doc__[] = 
""
;

static PyTypeObject Eiitype = {
  PyObject_HEAD_INIT(&PyType_Type)
  0,				/*ob_size*/
  "eno_interval_image",			/*tp_name*/
  sizeof(eiiobject),		/*tp_basicsize*/
  0,				/*tp_itemsize*/
  /* methods */
  (destructor)eii_dealloc,	/*tp_dealloc*/
  (printfunc)0,		/*tp_print*/
  (getattrfunc)eii_getattr,	/*tp_getattr*/
  (setattrfunc)eii_setattr,	/*tp_setattr*/
  (cmpfunc)0,		/*tp_compare*/
  (reprfunc)eii_repr,		/*tp_repr*/
  0,			/*tp_as_number*/
  0,		/*tp_as_sequence*/
  0,		/*tp_as_mapping*/
  (hashfunc)0,		/*tp_hash*/
  (ternaryfunc)0,		/*tp_call*/
  (reprfunc)eii_str,		/*tp_str*/

  /* Space for future expansion */
  0L,0L,0L,0L,
  Eiitype__doc__ /* Documentation string */
};

/* End of code for eno_interval_image objects */
/* -------------------------------------------------------- */


static char EI_NewENOImage__doc__[] =
"Create a new enoimage object of size height,width, computing \n\
the ENO polynomial interpolants."
;

static PyObject *
EI_NewENOImage(self, args)
	PyObject *self;	/* Not used */
	PyObject *args;
{
    PyObject *op;
    PyArrayObject *mp;
    eiiobject *ret;
    double *data;
    int h,w;
    int sz,err;
    
    TRY(PyArg_ParseTuple(args, "O", &op));
    TRY(mp = (PyArrayObject *)PyArray_ContiguousFromObject(op,PyArray_DOUBLE, 2, 2));
    h = mp->dimensions[0];
    w = mp->dimensions[1];

    data = (double *)mp->data;

    ret = neweiiobject(h,w);
    if (ret == NULL){
        Py_DECREF(mp);
        PyErr_NoMemory();
        return NULL;
    }
    enoimage_compute_interpolants(&ret->image,data);
    Py_DECREF(mp);
    return (PyObject *) ret;
}

/* List of methods defined in the module */

static struct PyMethodDef EI_methods[] = {
  {"newENOImage",	EI_NewENOImage,	1,	EI_NewENOImage__doc__},
 
  {NULL,		NULL}		/* sentinel */
};


/* Initialization function for the module (*must* be called initenoimage) */

static char enoimage_module_documentation[] = 
"Module for computing the ENO zero crossings of a surface."
;


void
MODULE_INIT_FUN_NAME()
{
  PyObject *m, *d;

  m = PyImport_ImportModule("multiarray");
  if (m == NULL){
      Py_FatalError("can't load multiarray module");
  }
  
  /* Create the module and add the functions */
  m = Py_InitModule4(MODULE_NAME_STRING, EI_methods,
		     enoimage_module_documentation,
		     (PyObject*)NULL,PYTHON_API_VERSION);

  /* Add some symbolic constants to the module */
  d = PyModule_GetDict(m);
  ErrorObject = PyString_FromString("enoimage.error");
  PyDict_SetItemString(d, "error", ErrorObject);

  fprintf(stderr,"loading enoimage using %s\n",ENOIMAGE_VERSION);
  /* Check for errors */
  if (PyErr_Occurred())
    Py_FatalError("can't initialize module enoimage");
}

