#ifndef GL_FONT_H_INCLUDED
#define GL_FONT_H_INCLUDED

#include <string>
#include "basegui_xpdefs.h" // defines DISABLE_GL_FONTS to 0 or 1

/*
 * A GLFontLibrary is what stores the actual fonts, outlines, etc.
 * A GLFont is just a specification of what font we want.
 * We do it this way because each window needs it's _own_ GLFontLibrary,
 * because the FTGL library stores data in display lists which are
 * specific to each window. So, each window should call GLFont::newLibrary
 * once, and store its own GLFontLibrary.
 */
class GLFontLibrary;

class GLFont {
public:
  enum FontFace {
    DEFAULT_FACE
  };

private:
  FontFace fc;
  int sz;

public:
  static GLFont *getFont(FontFace f, int size);
  static GLFontLibrary *newLibrary();
  static void deleteLibrary(GLFontLibrary *lib);

  void render(double x, double y, std::string txt, GLFontLibrary *l);
  void renderAffine(double x, double y, std::string txt, GLFontLibrary *l);

  FontFace face() const;
  int size() const;

private:
  GLFont(FontFace ffc, int ssz);
  friend class GLFontLibrary;

  GLFont(const GLFont &);
  GLFont &operator=(const GLFont &);
};

#endif
