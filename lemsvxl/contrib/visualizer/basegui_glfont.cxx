#include "basegui_glfont.h"
#include <map>
#include <iostream>

#if !(DISABLE_GL_FONTS)
#include "basegui_xpgl.h"
#include "basegui_xpdefs.h" // for DEFAULT_FACE_PATH
#include <FTGLBitmapFont.h>
#include <FTGLPixmapFont.h>
#include <FTGLPolygonFont.h>
#else
class FTFont;
#endif

// TODO OPT: if looking up the font in the map<> takes too much time,
// have a FontMemento class which encapulates a font lookup, and have
// the GL graphics visitor remember the font lookup memento.
class GLFontLibrary {
public:
  GLFontLibrary();
  ~GLFontLibrary();

  void render(GLFont *font, double x, double y, std::string txt);
  void renderAffine(GLFont *font, double x, double y, std::string txt);

private:
  FTFont *openBitmapFont(GLFont *fontspec);
  FTFont *openPolygonFont(GLFont *fontspec);

  typedef std::map<GLFont*, FTFont*> polygonFonts_t;
  polygonFonts_t polygonFonts;

  typedef std::map<GLFont*, FTFont*> bitmapFonts_t;
  bitmapFonts_t bitmapFonts;
};

GLFont::GLFont(FontFace ffc, int ssz) : fc(ffc), sz(ssz) { }

GLFontLibrary *GLFont::newLibrary() { return new GLFontLibrary; }
void GLFont::deleteLibrary(GLFontLibrary *lib) { delete lib; }

GLFont *GLFont::getFont(FontFace f, int size) {
  return new GLFont(f, size);
}

void GLFont::render(double x, double y, std::string txt, GLFontLibrary *l) {
  l->render(this, x, y, txt);
}

void GLFont::renderAffine(double x, double y, std::string txt, GLFontLibrary *l) {
  l->renderAffine(this, x, y, txt);
}

GLFont::FontFace GLFont::face() const {
  return fc;
}

int GLFont::size() const {
  return sz;
}

GLFontLibrary::GLFontLibrary() {}
GLFontLibrary::~GLFontLibrary() {
  for(polygonFonts_t::iterator
      i = polygonFonts.begin(); i != polygonFonts.end(); ++i)
    delete i->second;
  for(bitmapFonts_t::iterator
      j = bitmapFonts.begin(); j != bitmapFonts.end(); ++j)
    delete j->second;
}

FTFont *GLFontLibrary::openBitmapFont(GLFont *fontspec) {
#if DISABLE_GL_FONTS
  return NULL;
#else

  FTFont *glf = ANTIALIAS_GL_FONTS ?
    (FTFont*)new FTGLPixmapFont : (FTFont*)new FTGLBitmapFont;

  const char *fontFile = NULL;
  switch(fontspec->face()) {
    case GLFont::DEFAULT_FACE:
      fontFile = DEFAULT_FACE_PATH;
      break;
  }

  if(!glf->Open(fontFile, false)) {
    std::cerr << "warning: couldn't open font file " << fontFile << std::endl;
    return NULL;
  }

  glf->FaceSize(fontspec->size());
  return glf;
#endif
}

FTFont *GLFontLibrary::openPolygonFont(GLFont *fontspec) {
#if DISABLE_GL_FONTS
  return NULL;
#else

  FTFont *glf = new FTGLPolygonFont;

  const char *fontFile = NULL;
  switch(fontspec->face()) {
    case GLFont::DEFAULT_FACE:
      fontFile = DEFAULT_FACE_PATH;
      break;
  }

  if(!glf->Open(fontFile, false)) {
    std::cerr << "warning: couldn't open font file " << fontFile << std::endl;
    return NULL;
  }

  glf->FaceSize(fontspec->size());
  return glf;
#endif
}

void GLFontLibrary::render(GLFont *fontspec,
    double x, double y, std::string txt) {
#if !(DISABLE_GL_FONTS)
  FTFont *f = bitmapFonts[fontspec];
  if(!f) {
    f = openBitmapFont(fontspec);
    bitmapFonts[fontspec] = f;
  }

  assert(f);
  glRasterPos2f(x,y);
  f->render(txt.c_str());
#endif
}

void GLFontLibrary::renderAffine(GLFont *fontspec,
    double x, double y, std::string txt) {
#if !(DISABLE_GL_FONTS)
  FTFont *f = polygonFonts[fontspec];
  if(!f) {
    f = openPolygonFont(fontspec);
    polygonFonts[fontspec] = f;
  }

  assert(f);
  glTranslatef(x, y, 0);
  f->render(txt.c_str());
  glTranslatef(-x, -y, 0);
#endif
}
