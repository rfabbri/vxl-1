/* $Id: mesh.cpp,v 1.4 2007-02-28 16:28:03 ntrinh Exp $ */


/*
 *
 *  Copyright (C) 2001-2004 EPFL (Swiss Federal Institute of Technology,
 *  Lausanne) This program is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 *  USA.
 *
 *  In addition, as a special exception, EPFL gives permission to link
 *  the code of this program with the Qt non-commercial edition library
 *  (or with modified versions of Qt non-commercial edition that use the
 *  same license as Qt non-commercial edition), and distribute linked
 *  combinations including the two.  You must obey the GNU General
 *  Public License in all respects for all of the code used other than
 *  Qt non-commercial edition.  If you modify this file, you may extend
 *  this exception to your version of the file, but you are not
 *  obligated to do so.  If you do not wish to do so, delete this
 *  exception statement from your version.
 *
 *  Authors : Nicolas Aspert, Diego Santa-Cruz and Davy Jacquet
 *
 *  Web site : http://mesh.epfl.ch
 *
 *  Reference :
 *   "MESH : Measuring Errors between Surfaces using the Hausdorff distance"
 *   in Proceedings of IEEE Intl. Conf. on Multimedia and Expo (ICME) 2002, 
 *   vol. I, pp. 705-708, available on http://mesh.epfl.ch
 *
 */




#include <stdio.h>
#include <math.h>
//#include <strings.h>
#include <ply.h>

#include <colormap.h>

#include <time.h>
#include <string.h>
//#include <qapplication.h>
//#include <qprogressdialog.h>
//#include <qpixmap.h>
#include <mesh.h>
//#include <ScreenWidget.h>
//#include <InitWidget.h>
#include <mesh_run.h>
#include <3dmodel.h>

#ifndef _MESHICON_XPM
# define _MESHICON_XPM
//# include <meshIcon.xpm>
#endif

/* The version string, also parsed by Makefile */
const char *version = "1.13";

/* The Copyright */
const char *copyright = "2001-2004 EPFL";

static void print_version(FILE *out) {
  fprintf(out,"MESH: Measuring Error between Surfaces using the "
          "Hausdorff distance\n");
  fprintf(out,"MESH version %s\n",version);
  fprintf(out,"\n");
  fprintf(out,"Copyright (C) %s\n",copyright);
  fprintf(out,
          "This is free software; see the source for copying conditions.\n");
  fprintf(out,
          "There is NO warranty; not even for MERCHANTABILITY or FITNESS "); 
  fprintf(out,"FOR A\nPARTICULAR PURPOSE.\n\n");
  fprintf(out,"Report bugs to <Nicolas.Aspert@epfl.ch> and ");
  fprintf(out,"<Diego.SantaCruz@epfl.ch>\n");
  fprintf(out,"\n");
}

/* Prints usage information to the out stream */
static void print_usage(FILE *out)
{
  print_version(out);
  fprintf(out,"Usage: mesh [[options] file1 file2]\n");
  fprintf(out,"\n");
  fprintf(out,"The program measures the distance from the 3D model in\n");
  fprintf(out,"file1 to the one in file2. The models must be given as\n");
  fprintf(out,"triangular meshes in RAW, VRML2, Inventor 2.x, PLY ASCII,\n");
  fprintf(out,"PLY binary, and SMF formats. Normals are supported in RAW\n");
  fprintf(out,"and VRML2 formats. It also reads gzipped files directly.\n");
  fprintf(out,"The VRML parser reads IndexedFaceSets nodes only,\n");
  fprintf(out,"ignoring all transformations, and does not support USE tags\n");
  fprintf(out,"(DEF tags are ignored). Likewise the Inventor 2.x reader is\n");
  fprintf(out,"somewhat limited. The file type is autodetected.\n");
  fprintf(out,"After the distance is calculated the result is displayed\n");
  fprintf(out,"as overall measures in text form and as a detailed distance\n");
  fprintf(out,"map in graphical form.\n");
  fprintf(out,"If no options nor filenames are given a dialog is shown\n");
  fprintf(out,"to select the input file names as well as the parameters.\n");
  fprintf(out,"\n");
  fprintf(out,"Display:\n");
  fprintf(out,"After calculating the distance, the error distribution on\n");
  fprintf(out,"model 1 is displayed in graphical form. There are three\n");
  fprintf(out,"modes to display the error distribution: vertex error, face\n");
  fprintf(out,"mean error and sample error.\n");
  fprintf(out,"In vertex error, each vertex is assigned a color based on\n");
  fprintf(out,"the calculated error at that vertex (if not calculated at\n");
  fprintf(out,"that vertex, dark gray is used) and the color is\n");
  fprintf(out,"interpolated between vertices. Invisible vertices are added\n");
  fprintf(out,
          "as necessary to display error at sample points which are not\n");
  fprintf(out,"vertices of model 1. The ratio between the total number of\n");
  fprintf(out,"error samples and error samples displayed is controlled by\n");
  fprintf(out,"the \"vertex error downsampling\" parameter. For low\n");
  fprintf(out,"downsampling values it can be very accurate, but it can use\n");
  fprintf(out,"a large number of triangles. For high downsampling values\n");
  fprintf(out,"the result can be misleading, although very fast.\n");
  fprintf(out,"In mean face error, each triangle is assigned a color based\n");
  fprintf(out,
          "on the mean error for that face (if that face has no samples\n");
  fprintf(out,"a dark gray is used).\n");
  fprintf(out,"In sample error mode  a color is assigned to each sample\n");
  fprintf(out,"point based on the error at that point, and  applied to the\n");
  fprintf(out,"model using texture mapping (if a face has no sample points\n");
  fprintf(out,"a dark gray is assigned to it). This mode is as accurate as\n");
  fprintf(out,"the vertex error mode with a downsampling value of 1,\n");
  fprintf(out,"although it can be very slow if model 1 has a large number\n");
  fprintf(out,"of triangles. If model 1 has few triangles and there is a\n");
  fprintf(out,
          "large number of samples this mode can be considerably faster\n");
  fprintf(out,"than vertex error mode with a low downsampling value.\n");
  fprintf(out,"A colorbar showing the correspondance between error values\n");
  fprintf(out,"and color is displayed. The colorbar also shows the\n");
  fprintf(out,"approximate distribution, on the surface of model 1, of the\n");
  fprintf(out,"error values using a histogram (approximate because the\n");
  fprintf(out,"distribution of the sample points on the surface is not\n");
  fprintf(out,"truly uniform).\n");
  fprintf(out,"Note that the -mf flag can force a minimum number of samples\n");
  fprintf(out,"per face.\n");
  fprintf(out,"\n");
  fprintf(out,"options:");
  fprintf(out,"\n");
  fprintf(out,"  -h\tDisplays this help message and exits.\n");
  fprintf(out,"\n");
  fprintf(out,"  -v\tDisplays version number, legal info and exits.\n");
  fprintf(out,"\n");
  fprintf(out,"  -s\tCalculate a symmetric distance measure. It calculates\n");
  fprintf(out,"    \tthe distance in the two directions and uses the max\n");
  fprintf(out,"    \tas the symmetric distance (Hausdorff distance).\n");
  fprintf(out,"\n");
  fprintf(out,"  -q\tQuiet, do not print progress meter.\n");
  fprintf(out,"\n");
  fprintf(out,"  -va\tBe verbose about non-manifold vertices during the\n");
  fprintf(out,"     \tmodel analysis phase.\n");
  fprintf(out,"\n");
  fprintf(out,"  -t\tDisplay only textual results, do not display the GUI.\n");
  fprintf(out,"\n");
  fprintf(out,
          "  -l s\tSet the sampling step to s, which is a percentage of\n");
  fprintf(out,"      \tthe bounding box diagonal of the second model. The\n");
  fprintf(out,"      \ttriangles of the first model are sampled, in order\n");
  fprintf(out,"      \tto calculate an approximation of the distance, so\n");
  fprintf(out,"      \tthat the sampling density (number of samples per\n");
  fprintf(out,
          "      \tunit surface) is 1/(s^2) (i.e. one sample per square\n");
  fprintf(out,"      \tof side length s). A probabilistic model is used so\n");
  fprintf(out,
          "      \tthat the resulting number is as close as possible to\n");
  fprintf(out,"      \tthe target. The default is 0.5\n\n");
  fprintf(out,"  -mf f\tEnsure that each triangle has a sampling frequency\n");
  fprintf(out,"       \tof at least f. Normally the sampling frequency for\n");
  fprintf(out,"       \ta triangle is determined by the sampling step and\n");
  fprintf(out,"       \ttriangle size. For some combinations this can lead\n");
  fprintf(out,"       \tto some triangles having no or few samples. With a\n");
  fprintf(out,"       \tnon-zero f parameter this can be avoided, although\n");
  fprintf(out,"       \tit disturbs the uniform distribution of samples.\n");
  fprintf(out,"       \tWith f set to 1, all triangles get at least one\n");
  fprintf(out,
          "       \tsample. With f set to 2, all triangles get at least\n");
  fprintf(out,"       \tthree samples, and thus all vertices get a sample.\n");
  fprintf(out,"       \tHigher values of f are less useful. By default it\n");
  fprintf(out,"       \tis zero in non-GUI mode and two in GUI mode.\n\n");
  fprintf(out,"  -wlog\tDisplay textual results in a window instead of on\n");
  fprintf(out,
          "       \tstandard output. Not compatible with the -t option.\n\n");
  fprintf(out,"  -tex\tEnables the display of the error by texture mapping\n");
  fprintf(out,"      \ton the model. \n");
  fprintf(out,"      \tWARNING : handle with care. Some platforms will \n");
  fprintf(out,"      \tfail to display this, and using this on big models\n");
  fprintf(out,"      \twill probably crash your computer with a\n");
  fprintf(out,"      \tswap storm. Not compatible with the -t option.\n");
  fprintf(out,"\n");
}

/* Initializes *pargs to default values and parses the command line arguments
 * in argc,argv. */
static void parse_args(int argc, char **argv, struct args *pargs)
{
  char *endptr;
  int i;

  memset(pargs,0,sizeof(*pargs));
  pargs->sampling_step = 0.5;
  pargs->min_sample_freq = -1;
  i = 1;
  while (i < argc) {
    if (argv[i][0] == '-') { /* Option */
      if (strcmp(argv[i],"-h") == 0) { /* help */
        print_usage(stdout);
        exit(0);
      } else if (strcmp(argv[i],"-v") == 0) { /* Version */
        print_version(stdout);
        exit(0);
      } else if (strcmp(argv[i],"-t") == 0) { /* text only */
        pargs->no_gui = 1;
      } else if (strcmp(argv[i],"-q") == 0) { /* quiet */
        pargs->quiet = 1;
      } else if (strcmp(argv[i],"-va") == 0) { /* verbose analysis */
        pargs->verb_analysis = 1;
      } else if (strcmp(argv[i],"-s") == 0) { /* symmetric distance */
        pargs->do_symmetric = 1;
      } else if (strcmp(argv[i],"-l") == 0) { /* sampling step */
        if (argc <= i+1) {
          fprintf(stderr,"ERROR: missing argument for -l option\n");
          exit(1);
        }
        pargs->sampling_step = strtod(argv[++i],&endptr);
        if (argv[i][0] == '\0' || *endptr != '\0' ||
            pargs->sampling_step <= 0) {
          fprintf(stderr,"ERROR: invalid number for -l option\n");
          exit(1);
        }
      } else if (strcmp(argv[i], "-mf") == 0) { /* sample all triangles */
        if (argc <= i+1) {
          fprintf(stderr,"ERROR: missing argument for -mf option\n");
          exit(1);
        }
        pargs->min_sample_freq = strtol(argv[++i],&endptr,10);
        if (argv[i][0] == '\0' || *endptr != '\0' ||
            pargs->min_sample_freq < 0) {
          fprintf(stderr,"ERROR: invalid number for -mf option\n");
          exit(1);
        }
      } else if (strcmp(argv[i], "-wlog") == 0) { /* log into window */
  pargs->do_wlog = 1;
      } else if (strcmp(argv[i], "-tex") == 0) { /* enable textures */
        pargs->do_texture = 1;
      } 
      else { /* unrecognized option */
        fprintf(stderr,
                "ERROR: unknown option in command line, use -h for help\n");
        exit(1);
      }
    } else { /* file name */
      if (pargs->m1_fname == NULL) {
        pargs->m1_fname = argv[i];
      } else if (pargs->m2_fname == NULL) {
        pargs->m2_fname = argv[i];
      } else {
        fprintf(stderr,
                "ERROR: too many arguments in command line, use -h for help\n");
        exit(1);
      }
    }
    i++; /* next argument */
  }
  if (pargs->no_gui && pargs->do_wlog) {
    fprintf(stderr, "ERROR: incompatible options -t and -wlog\n");
    exit(1);
  }
  if (pargs->no_gui && pargs->do_texture) {
    fprintf(stderr, "ERROR: incompatible options -t and -tex\n");
    exit(1);
  }
  if (pargs->min_sample_freq < 0) {
    pargs->min_sample_freq = (pargs->no_gui) ? 1 : 2;
  }
  pargs->sampling_step /= 100; /* convert percent to fraction */
}

/*****************************************************************************/
/*            'main' starts here.....                                        */
/*****************************************************************************/
int write_ply_file(model_error* err, const char* filename);

int main( int argc, char **argv )
{

  int i;
  struct args pargs;
  struct model_error model1,model2;
  int rcode;
  struct outbuf *log;
  struct prog_reporter pr;
  
  fprintf(stdout, "MESH : compute distance between distance between two surfaces\n");
  //fprintf(stdout, "Press any key to continue ...");

  //getchar();

  memset(&model1,0,sizeof(model1));
  memset(&model2,0,sizeof(model2));
  memset(&pr,0,sizeof(pr));
  log = NULL;
  i = 0;
  while (i<argc) {
    if (strcmp(argv[i],"-t") == 0) /* text version requested */
      break; 
    if (strcmp(argv[i],"-h") == 0) /* just asked for command line help */
      break; 
    i++;
  }

  /* Parse arguments */
  parse_args(argc,argv,&pargs);




  /* Display starting dialog if insufficient arguments */
  if (pargs.m1_fname != NULL || pargs.m2_fname != NULL) {
    if (pargs.m1_fname == NULL || pargs.m2_fname == NULL) {
      fprintf(stderr,"ERROR: missing file name(s) in command line\n");
      exit(1);
    }

    //
    if (!pargs.do_wlog) {
      log = outbuf_new(stdio_puts,stdout);
    }
    else {
      exit(1);
    }

    
    if (pargs.no_gui) {
      pr.prog = stdio_prog;
      pr.cb_out = stdout;
    } 
    
    else {
      // added by Nhon
      exit(1);
    }

    mesh_run(&pargs, &model1, &model2, log, &pr);
  } 
  
  else {
    fprintf(stderr, "insufficient arguments. Quit now.\n");
    exit(1);
  }
    rcode = 0;

  

    // write model to file, attaching color info to it
    char filename[250];
    strcpy(filename, pargs.m1_fname);
    char *pos = strrchr(filename, '.');
    strcpy(pos, "_wcolor.ply");
    fprintf(stdout, "\n\nfilename=%s\n", filename);
    write_ply_file(&model1, filename);
   
  /* Free model data */
  if (model1.mesh != NULL) __free_raw_model(model1.mesh);
  free(model1.verror);
  free(model1.info);
  free_face_error(model1.fe);
  if (model2.mesh != NULL) __free_raw_model(model2.mesh);
  free(model2.verror);
  free(model2.info);
  /* Return exit code */
  return rcode;
}


 
// write model to file, attaching color info.
int write_ply_file(model_error* me, const char* filename)
{

  
  model* mesh = me->mesh;

  // calculate error at vertices
  //int nv_empty;
  //int nf_empty;
  //calc_vertex_error(me, &nv_empty, &nf_empty);
  
  
  float* verror = me->verror;


  //
  if (me->verror == NULL)
  {
    fprintf(stdout, "vertex error map not available. Quit now.\n");
    return 0;
  }
  
  int nelems = 2;
  /* list of the kinds of elements in the user's object */
  char *elem_names[] = {"vertex", "face"};
  int nverts = mesh->num_vert;
  int nfaces = mesh->num_faces;

  int len = 200;

  float range_min = 0; //(float)err->min_error; //-3;
  float range_max = 6; // (float)err->max_error; //3;
  float range = range_max - range_min;


  int i;
  float** cmap_float = colormap_hsv(len);
  fprintf(stdout, "Colormap has len=%d\n", len);

  // print out colormap to a file
  char colorfile[250];
  strcpy(colorfile, filename);
  char *pos = strrchr(colorfile, '.');
  strcpy(pos, "_colormap.txt");
  fprintf(stdout, "Writing colormap to filename=%s ...", colorfile);
  FILE* fp_color = fopen(colorfile, "w");

  for(i=0; i<len; ++i)
  {
    cmap_float[i][0] = floorf(cmap_float[i][0] * 255);
    cmap_float[i][1] = floorf(cmap_float[i][1] * 255);
    cmap_float[i][2] = floorf(cmap_float[i][2] * 255);

    float dist = range_min + (i*range)/len;  
    fprintf(fp_color, "%.0f %.0f %.0f %.3f\n", cmap_float[i][0], cmap_float[i][1], cmap_float[i][2], dist);
  }
  fclose(fp_color);
  fprintf(stdout, " done.\n");

  // print out face properties to a separate file
  // in case something need to be done

  // print out colormap to a file
  char facefile[250];
  strcpy(facefile, filename);
  pos = strrchr(facefile, '.');
  strcpy(pos, "_faceinfo.txt");
  fprintf(stdout, "Writing colormap to filename=%s ...", facefile);
  FILE* fp_face = fopen(facefile, "w");

  fprintf(fp_face, "%s %s %s %s %s\n", "#face_area", "#min_error", "#max_error", "#mean_error", "#mean_sqr_error");
  struct face_error* fe = me->fe;
  for(i=0; i<me->mesh->num_faces; ++i)
  {
    fprintf(fp_face, "%f %f %f %f %f\n", fe[i].face_area, fe[i].min_error, fe[i].max_error, fe[i].mean_error, fe[i].mean_sqr_error);
  }
  fclose(fp_face);
  fprintf(stdout, " done.\n");

  PlyFile *ply;
  //int num_elem_types;


  typedef struct Vertex {
  float x,y,z;
  unsigned char red, green, blue;
  float verror_abs;
} Vertex;

typedef struct Face {
  unsigned char nverts;    /* number of vertex indices in list */
  int *verts;              /* vertex index list */
} Face;

  /////* list of property information for a vertex */
  //PlyProperty vert_props[] = { 
  //{"x", Float32, Float32, offsetof(Vertex,x), 0, 0, 0, 0},
  //{"y", Float32, Float32, offsetof(Vertex,y), 0, 0, 0, 0},
  //{"z", Float32, Float32, offsetof(Vertex,z), 0, 0, 0, 0}};


  ///* list of property information for a vertex */
  PlyProperty vert_props[] = { 
  {"x", Float32, Float32, offsetof(Vertex,x), 0, 0, 0, 0},
  {"y", Float32, Float32, offsetof(Vertex,y), 0, 0, 0, 0},
  {"z", Float32, Float32, offsetof(Vertex,z), 0, 0, 0, 0},
  {"red", Uint8, Uint8, offsetof(Vertex,red), 0, 0, 0, 0},
  {"green", Uint8, Uint8, offsetof(Vertex,green), 0, 0, 0, 0},
  {"blue", Uint8, Uint8, offsetof(Vertex,blue), 0, 0, 0, 0},
  {"verror_abs", Float32, Float32, offsetof(Vertex,verror_abs), 0, 0, 0, 0},
};


  PlyProperty face_props[] = { /* list of property information for a face */
  {"vertex_indices", Int32, Int32, offsetof(Face,verts),
   1, Uint8, Uint8, offsetof(Face,nverts)},
};


  /*** Write out the transformed PLY object ***/
 
  fprintf(stdout, "Writing ply file with color info to filename=%s ...\n", filename);
  FILE* fp = fopen(filename, "w");

  // ply = write_ply (stdout, nelems, elem_names, PLY_ASCII);
  ply = write_ply (fp, nelems, elem_names, PLY_ASCII);

  /* describe what properties go into the vertex elements */

  describe_element_ply (ply, "vertex", nverts);
  describe_property_ply (ply, &vert_props[0]);
  describe_property_ply (ply, &vert_props[1]);
  describe_property_ply (ply, &vert_props[2]);
  describe_property_ply (ply, &vert_props[3]);
  describe_property_ply (ply, &vert_props[4]);
  describe_property_ply (ply, &vert_props[5]);

  // vertex error
  describe_property_ply (ply, &vert_props[6]);

  describe_element_ply (ply, "face", nfaces);
  describe_property_ply (ply, &face_props[0]);

  append_comment_ply (ply, "created by Nhon Trinh - write_ply_file()");

  header_complete_ply (ply);

  // set up vertex list
  Vertex* vlist;
  vlist = (Vertex *) malloc (sizeof (Vertex) * nverts);
  vertex_t *vertices = mesh->vertices;
  

  float max_temp = -1e5;
  float min_temp = 1e5;
  int max_color_index = -1;
  for (i=0; i<nverts; ++i)
  {
    vlist[i].x = vertices[i].x;
    vlist[i].y = vertices[i].y;
    vlist[i].z = vertices[i].z;

    // check max and min error
    max_temp = (max_temp < verror[i]) ? verror[i] : max_temp;
    min_temp = (min_temp > verror[i]) ? verror[i] : min_temp;


    // compute color of vertex

    // color has range [0, 1]
    float color = (verror[i]-range_min)/range;
    color = (color > 1) ? 1 : color;
    color = (color < 0) ? 0 : color;

    // color_index has range [0, len-1]
    int color_index = (int)floorf(color*(len-1));

    max_color_index = (max_color_index < color_index) ? color_index : max_color_index;

    // color
    unsigned char red, green, blue;
    red = (unsigned char) (cmap_float[color_index][0]);
    green = (unsigned char) (cmap_float[color_index][1]);
    blue = (unsigned char) (cmap_float[color_index][2]);

    vlist[i].red = red;
    vlist[i].green = green;
    vlist[i].blue = blue;

    // distance to the other surface
    vlist[i].verror_abs = verror[i];
  }

  fprintf(stdout, "Max-error on vertices= %.3f\n", max_temp);
  fprintf(stdout, "Min-error on vertices= %.3f\n", min_temp);
  fprintf(stdout, "Max color index on vertices= %d\n", max_color_index);  

  /* set up and write the vertex elements */
  put_element_setup_ply (ply, "vertex");
  for (i = 0; i < nverts; i++)
    put_element_ply (ply, (void *) &vlist[i]);


  // set up face list
  Face *flist;
  flist = (Face *) malloc (sizeof (Face) * nfaces);

  face_t *faces = mesh->faces;
  for (i=0; i<nfaces; ++i)
  {
    flist[i].nverts = 3;
    flist[i].verts = (int *) malloc (sizeof(int) * 3);
    flist[i].verts[0] = faces[i].f0;
    flist[i].verts[1] = faces[i].f1;
    flist[i].verts[2] = faces[i].f2;
  }


  /* set up and write the face elements */
  put_element_setup_ply (ply, "face");
  for (i = 0; i < nfaces; i++)
    put_element_ply (ply, (void *) &flist[i]);

  free(vlist);
  free(flist);

  free_colormap(cmap_float);

  close_ply (ply);
  free_ply (ply);

  fclose(fp);
  fprintf(stdout, " done.\n");
  return 1;
}
