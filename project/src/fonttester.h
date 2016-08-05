#ifndef FONTTESTER_H
#define FONTTESTER_H


// STL
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>
// libCGL
#include "CGL/CGL.h"
#include "CGL/renderer.h"
#include "CGL/osdtext.h"

//TODO add your freetype shit
#include "ft2build.h"
#include FT_FREETYPE_H
#include "freetype.h"
#include "ftimage.h"
//#include "freetype/*.h"

#include "fontedit.h"


namespace CGL {

struct AppConfig {

  AppConfig () {
  }

  char* directory_filename;
  std::string text_input;
};

//IF IT WASN'T CLEAR, THIS IS YOUR RENDERER
class FontTester : public Renderer {
 public:

  FontTester(AppConfig config);
  ~FontTester( ) { }
  
  FT_Library library;
  FT_Face face;
  FT_GlyphSlot slot;
  FT_Outline outline;
  int error = FT_Init_FreeType( &library );
  FT_UInt glyph_index;
  int pen_x, pen_y, n;
  std::string text;
  char* filename;
  int selectedPointIndex;
  bool selectMode;
  float radius;
  FT_Vector pointset[];
  
  void init( void ) ;

  void render( void ) ;

  void resize( size_t w, size_t h ) ;

  std::string name( void );

  std::string info( void );

  void cursor_event( float x, float y ); //

  void scroll_event( float offset_x, float offset_y ); //

  void mouse_event( int key, int event, unsigned char mods ); //

  void keyboard_event( int key, int event, unsigned char mods ) ; //

  void use_hdpi_reneder_target() { use_hdpi = true; } //
  
  bool selectPointAttempt();
  void movePoint();
  

private:

  // Internal event system //

  float mouseX, mouseY;
  enum e_mouse_button {
    LEFT   = MOUSE_LEFT,
    RIGHT  = MOUSE_RIGHT,
    MIDDLE = MOUSE_MIDDLE
  };

  size_t screenW;
  size_t screenH;

  bool gl_window;
};

} // namespace CGL

#endif // FONTTESTER_H
