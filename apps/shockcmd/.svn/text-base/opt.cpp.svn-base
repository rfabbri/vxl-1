#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <vcl_string.h>

#include "opt.h"

static char *typestrings[] = {
  "void", "char", "string", "short", "int", "float", "double"};

void
opt_parse_args(int argc, char *argv[], OPTTABLE *table)
{
  OPTTABLE  *op = table;
  void    *val_addr = 0;
  OPT_TYPE  val_type;
  int    n, ap, found;
  float    x;
  char    *astr;

  for(ap=1; ap<argc; ap++) {
    astr = argv[ap];
    if (*astr == '-') {
      char *astr1 = astr+1;
      if(!strcmp(astr1, "?") ||
   !strcmp(astr1, "h") ||
   !strcmp(astr1, "H") ||
   !strcmp(astr1, "help"))
  opt_usage(-1, argc, argv, table);

      for(op=table, found=0; op->flag_string != 0 && ! found; op++) {
  if(!strcmp (astr+1, op->flag_string)) {
    val_addr = op->value_addr;
    val_type = op->value_type;
    if(op->present_p)
      *(op->present_p) = 1;
    found = 1;
    break;
  }
      }
      if(!found) {
  vcl_fprintf(stderr, "Unknown flag \"%s\"\n", argv[ap]);
  opt_usage (-1, argc, argv, table);
      }
      else if(val_addr != 0) {
  astr = argv[++ap];
  switch (val_type) {
  case _VOID:
    /* void does not take any values */
    opt_usage(ap, argc, argv, table);
    break;
  case _CHAR:
    /* One character argument is expected. */
    /* It is passed unchanged to the value address */
    if (*(astr+1) != 0)
      opt_usage(ap, argc, argv, table);
    else
      *((char*)op->value_addr) = *astr;
    break;
  case _STRING:
    *((char **)op->value_addr) = astr;
    break;
  case _SHORT:
    if (sscanf (argv[ap], "%d", &n) == 1)
      *((short*)op->value_addr) = n;
    else
      opt_usage(ap, argc, argv, table);
    break;
  case _INT:
    if (sscanf (argv[ap], "%d", &n) == 1)
      *((int*)op->value_addr) = n;
    else
      opt_usage(ap, argc, argv, table);
    break;
  case _FLOAT:
    if (sscanf (argv[ap], "%f", &x) == 1)
      *((float*)(op->value_addr)) = x;
    else
      opt_usage(ap, argc, argv, table);
    break;
  case _DOUBLE:
    if (sscanf (argv[ap], "%f", &x) == 1)
      *((double*)op->value_addr) = x;
    else
      opt_usage(ap, argc, argv, table);
    break;
  }
      }
      val_addr = 0;
    }
  }
}

#define LINELENGTH 79
#define INDENT 8

#ifdef unix
#define DIRDEL '/'
#else
#define DIRDEL '\\'
#endif

int
opt_usage(int argnum, int argc, char *argv[], OPTTABLE *tbl)
{
  char  *tstring = "Usage: ", *pname = argv[0], *short_pname = 0;
  int  col, blen, maxblen=0, offset = strlen(tstring)+1+strlen(pname);
  OPTTABLE  *op = tbl;
    
  short_pname = strrchr(pname, DIRDEL);
  if (short_pname == 0)
    short_pname = pname;
  else
    short_pname++;

  vcl_fprintf(stderr, "%s%s ", tstring, short_pname);
  col = offset;
  for(op=tbl; op->flag_string != 0; op++) {
    char buf[111];
    if (op->value_addr) {
      char *typestring = typestrings[op->value_type];
      sprintf (buf, "-%s <%s>  ", op->flag_string, typestring);
    }
    else
      sprintf (buf, "-%s  ", op->flag_string);
    blen = strlen (buf);
    if(blen > maxblen)
      maxblen = blen;
    if(col+blen >= LINELENGTH) {
      int i;
      vcl_fprintf (stderr, "\n");
      for (i=0; i<INDENT; i++)
  vcl_fprintf (stderr, " ");
      col = INDENT;
    }
    vcl_fprintf (stderr, buf);
    col += blen;
  }
  vcl_fprintf(stderr, "\n  where:\n");
  for(op=tbl; op->flag_string != 0; op++) {
    char buf[111];
    int i;
    if (op->value_addr) {
      char *typestring = typestrings[op->value_type];
      sprintf (buf, "-%s <%s>  ", op->flag_string, typestring);
    }
    else
      sprintf (buf, "-%s  ", op->flag_string);
    blen = strlen (buf);
  
    vcl_fprintf(stderr, "    %s", buf);
    for (i=blen; i<maxblen; i++)
      vcl_fprintf (stderr, " ");
    if (op->comment)
      vcl_fprintf (stderr, " -- %s\n", op->comment);
  }
  exit(1);
  return(1);
}

#ifdef OPTTEST
/******************************************************************/

int  aa_p, bb_p, cc_p, dd_p, ss_p, vv_p, str_p; 
float  aa;
int  bb;
char  cc;
double  dd;
short  ss;
char  *str;

OPTTABLE fake[] =
{
  {"a",     FLOAT,  &aa_p,   &aa, "No comment for aa"},
  {"bb",    INT,    &bb_p,   &bb, "No comment for bb"},
  {"ccc",   CHAR,   &cc_p,   &cc, "No comment for ccc"},
  {"dd",    DOUBLE, 0,   &dd, "No comment for dd"},
  {"s",     SHORT,  &ss_p,   &ss, "No comment for s"},
  {"v",     VOID,   &vv_p,   0,   "No comment for vv"},
  {"str",   STRING, &str_p,  &str,"No comment for str"},

  {0,0,0,0,0}
};

main (int argc, char *argv[])
{
  opt_parse_args(argc, argv, fake);
    
  printf ("aa_p=%d, bb_p=%d, cc_p=%d, dd_p=%d, ss_p=%d, vv_p=%d, str_p=%d\n",
    aa_p, bb_p, cc_p, dd_p, ss_p, vv_p, str_p);
    
  printf ("aa=%f, bb = %d, cc=%c, dd=%lf, ss=%d\n",
    aa, bb, cc, dd, ss);
  printf ("str: \"%s\"\n", str);
}
#endif
