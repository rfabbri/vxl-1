/******************************************************************************
* Author: Mike Rodehorst
* 
******************************************************************************/

/*
 * If RENDER_OFF is not defined, do not compile this file AT ALL.
 */
#ifndef RENDER_OFF

/*****************************************************************************/
/* Libraries */

#include <GL/glut.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <cmath>

#include <iostream>
using namespace std;

#include "utility.h"
#include "surface_tracer.h"

/*****************************************************************************/
/* Global Variables */

static vector<double> vertex_list;
static vector<unsigned int> poly_list;
static vector<double> normal_list;
static unsigned int file_dimensions[NUM_DIMENSIONS];
static vector<EnoPoint> eno_array;

#ifndef RENDER_FLAT
static double * vertex_normal_list = 0; // dynamically allocated array
#endif

static int last_poly;

static double translate_global_x;
static double translate_global_y;
static double translate_global_z;

static float light0_position[4];
static float light1_position[4];
static float light0_linear_attenuation = 1.0;

const float light0_diffuse[4] = {1.0, 1.0, 1.0, 1.0};
const float light0_ambient[4] = {1.0, 1.0, 1.0, 1.0};
const float light0_specular[4] = {1.0, 1.0, 1.0, 1.0};
const float light1_diffuse[4] = {-0.5, -0.5, -0.5, -0.5};
//const float light1_ambient[4] = {-1.0, -1.0, -1.0, 1.0};
const float light1_specular[4] = {-0.5, -0.5, -0.5, -0.5};

const float surface_diffuse[4] = {0.2, 0.2, 0.6, 1.0};//{0.4, 0.4, 0.4, 1.0};
const float surface_ambient[4] = {0.1, 0.1, 0.2, 1.0};//{0.1, 0.1, 0.1, 1.0};
const float surface_specular[4] = {0.3, 0.3, 0.3, 1.0};//{0.5, 0.5, 0.5, 1.0};
const float surface_shininess = 10.0;

const float black_color[4] = {0.0, 0.0, 0.0, 1.0};

const unsigned int window_width = 900;
const unsigned int window_height = 900;

/*****************************************************************************/
/* Function Prototypes for just this file */

void Init();
//void Display();
void MouseFunction(int button, int state, int x, int y);
void KeyboardFunction(unsigned char key, int x, int y);
void RenderSurface();

void MotionFunction(int x, int y) { MouseFunction(-1,-1,x,y); }

#ifndef RENDER_FLAT
void GenerateVertexNormals(const vector<unsigned int> & polygons,
                           const vector<double> & poly_normals,
                           const vector<double> & vertex_coords,
                           const unsigned int num_vertices,
                           double * vertex_normals);
#endif

#ifdef WRITE_PPM_FRAMES
void WritePPMFile(const char * out_file_name);
#endif
#ifdef DEBUG_RENDER
void DebugRender();
#endif

/*****************************************************************************/

/*
 * Declare initial window size, position, and display mode
 * (single buffer and RGBA).  Open window with "Terrain"
 * in its title bar.  Call initialization routines.
 * Register callback function to display graphics.
 * Enter main loop and process events.
 */

int main(int argc, char ** argv)
{
  glutInit(&argc, argv);
#ifdef DOUBLE_BUFFER
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
#else
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
#endif
  glutInitWindowSize(window_width, window_height);
  glutInitWindowPosition(100, 0);
  glutCreateWindow ("Surface Tracer");
  
  SurfaceTracerIncrement(vertex_list,
                         eno_array,
                         poly_list,
                         normal_list,
                         file_dimensions);
  
  //cout << "Surface Tracer Successful" << endl;
  
  Init();
  //cout << "OpenGL Init Successful" << endl;
  
#ifdef DEBUG_RENDER
  glutDisplayFunc(DebugRender);
#else
  glutDisplayFunc(RenderSurface);
#endif
  // glutReshapeFunc(reshape);
  glutMouseFunc(MouseFunction);
  glutMotionFunc(MotionFunction);
  glutKeyboardFunc(KeyboardFunction);
  // glutIdleFunc(spinDisplay);

  //cout << "Starting OpenGL Main Loop" << endl;
  glutMainLoop();
  return 0;
}




/*****************************************************************************/

void Init()
{
  glClearColor (1.0, 1.0, 1.0, 1.0);
  
//#ifdef RENDER_FLAT
  glShadeModel(GL_FLAT);
  //glEnable(GL_NORMALIZE);
//#endif
  
  glClearDepth(1.0f);
  glEnable(GL_DEPTH_TEST); // Enables Depth Testing

  glDepthFunc(GL_LEQUAL);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective

  //glAlphaFunc(GL_GREATER, 0.5);
  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_BACK);

  // Lighting
  glEnable(GL_LIGHTING);
  glFrontFace(GL_CCW);
  
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light0_linear_attenuation);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);
  //glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
  //glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 0.2);
  glEnable(GL_LIGHT0);
  
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, light0_linear_attenuation);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
  glEnable(GL_LIGHT1);
  
  // Set base material properties for surface
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, black_color);
  glMaterialfv(GL_FRONT, GL_SPECULAR, surface_specular);
  glMaterialfv(GL_BACK, GL_SPECULAR, black_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, surface_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, surface_diffuse);
  glMaterialfv(GL_BACK, GL_DIFFUSE, black_color);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, surface_shininess);
  
  light0_position[DIM_X] = 0.0;//file_dimensions[DIM_X]*2.0;
  light0_position[DIM_Y] = 0.0;//file_dimensions[DIM_Y]*1.2;
  light0_position[DIM_Z] = 1.0;//file_dimensions[DIM_Z]*0.5;
  light0_position[3] = 0.0; // directional light, not positional
  
  light1_position[DIM_X] = -light0_position[DIM_X];
  light1_position[DIM_Y] = -light0_position[DIM_Y];
  light1_position[DIM_Z] = -light0_position[DIM_Z];
  light1_position[3] = 0.0; // directional light, not positional
  
  // Viewing Matrices
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.0,  // fovy angle
                 double(window_width)/double(window_height),   // Aspect ratio
                 0.1,   // Near plane distance
                 100.0*file_dimensions[DIM_X]); // Far plane distance
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  gluLookAt(file_dimensions[DIM_X]*0.5, // Eye coords -- position of viewpoint
            file_dimensions[DIM_Y]*2.0,
            file_dimensions[DIM_Z]*0.5,
            0.0, 0.0, 0.0, // Center coords -- a point along the line of sight
            0.0, 0.0, 1.0); // Up Vector
  
  translate_global_x = 0.0;
  translate_global_y = 0.0;
  translate_global_z = 0.0;
}




/*****************************************************************************/

void RenderSurface()
{
  //cout << "Rendering..." << endl;
  
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  unsigned int pnt;

#ifdef DEBUG13
  glDisable(GL_LIGHTING);
  glColor3d(0.0, 1.0, 0.0);
  glPointSize(1.0);
  glBegin(GL_POINTS);
  for(pnt = 0; pnt < vertex_list.size(); pnt+=3)
  {
    glVertex3dv(&vertex_list[pnt]);
  }
  glEnd();
  glEnable(GL_LIGHTING);
#endif

  //glTranslated(translate_global_x, translate_global_y, translate_global_z);
  
  // Light positions are not affected by viewing transformations.
  glPushMatrix();
  glLoadIdentity();
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light0_linear_attenuation);
  //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_spot_direction);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  glPopMatrix();
  
  unsigned int poly;
  
  // NOTE: #defining RENDER_FLAT causes polygons to always be rendered with
  //       their polygon normals, so the lighting makes the polygons distinct.
#ifndef RENDER_FLAT
  if(vertex_normal_list)
  {
    glDrawElements(GL_TRIANGLES, poly_list.size(), GL_UNSIGNED_INT, &*poly_list.begin());
  }
  else
  {
#endif
    glBegin(GL_TRIANGLES);
    for(poly = 0; poly < poly_list.size(); poly+=3)
    {
      glNormal3dv(&normal_list[poly]);
      glVertex3dv(&vertex_list[3*poly_list[poly]]);
      glVertex3dv(&vertex_list[3*poly_list[poly+1]]);
      glVertex3dv(&vertex_list[3*poly_list[poly+2]]);
    }
    glEnd();
#ifndef RENDER_FLAT
  }
#endif
  
  last_poly = poly;

  glDisable(GL_LIGHTING);
  //glDisable(GL_DEPTH_TEST);
#ifdef DEBUG13
  glColor3d(0.0, 0.0, 1.0);
  glPointSize(3.0);
#else
  glColor3d(0.0, 0.6, 0.0);
  glPointSize(1.0);
#endif
  glBegin(GL_POINTS);
  for(pnt = 0; pnt < vertex_list.size(); pnt+=3)
  {
    if(!eno_array[pnt/3].used)
    {
      glVertex3dv(&vertex_list[pnt]);
    }
  }
  glEnd();
#ifdef DEBUG18
  glColor3d(0.0, 0.0, 1.0);
  glLineWidth(1.0);
  glBegin(GL_LINES);
  
  const unsigned int db_box0[NUM_DIMENSIONS] = {48, 36, 59};
  const unsigned int db_box1[NUM_DIMENSIONS] = {49, 37, 60};
  
  glVertex3i(db_box0[DIM_X], db_box0[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box0[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box0[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box0[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box1[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box1[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box1[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box1[DIM_Y], db_box1[DIM_Z]);
  
  glVertex3i(db_box0[DIM_X], db_box0[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box1[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box0[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box1[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box0[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box1[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box0[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box1[DIM_Y], db_box1[DIM_Z]);
  
  glVertex3i(db_box0[DIM_X], db_box0[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box0[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box0[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box0[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box1[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box0[DIM_X], db_box1[DIM_Y], db_box1[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box1[DIM_Y], db_box0[DIM_Z]);
  glVertex3i(db_box1[DIM_X], db_box1[DIM_Y], db_box1[DIM_Z]);
  
  glEnd();
#endif
  //glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  
#ifdef DOUBLE_BUFFER
  glutSwapBuffers();
#else
  glFlush();
#endif
  //glutPostRedisplay();
}




/******************************************************************************/

void MouseFunction(int button, int state, int x, int y)
{
  static bool tracking = FALSE;
  static int tracking_button;
  static int start_x = 0;
  static int start_y = 0;
  GLdouble origin_winx, origin_winy, origin_winz,
           objx, objy, objz,
           start_objx, start_objy, start_objz;
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  
  if(!tracking)
  {
    if(state == GLUT_DOWN)
    {
      tracking = TRUE;
      tracking_button = button;
      start_x = x;
      start_y = y;
    }
  }
  else // if(state == GLUT_UP)
  {
    if((x == start_x) && (y == start_y))
    {
      // Mouse didn't actually move; assume no re-rendering is necessary.
      if(state == GLUT_UP)
        tracking = FALSE;
      return;
    }
    
    glGetIntegerv(GL_VIEWPORT, viewport);
    glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
    
    gluProject(double(file_dimensions[DIM_X]-1)/2.0,
               double(file_dimensions[DIM_Y]-1)/2.0,
               double(file_dimensions[DIM_Z]-1)/2.0,
               mvmatrix, projmatrix, viewport,
               &origin_winx, &origin_winy, &origin_winz);
    
    if(tracking_button == GLUT_LEFT_BUTTON)
    {
      /*
       * Translate
       */
      
      gluUnProject(GLdouble(x), GLdouble(viewport[3] - y), origin_winz,
                   mvmatrix, projmatrix, viewport, &objx, &objy, &objz);
      
      gluUnProject(GLdouble(start_x), GLdouble(viewport[3] - start_y), origin_winz,
                   mvmatrix, projmatrix, viewport,
                   &start_objx, &start_objy, &start_objz);
      
      translate_global_x += objx - start_objx;
      translate_global_y += objy - start_objy;
      translate_global_z += objz - start_objz;
      
      glMatrixMode(GL_MODELVIEW);
      glTranslated(objx - start_objx, objy - start_objy, objz - start_objz);
      
#ifdef DEBUG_TRANSLATE
      cout << "Origin Window Coordinates:" << endl;
      cout << origin_winx << "\t" << origin_winy << "\t" << origin_winz << endl;
      cout << "Start Window Coordinates:" << endl;
      cout << start_x << "\t" << start_y << "\t" << origin_winz << endl;
      cout << "Final Window Coordinates:" << endl;
      cout << x << "\t" << y << "\t" << origin_winz << endl;
      cout << "Start Object Coordinates:" << endl;
      cout << start_objx << "\t" << start_objy << "\t" << start_objz << endl;
      cout << "Final Object Coordinates:" << endl;
      cout << objx << "\t" << objy << "\t" << objz << endl;
      cout << "Translation Vector:" << endl;
      cout << translate_global_x << "\t" << translate_global_y << "\t" << translate_global_z << endl;
      cout << endl;
#endif
    }
    else if(tracking_button == GLUT_MIDDLE_BUTTON)
    {
      /*
       * Zoom In/Out
       */
      
      /**********
      double scale_factor = exp(2.0 * double(start_y - y) / 
                                (viewport[3] - viewport[1]));
      glMatrixMode(GL_MODELVIEW);
      // NOTE: Scaling de-normalizes the surface normals, affecting lighting.
      glScaled(scale_factor, scale_factor, scale_factor);
      light0_linear_attenuation /= scale_factor;
      **********/
      
      double zoom_factor = double(start_y - y) /
                           double(viewport[3] - viewport[1]) / 100.0;

#ifdef DEBUG_ZOOM
      cout << start_y - y << "\t"
           << viewport[3] - viewport[1] << "\t"
           << file_dimensions[DIM_X] << "\t"
           << zoom_factor << "\t" << endl;
#endif
      
      gluUnProject(GLdouble(viewport[0] + viewport[2])/2.0,
                   GLdouble(viewport[1] + viewport[3])/2.0, 1.0,
                   mvmatrix, projmatrix, viewport,
                   &start_objx, &start_objy, &start_objz);
      
      gluUnProject(GLdouble(viewport[0] + viewport[2])/2.0,
                   GLdouble(viewport[1] + viewport[3])/2.0, 0.0,
                   mvmatrix, projmatrix, viewport,
                   &objx, &objy, &objz);
      
      glTranslated((objx - start_objx)*zoom_factor,
                   (objy - start_objy)*zoom_factor,
                   (objz - start_objz)*zoom_factor);
    }
    else if(tracking_button == GLUT_RIGHT_BUTTON)
    {
      /*
       * Rotate
       */
      
      gluUnProject(GLdouble(start_x + y - start_y),
                   GLdouble(start_y + x - start_x), origin_winz,
                   mvmatrix, projmatrix, viewport,
                   &objx, &objy, &objz);
      
      gluUnProject(GLdouble(start_x), GLdouble(start_y), origin_winz,
                   mvmatrix, projmatrix, viewport,
                   &start_objx, &start_objy, &start_objz);
      
      double axis_objx = objx - start_objx;
      double axis_objy = objy - start_objy;
      double axis_objz = objz - start_objz;
      
      double theta = sqrt(double((x - start_x)*(x - start_x) +
                                 (y - start_y)*(y - start_y))) *
                     360 / (viewport[3] - viewport[0]);
      
      glMatrixMode(GL_MODELVIEW);
      /*
       * Perform Rotation so it turns around the center of the 3D space.
       * This requires a translation before and after to set the origin to the
       * middle of the space.
       */
      glTranslated(double(file_dimensions[DIM_X]-1)/2.0,
                   double(file_dimensions[DIM_Y]-1)/2.0,
                   double(file_dimensions[DIM_Z]-1)/2.0);
      glRotated(theta, axis_objx, axis_objy, axis_objz);
      glTranslated(-double(file_dimensions[DIM_X]-1)/2.0,
                   -double(file_dimensions[DIM_Y]-1)/2.0,
                   -double(file_dimensions[DIM_Z]-1)/2.0);
    }
    
    start_x = x;
    start_y = y;
    
    if(state == GLUT_UP)
      tracking = FALSE;
    
    glutPostRedisplay();
  }
}




/******************************************************************************/

void KeyboardFunction(unsigned char key, int x, int y)
{
  long int num_calls = 1;
  bool still_more;
  
  switch(key)
  {
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    num_calls = 1L<<long(key - '0'); // 2 to the atoi(key) power
  case ' ':
    still_more = TRUE;
    
#ifdef WRITE_PPM_FRAMES
    static int frame_count = 0;
    const char ppm_file_name_base[] = "brain-h-avg27-step%03d.ppm";
    const char gif_file_name_base[] = "brain-h-avg27-step%03d.gif";
    char ppm_file_name[MAX_STRING_LENGTH];
    sprintf(ppm_file_name, ppm_file_name_base, frame_count);
    printf("convert brain-h-avg27-step%03d.ppm brain-h-avg27-step%03d.gif\n", frame_count, frame_count);
    fflush(stdout);
    WritePPMFile(ppm_file_name);
    frame_count++;
#endif
    
    for(int call = 0; (call < num_calls) && still_more; call++)
    {
      still_more = SurfaceTracerIncrement(vertex_list,
                                          eno_array,
                                          poly_list,
                                          normal_list,
                                          file_dimensions);
    }

    if(!still_more)
    {
#ifndef RENDER_FLAT
      cout << "Generating Vertex Normals..." << endl;
      
      glShadeModel(GL_SMOOTH);
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_NORMAL_ARRAY);
      //glEnableClientState(GL_COLOR_ARRAY);
      
      vertex_normal_list = MakeVertexNormals(&*vertex_list.begin(),
                                             vertex_list.size()/3,
                                             &*poly_list.begin(),
                                             poly_list.size()/3,
                                             &*normal_list.begin());
      
      glVertexPointer(POINTS_PER_TRIANGLE, GL_DOUBLE, 0, &*vertex_list.begin());
      glNormalPointer(GL_DOUBLE, 0, vertex_normal_list);
      
#endif
      
      glutKeyboardFunc(0);
      //glEnable(GL_CULL_FACE);
      //glCullFace(GL_BACK);
      glutPostRedisplay();
    }
    
#ifdef WRITE_PPM_FRAMES
    glutPostRedisplay();
#else
    unsigned int poly;
    glBegin(GL_TRIANGLES);
    for(poly = last_poly; poly < poly_list.size(); poly+=3)
    {
      glNormal3dv(&normal_list[poly]);
      glVertex3dv(&vertex_list[3*poly_list[poly]]);
      glVertex3dv(&vertex_list[3*poly_list[poly+1]]);
      glVertex3dv(&vertex_list[3*poly_list[poly+2]]);
    }
    glEnd();
    last_poly = poly;
    
#ifdef DOUBLE_BUFFER
    glutSwapBuffers();
#else
    glFlush();
#endif
#endif
    break;
  case '\r':
  case '\n':
    // NOTE: While loop has no body; executes argument until it returns false.
    while(SurfaceTracerIncrement(vertex_list,
                                 eno_array,
                                 poly_list,
                                 normal_list,
                                 file_dimensions));
    
#ifndef RENDER_FLAT
    cout << "Generating Vertex Normals..." << endl;
    
    glShadeModel(GL_SMOOTH);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);
    
    vertex_normal_list = MakeVertexNormals(&*vertex_list.begin(),
                                           vertex_list.size()/3,
                                           &*poly_list.begin(),
                                           poly_list.size()/3,
                                           &*normal_list.begin());
    
    glVertexPointer(POINTS_PER_TRIANGLE, GL_DOUBLE, 0, &*vertex_list.begin());
    glNormalPointer(GL_DOUBLE, 0, vertex_normal_list);
    
#endif
    
    glutKeyboardFunc(0);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glutPostRedisplay();
    break;
  }
}




/******************************************************************************/

#ifdef WRITE_PPM_FRAMES

void WritePPMFile(const char * out_file_name)
{
   FILE * outfile;
   if ((outfile = fopen(out_file_name, "wb")) == NULL)
   {
      fprintf(stderr, "ERROR in WritePPMFile(): can't open %s\n", out_file_name);
      exit(1);
   }

   //                 "P3" for ASCII raster data
   //fprintf(outfile, "P6\r%d %d\r255\r",
   //        window_width, window_height);

   // Since Windows can't allow putting a carriage return by itself without
   // a line feed, I have to use this sneaky method to make the file readable
   // by all the readers I have.

   char header[100];
   sprintf(header, "P6 %4d %4d 255 ", window_width, window_height);

   // Replace these explicit spaces with line feeds
   header[2] = 0x0A;
   header[12] = 0x0A;
   header[16] = 0x0A;

   fwrite(header, sizeof(char), 17, outfile);

   //GLubyte image_buffer[window_width * window_height * 3];
   GLubyte image_buffer[window_width * 3];

   // Read all pixels at once
   // Doesn't work properly, on Windows at least.  Seems to insert extra bytes at end of each line.
   //glReadPixels(0, window_height - row, window_width, 1,
   //             GL_RGB, GL_UNSIGNED_BYTE, image_buffer);

   // GL starts in lower left corner, PPM starts in upper-left corner.
   // So, write in reverse row order.
   int row;
   for(row = 1; row <= window_height; row++)
   {
      // Read pixels one row at a time
      glReadPixels(0, window_height - row, window_width, 1,
                   GL_RGB, GL_UNSIGNED_BYTE, image_buffer);

      // If we read all the pixels into image_buffer at once:
      //fwrite(&image_buffer[(window_height-row)*window_width*3],
      //       window_width, sizeof(GLubyte), outfile);
      fwrite(image_buffer, window_width*3, sizeof(GLubyte), outfile);
   }
   fclose(outfile);
}

#endif


/*****************************************************************************/

#ifdef DEBUG_RENDER

/*****************************************************************************/

const unsigned int NUM_ENOS = 33;
static const unsigned int eno_indexes[NUM_ENOS] = {
         114,
         115,
         116,
         117,
         152,
         154,
         212,
         214,
         290,
       21687,
       21691,
       21937,
       21941,
       21945,
       22195,
       22199,
       22203,
       22207,
       22485,
       22489,
       22493,
       35022,
       35024,
       35026,
       35250,
       35252,
       35253,
       35254,
       35256,
       35260,
       35482,
       35484,
       35710
};

/*****************************************************************************/

void DebugRender()
{
  //cout << "Rendering..." << endl;
  
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  unsigned int pnt;

#ifdef DEBUG13
  glDisable(GL_LIGHTING);
  glColor3d(0.0, 1.0, 0.0);
  glPointSize(1.0);
  glBegin(GL_POINTS);
  for(pnt = 0; pnt < vertex_list.size(); pnt+=3)
  {
    glVertex3dv(&vertex_list[pnt]);
  }
  glEnd();
  glEnable(GL_LIGHTING);
#endif

  //glTranslated(translate_global_x, translate_global_y, translate_global_z);
  
  glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
  //glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light0_linear_attenuation);
  //glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_spot_direction);
  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  
  unsigned int poly;
  
  // NOTE: #defining RENDER_FLAT causes polygons to always be rendered with
  //       their polygon normals, so the lighting makes the polygons distinct.
#ifndef RENDER_FLAT
  if(vertex_normal_list)
  {
    glDrawElements(GL_TRIANGLES, poly_list.size(), GL_UNSIGNED_INT, poly_list.begin());
  }
  else
  {
#endif
    glBegin(GL_TRIANGLES);
    for(poly = 0; poly < poly_list.size(); poly+=3)
    {
      for(int p = 0; p < NUM_ENOS; p++)
      {
        if((poly_list[poly] == eno_indexes[p]) ||
           (poly_list[poly+1] == eno_indexes[p]) ||
           (poly_list[poly+2] == eno_indexes[p]))
        {
          glNormal3dv(&normal_list[poly]);
          glVertex3dv(&vertex_list[3*poly_list[poly]]);
          glVertex3dv(&vertex_list[3*poly_list[poly+1]]);
          glVertex3dv(&vertex_list[3*poly_list[poly+2]]);
        }
      }
    }
    glEnd();
#ifndef RENDER_FLAT
  }
#endif
  
  last_poly = poly;

  glDisable(GL_LIGHTING);
  //glDisable(GL_DEPTH_TEST);
#ifdef DEBUG13
  glColor3d(0.0, 0.0, 1.0);
  glPointSize(3.0);
#else
  glColor3d(0.0, 1.0, 0.0);
  glPointSize(3.0);
#endif
  glBegin(GL_POINTS);
  for(pnt = 0; pnt < 33; pnt++)
  {
    //if(!eno_array[pnt].used)
    //{
      glVertex3dv(&vertex_list[3*eno_indexes[pnt]]);
    //}
  }
  glEnd();
  glColor3d(0.0, 0.0, 1.0);
  glLineWidth(1.0);
  glBegin(GL_LINES);
  
  glVertex3i(-999999, 20, 53);
  glVertex3i( 999999, 20, 53);
  glVertex3i(-999999, 20, 54);
  glVertex3i( 999999, 20, 54);
  glVertex3i(-999999, 21, 53);
  glVertex3i( 999999, 21, 53);
  glVertex3i(-999999, 21, 54);
  glVertex3i( 999999, 21, 54);
  glVertex3i(-999999, 22, 53);
  glVertex3i( 999999, 22, 53);
  glVertex3i(-999999, 22, 54);
  glVertex3i( 999999, 22, 54);
  
  glVertex3i(40, -999999, 53);
  glVertex3i(40,  999999, 53);
  glVertex3i(41, -999999, 53);
  glVertex3i(41,  999999, 53);
  glVertex3i(40, -999999, 54);
  glVertex3i(40,  999999, 54);
  glVertex3i(41, -999999, 54);
  glVertex3i(41,  999999, 54);
  
  glVertex3i(40, 20, -999999);
  glVertex3i(40, 20,  999999);
  glVertex3i(40, 21, -999999);
  glVertex3i(40, 21,  999999);
  glVertex3i(40, 22, -999999);
  glVertex3i(40, 22,  999999);
  glVertex3i(41, 20, -999999);
  glVertex3i(41, 20,  999999);
  glVertex3i(41, 21, -999999);
  glVertex3i(41, 21,  999999);
  glVertex3i(41, 22, -999999);
  glVertex3i(41, 22,  999999);
  
  glEnd();
  //glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  
  glFlush();
  //glutSwapBuffers();
  //glutPostRedisplay();
}



#endif // DEBUG_RENDER

#endif // RENDER_OFF
