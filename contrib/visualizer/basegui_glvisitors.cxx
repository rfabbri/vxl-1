#include "basegui_xpgl.h"
#include "basegui_xpglu.h"
#include "basegui_glvisitors.h"
#include "basegui_xform.h"
#include "basegui_style.h"
#include "basegui_nodes.h"
#include "basegui_view.h"
#include "basegui_scenegraph.h"
#include "basegui_glfont.h"


unsigned int next_power_of_2(unsigned int i) {
  int np2 = 1;
  while(i != 0) {
    i >>= 1;
    np2 <<= 1;
  }
  return np2;
}

GLGraphicsOut::GLGraphicsOut(GLFontLibrary *fnts) {
  tesselator = gluNewTess();
  assert(tesselator);

  _fonts = fnts;
  _hidden = false;
}
GLGraphicsOut::~GLGraphicsOut() {
  gluDeleteTess(tesselator);
}

VisitResult GLGraphicsOut::visitView(GraphicsView *v, VisitState vs) {
  if(vs == VISIT_PRE) {
    glLoadIdentity();
    scale_factors.push_back(double(v->height()));
    multXForm(v->getProjectionMatrix());
  }
  return VISIT_CONTINUE;
}

VisitResult GLGraphicsOut::visitSceneGraph(SceneGraph *g, VisitState vs) {
  return VISIT_CONTINUE;
}

VisitResult GLGraphicsOut::visitStyle(StyleNode *p, VisitState vs) {
  return VISIT_CONTINUE;
}

VisitResult GLGraphicsOut::visitGeometry(GraphicsNode *n, VisitState vs) {
  if(_hidden)
    return VISIT_SKIP; // optimization: skip over hidden nodes
  return VISIT_CONTINUE;
}

VisitResult GLGraphicsOut::visitGroup(Group *g, VisitState vs) {
  if(_hidden)
    return VISIT_SKIP; // optimization: skip over hidden nodes
  return VISIT_CONTINUE;
}

VisitResult GLGraphicsOut::visitXForm(XFormNode *n, VisitState vs) {
  return VISIT_CONTINUE;
}

void GLGraphicsOut::pushMatrix() {
  glPushMatrix();
  scale_factors.push_back(scale_factors.back());
    // TODO would it be faster to glGet when unscale is called?
}

void GLGraphicsOut::multXForm(const XForm &xform) {
  float matrix[16] = { // remember, GL wants their matrices
                       // transposed (column major)
    xform(0,0), xform(1,0), 0, 0,
    xform(0,1), xform(1,1), 0, 0,
             0,          0, 1, 0,
    xform(0,2), xform(1,2), 0, 1};
  glMultMatrixf(matrix);
  scale_factors.back() *= sqrt(fabs(determinant(xform)));
}

double GLGraphicsOut::getScaleFactor() {
  return scale_factors.back();
}

void GLGraphicsOut::popMatrix() {
  scale_factors.pop_back();
  glPopMatrix();
}

void GLGraphicsOut::unscale(double origin_x, double origin_y) {
  glPushMatrix();
  glTranslatef(origin_x, origin_y, 0);
  glScalef(1/scale_factors.back(), 1/scale_factors.back(), 1);
}

void GLGraphicsOut::rescale() {
  glPopMatrix();
}

void GLGraphicsOut::beginPolyline() { glBegin(GL_LINE_STRIP); }
void GLGraphicsOut::beginFilledPolygon() { glBegin(GL_POLYGON); }
void GLGraphicsOut::beginUnfilledPolygon() { glBegin(GL_LINE_LOOP); }
void GLGraphicsOut::beginPoints() { glBegin(GL_POINTS); }
void GLGraphicsOut::end() { glEnd(); }

void GLGraphicsOut::text(double x, double y, const char *t) {
  assert(_cur_font);
  _cur_font->render(x, y, t, _fonts);
}

void GLGraphicsOut::affineText(double x, double y, const char *t) {
  assert(_cur_font);
  _cur_font->renderAffine(x, y, t, _fonts);
}

// Amir:
//  We used to display images by texture mapping
//  But We can't control the gray levels accurately for debug so I have
//  decided to use glDrawPixels for now

void GLGraphicsOut::image(double x1, double y1, double x2, double y2, Image *img) 
{
  int x0 = x1;
  int y0 = y1;

  assert(x1 <= x2);
  assert(y1 <= y2);

  unsigned char *data = img->data();
  const unsigned int format = img->glFormat();
  const unsigned int type = img->glType();
  int h = img->height();
  int w = img->width(); 

  // Get current matrix state, in fortran order.
  double Pt[4][4], Mt[4][4];
  glGetDoublev(GL_PROJECTION_MATRIX, &Pt[0][0]);
  glGetDoublev(GL_MODELVIEW_MATRIX,  &Mt[0][0]);

  // Get total world-to-device transformation. It should be of the form :
  // * 0 0 *
  // 0 * 0 *
  // 0 0 * *
  // 0 0 0 *
  // with the diagonal entries non-zero.

  double T[4][4];
  for (unsigned i=0; i<4; ++i) 
  {
    for (unsigned j=0; j<4; ++j) 
    {
      T[i][j] = 0;
      for (unsigned k=0; k<4; ++k)
        T[i][j] += Pt[k][i] * Mt[j][k]; // Pt[k][i] = P[i][k] etc
    }
  }

  // From image to device coordinates, the projection is :
  // [ T00  0  T03 ]   [ a   u ]
  // [  0  T11 T13 ] ~ [   b v ]
  // [  0   0  T33 ]   [     1 ]

  float a = T[0][0]/T[3][3], b = T[1][1]/T[3][3];
  float u = T[0][3]/T[3][3], v = T[1][3]/T[3][3];

  // Get size of viewport. We need this to determine how much to scale pixels by.
  GLint vp[4]; // x,y, w,h
  glGetIntegerv(GL_VIEWPORT, vp);
  //int vp_x = vp[0];
  //int vp_y = vp[1];
  int vp_w = vp[2];
  int vp_h = vp[3];
  if (vp_w <= 0 || vp_h <= 0)
    return;

  // From device to viewport coordinates, the transformation is :
  // [ vp_w   0  vp_x ] [ 1/2  0  1/2 ]
  // [   0  vp_h vp_y ] [  0  1/2 1/2 ]
  // [   0    0    1  ] [  0   0   1  ]
  // where vp_x, vp_y, vp_w, vp_h are the start, 
  // width and height of the viewport.

  // Compute pixel zoom, as passed to glPixelZoom().
  float zoomx = a*vp_w/2;
  float zoomy = b*vp_h/2;

  // Clip the given region [x1, x2] x [y1, y2] to the viewport.  In
  // device coordinates, the viewport is [-1, +1] x [-1, +1] so it's
  // easiest to start from that. This clipping is especially important
  // for non-local displays, where the display clipping happens on the
  // display server.
  //
  if (a>0) {
    // [ (-1-u)/a, (+1-u)/a ]
    x1 = max(x1, (-1-u)/a);
    x2 = min(x2, (+1-u)/a);
  }
  else {
    // [ (+1-u)/a, (-1-u)/a ]
    x1 = max(x1, (+1-u)/a);
    x2 = min(x2, (-1-u)/a);
  }
  if (b>0) {
    // [ (-1-v)/b, (+1-v)/b ]
    y1 = max(y1, (-1-v)/b);
    y2 = min(y2, (+1-v)/b);
  }
  else {
    // [ (+1-v)/b, (-1-v)/b ]
    y1 = max(y1, (+1-v)/b);
    y2 = min(y2, (-1-v)/b);
  }
  if (x1 > x2 || y1 > y2) {
    //assert(0); //nothign left to display
    return; // that's easy.
  }

  int i_x0 = ceil(x1), i_y0 = ceil(y1);
  int i_x1 = ceil (x2), i_y1 = ceil (y2);

  // To set a valid raster position outside the viewport, first
  // set a valid raster position inside the viewport, then call
  // glBitmap with NULL as the bitmap parameter and with xmove
  // and ymove set to the offsets of the new raster position.
  // This technique is useful when panning an image around the
  // viewport.
  // (from http://www.opengl.org/developers/faqs/technical/clipping.htm#0070)

  glRasterPos2d( i_x0, i_y0 );
   glBitmap(0, 0, 0, 0, 0, 0, NULL);

  // Set pixel transfer characteristics.
  glPixelStorei(GL_UNPACK_ALIGNMENT,   1);         // use byte alignment for now.
  glPixelStorei(GL_UNPACK_ROW_LENGTH,  w);         // size of image rows.
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, i_x0);      // number of pixels to skip on the left.
  glPixelStorei(GL_UNPACK_SKIP_ROWS,   i_y0);      // number of pixels to skip at the bottom.

  glPixelZoom( zoomx, zoomy );
  glDrawPixels(i_x1 - i_x0, // Size of pixel rectangle
          i_y1 - i_y0, // to be written to frame buffer.
          format,
          type,
          data);
}

/* ***************************************************************

void GLGraphicsOut::image(double x1, double y1, double x2, double y2, Image *img) {
  glEnable(GL_TEXTURE_2D);

  bool regenerate = false;
  unsigned int texname;
  if(texture_names.find(img) == texture_names.end()) {
    glGenTextures(1, &texname);
    texture_names[img] = texname;
    regenerate = true;
  } else {
    texname = texture_names[img];
    if(img->dirty())
      regenerate = true;
  }

  unsigned int width_p2 = next_power_of_2(img->width()),
      height_p2 = next_power_of_2(img->height());

  if(regenerate) {
    img->setDirty(false);
    glBindTexture(GL_TEXTURE_2D,texname);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR_MIPMAP_LINEAR
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_DECAL);

    // for some reason, glTexSubImage2D isn't working on linux->SGI remote display...
    unsigned char *data = img->data();
    unsigned char *expanded_data = new unsigned char[4*width_p2*height_p2];
    memset(expanded_data, 255, 4*width_p2*height_p2);

    const unsigned int format = img->glFormat();
    const unsigned int type = img->glType();
    const int img_height = img->height(), img_width = img->width(); 

    for(int i = 0; i < img_height; ++i) {
      for(int j = 0; j < img_width; ++j) {
        if(format == GL_RGBA) {
          expanded_data[4*(width_p2*i + j)+0] = data[4*(img_width*i + j)+0];
          expanded_data[4*(width_p2*i + j)+1] = data[4*(img_width*i + j)+1];
          expanded_data[4*(width_p2*i + j)+2] = data[4*(img_width*i + j)+2];
          expanded_data[4*(width_p2*i + j)+3] = data[4*(img_width*i + j)+3];
        } else {
          expanded_data[4*(width_p2*i + j)+0] = data[3*(img_width*i + j)+0];
          expanded_data[4*(width_p2*i + j)+1] = data[3*(img_width*i + j)+1];
          expanded_data[4*(width_p2*i + j)+2] = data[3*(img_width*i + j)+2];
          expanded_data[4*(width_p2*i + j)+3] = 255;
        }
      }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width_p2, height_p2, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, expanded_data);

    delete []expanded_data;
  } else {
    glBindTexture(GL_TEXTURE_2D,texname);
  }

  glBindTexture(GL_TEXTURE_2D,texname);
  float xrat = img->width()/float(width_p2),
      yrat = img->height()/float(height_p2);
  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);      glVertex2i(x1, y1);
  glTexCoord2f(xrat, 0);    glVertex2i(x2, y1);
  glTexCoord2f(xrat, yrat);  glVertex2i(x2, y2);
  glTexCoord2f(0, yrat);    glVertex2i(x1, y2);
  glEnd();

  glDisable(GL_TEXTURE_2D);
}
*/

void GLGraphicsOut::vertex(double x, double y) { glVertex2d(x, y); }
void GLGraphicsOut::infiniteVertex(double vx, double vy) { glVertex4d(vx, vy, 0, 0); }
void GLGraphicsOut::setColor(float r, float g, float b, float a)
  { glColor4f(r,g,b,a); }
void GLGraphicsOut::setLineWidth(double w) { glLineWidth(w); }
void GLGraphicsOut::setPointSize(double s) { glPointSize(s); }

void GLGraphicsOut::setFont(GLFont *font) {
  _cur_font = font;
}

void GLGraphicsOut::gluTessCombineStaticCB(GLdouble coords[3], void *vertex_data[4],
    GLfloat weight[4], void **outData,
    void *data) {
  reinterpret_cast<GLGraphicsOut*>(data)->gluTessCombineCB(coords, vertex_data, weight, outData);
}

// addTessVertex needs to create a location for 3 contiguous doubles and return a pointer to
// it. we can't use a vector (or some such), because we need to pass direct pointers into
// GL, and it might be reallocated before we call gluTessEndPolygon. but, mallocing 3 new
// pointers each time a vertex is added is too expensive (so an std::list won't work);
// so, we just allocate them in pools. for now, just do constant size pools.
// speed shouldn't be a big deal once we put nodes into display lists anyway.
const int TESS_VERT_POOL_SIZE = 50;
double *GLGraphicsOut::addTessVertex(double x, double y, double z) {
  double *v = NULL;
  if(tesselatorVertices.empty() ||
      tesselatorVertices.back().second+1 >= TESS_VERT_POOL_SIZE)
    tesselatorVertices.push_back(std::make_pair(new double[TESS_VERT_POOL_SIZE*3], 0));
  v = &tesselatorVertices.back().first[3*tesselatorVertices.back().second];
  ++tesselatorVertices.back().second;
  v[0] = x;
  v[1] = y;
  v[2] = z;
  return v;
}

void GLGraphicsOut::clearTessVertices() {
  for(tesselatorVertices_t::iterator
      i = tesselatorVertices.begin(); i != tesselatorVertices.end(); ++i)
    delete []i->first;
  tesselatorVertices.clear();
}

void GLGraphicsOut::gluTessCombineCB(GLdouble coords[3], void *vertex_data[4],
    GLfloat weight[4], void **outData) {
  *outData = addTessVertex(coords[0], coords[1], coords[2]);
}

void GLGraphicsOut::beginConcaveFilledPolygon() {
  assert(tesselator);

  gluTessCallback(tesselator, GLU_TESS_BEGIN, (OPENGL_CALLBACK)glBegin);
  gluTessCallback(tesselator, GLU_TESS_END, (OPENGL_CALLBACK)glEnd);
  gluTessCallback(tesselator, GLU_TESS_VERTEX, (OPENGL_CALLBACK)glVertex2dv);
  gluTessCallback(tesselator, GLU_TESS_COMBINE_DATA, (OPENGL_CALLBACK)GLGraphicsOut::gluTessCombineStaticCB);
  // TODO have error callback, combine callback?

  gluTessBeginPolygon(tesselator, this); // the second arg is the data pointer passed in to the combine callback
  gluTessBeginContour(tesselator);
}

void GLGraphicsOut::endConcaveFilledPolygon() {
  assert(tesselator);
  gluTessEndContour(tesselator);
  gluTessEndPolygon(tesselator);
  clearTessVertices();
}

void GLGraphicsOut::concaveVertex(double x, double y) {
  assert(tesselator);

  double *v = addTessVertex(x,y,0);
  gluTessVertex(tesselator, v, v);
}

void GLGraphicsOut::setHidden(bool h) {
  _hidden = h;
}

GLPicker::GLPicker(GLFontLibrary *fonts)
  :  GLGraphicsOut(fonts), _hit_records(NULL) {}

void GLPicker::setHitTestParameters(std::vector<HitRecord> *hits,
      int x, int y, int w, int h, int diameter) {
  _hit_records = hits;
  _hit_x = x;
  _hit_y = y;
  _window_w = w;
  _window_h = h;
  _diameter = diameter;
}

VisitResult GLPicker::visitSceneGraph(SceneGraph *g, VisitState vs) {
  assert(_hit_records);

  if(vs == VISIT_PRE) {
    cur_names.clear();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int viewport[4] = {0,0,_window_w,_window_h};

    // remember to flip the origin from top left to bottom left
    gluPickMatrix(_hit_x, _window_h-_hit_y, _diameter, _diameter, viewport);
    glOrtho(0, _window_w/float(_window_h), 0, 1,  -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glSelectBuffer(GL_SELECT_BUFFER_SIZE, select_buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    cur_names.push_back(0);
    glPushName(0);
  } else {
    int hits = glRenderMode(GL_RENDER);
    if(hits < 0) {
      std::cerr
        << "warning: OpenGL select buffer overflowed, selection will not work" << std::endl
        << "to work around, increase GL_SELECT_BUFFER_SIZE in glgraphics.h" << std::endl;
    }

    unsigned int *gl_hit_ptr = select_buffer;

    for(int i = 0; i < hits; ++i) {
      _hit_records->push_back(HitRecord());
      GraphicsNode *cur_node = g->rootGroup();

      // ignore the first item on the gl hit stack, it's always zero
      // (the root is the zeroth child)
      for(unsigned int k = 1; k < gl_hit_ptr[0]; ++k) {
        _hit_records->back().group_stack.push_back(
            static_cast<Group*>(cur_node));

        assert(dynamic_cast<Group*>(cur_node));
        cur_node = cur_node->child(gl_hit_ptr[3+k]);
      }
      assert(dynamic_cast<GeometryNode*>(cur_node));
      _hit_records->back().hit_object = cur_node;

      gl_hit_ptr += 3+gl_hit_ptr[0];
    }

    // restore the projection matrix (we shouldn't have modified
    // the modelview matrix... if we did, the scene graph is busted)
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
  }
  return VISIT_CONTINUE;
}

VisitResult GLPicker::visitGeometry(GraphicsNode *n, VisitState vs) {
  if(vs == VISIT_PRE) {
    glLoadName(cur_names.back()++);
    if(_hidden)
      return VISIT_SKIP;
  }

  return VISIT_CONTINUE;
}

VisitResult GLPicker::visitGroup(Group *g, VisitState vs) {
  if(vs == VISIT_PRE) {
    if(!_hidden) {
      glLoadName(cur_names.back()++);
      cur_names.push_back(0);
      glPushName(0);
      return VISIT_CONTINUE;
    } else {
      glLoadName(cur_names.back()++);
      return VISIT_SKIP;
    }
  } else {
    cur_names.pop_back();
    glPopName();
    return VISIT_CONTINUE;
  }
}
