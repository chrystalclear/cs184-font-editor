#include "fonttester.h"
//#include "fontedit.h"

using namespace std;

namespace CGL {

#define msg(s) cerr << "[FontEditor] " << s << endl;

FontTester::FontTester(AppConfig config) {
  //zoom = rad * 4;

  //SET UP VARIABLES
  filename = config.directory_filename;
  if (filename == "")
  {
    cout << "No ttf font file detected, defaulting to ARIAL.TTF\n";
    filename = "../ttf/ARIAL.TTF";
  }
  text = config.text_input;
  if (text.size() > 1)
  {
    cout << "One letter at a time for now please!\n";
    text = text[0]; 
  }
  if (text == "")
  {
    cout << "No text input detected, defaulting to single letter trial\n";
    text = "P"; 
  }
  
  radius = .03f; //radius
  selectMode = false;
  
  
  //INITIALIZE LIBRARY
  const char* err;
  if ( error )
  {
    msg("An error occurred during FreeType library initialization\n" << err);
    exit(EXIT_FAILURE);
  }
  
  //CREATE FACE OBJECT
  error = FT_New_Face(library,
                     filename,
                     0,
                     &face ); //TODO: THIS NEEDS TO ACCEPT AN INPUT FONT, NOT LOAD A DEFAULT
  if ( error == FT_Err_Unknown_File_Format )
  {
    msg("the font file could be opened and read, but it appears that its font format is unsupported\n" << err);
    exit(EXIT_FAILURE);
  }
  else if ( error )
  {
    std::cout << error << "\n";
    msg("the font file could not be opened or read, or it is broken...\n" << err);
    exit(EXIT_FAILURE);
  }
  //TODO: check for multiple faces
  slot = face->glyph;
  
  //SET CHARACTER SIZE
  error = FT_Set_Char_Size(
          face,    /* handle to face object           */
          0,       /* char_width in 1/64th of points  */
          16*64,   /* char_height in 1/64th of points */
          screenW,     /* horizontal device resolution    */
          screenH );   /* vertical device resolution      */
  //0 for char_width means emulate char_height
  
  error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
  if ( error )
  {msg("Error loading glyph image into slot\n" << err);}  //continue;  /* ignore errors */

  /* Get the outline and its points*/
  outline = slot->outline;
  
  error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
  if ( error )
  {msg("Error loading glyph image into slot\n" << err);}  //continue;  // ignore errors

  // Get the outline and its points
  outline = slot->outline;
  
}
  

void FontTester::init() {
  if (gl_window) {
    // Lighting needs to be explicitly enabled.
    glEnable(GL_LIGHTING);

    // Enable anti-aliasing and circular points.
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glEnable(GL_POINT_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
  }
}

//TODO: deconstruct this
void FontTester::render() {

  glPushAttrib(GL_VIEWPORT_BIT);
  glViewport(0, 0, screenW, screenH);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-1, 1, -1, 1, 0, 1);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glTranslatef(0, 0, -1);

  // -- Black with opacity .8;
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  //CODE FROM THE ORIGINAL RENDER
  
  
   
  
/*
  //for ( n = 0; n < text.size(); n++ ) //useless atm since we're only doing one letter at a time
  {
      // load glyph image into the slot (erase previous one)
      error = FT_Load_Char( face, text[n], FT_LOAD_RENDER );
      if ( error )
      {msg("Error loading glyph image into slot\n" << err);}  //continue;  // ignore errors

      // Get the outline and its points
      outline = slot->outline;
      */
      
      //TODO now, draw to our target surface 
      glColor4f(1, 0, 0, 1);
      float currentx, currenty;
      int contourstart = 0, contourend, nextind, prevind;
      for (int j = 0; j < outline.n_contours; j++)
      {
        glBegin(GL_LINE_STRIP);
        contourend = outline.contours[j];
        int i = contourstart;
        for(i; i<=contourend; i++)
        {
          //EXTRA: this only accomodates for ttf files, so there are no cubics ATM
          if (FT_CURVE_TAG(outline.tags[i]) == 1) //point on the line
          {
              //simple line segment
              glVertex2f((float)outline.points[i].x / (float) slot->metrics.width - .5, (float)outline.points[i].y / (float) slot->metrics.height - .5 );
            if (i==contourend and FT_CURVE_TAG(outline.tags[contourstart]) == 1) //wraparound edge case for above
            {
              //add final line segment
              glVertex2f((float)outline.points[contourstart].x / (float) slot->metrics.width -.5, (float)outline.points[contourstart].y / (float) slot->metrics.height - .5);
            }
          }
          else //conic point off the line
          {
            nextind = i+1, prevind = i-1;
            if (nextind > contourend) {nextind = contourstart;}
            else if (prevind < contourstart) {prevind = contourend;}
            float nextx = outline.points[nextind].x;
            float nexty = outline.points[nextind].y;
            float prevx = outline.points[prevind].x;
            float prevy = outline.points[prevind].y;
            if (FT_CURVE_TAG(outline.tags[prevind]) == 0){
              prevx = (prevx + outline.points[i].x)/2;
              prevy = (prevy + outline.points[i].y)/2;
            }
            if (FT_CURVE_TAG(outline.tags[nextind]) == 0){
              nextx = (nextx + outline.points[i].x)/2;
              nexty = (nexty + outline.points[i].y)/2;
            }

            //time to use that nice parametric equation
            
            for (float t = 0.f; t <= 1.f; t += 1.f/100.f)
            {
              float tempx = bez_quad(t, prevx, outline.points[i].x, nextx);
              float tempy = bez_quad(t, prevy, outline.points[i].y, nexty);
              glVertex2f(tempx / (float) slot->metrics.width - .5,
                      tempy / (float) slot->metrics.height - .5);
            }
          }

        
        /* THIS IS HOW TO REPRESENT THE POINTS FOR NOW:
        glVertex2f((float)outline.points[i].x / (float) slot->metrics.width - .5,
                    (float)outline.points[i].y / (float) slot->metrics.height - .5 );
        //TODO: end*/
        }
        contourstart = contourend + 1;
        glEnd();
      }
      //std::cout << "all lines rendered\n";
    
    
      //draw the point markers over the lines
      glPointSize(4.f);
      glBegin(GL_POINTS);
      int temp = 0;
      for (int i = 0; i<outline.n_points; i++){
        if (selectMode and i == selectedPointIndex)
          {glColor4f(0, 1, 0, 1);}
        else if (FT_CURVE_TAG(outline.tags[i]) == 1)
          {glColor4f(1, 1, 0, 1);} //yellow = control on line
        else
          {glColor4f(0, .5, 1, 1);}
      glVertex2f((float)outline.points[i].x / (float) slot->metrics.width - .5,
                      (float)outline.points[i].y / (float) slot->metrics.height - .5 );
      }
      glEnd();
    
    /* increment pen position */
    //pen_x += slot->advance.x >> 6; //y not useful for basic horizontal string
    
  //}
  //std::cout << "all text rendered\n";
  //SET MOUSE RADIUS 
  draw_circle(radius, mouseX, mouseY, screenW, screenH);
	//std::cout << "here we go " << mouseX/screenW *2 -1<< " " << -(mouseY/screenH)*2-1 << "\n";
}

void FontTester::resize(size_t w, size_t h) {
  screenW = w;
  screenH = h;
}


string FontTester::name() {
  return "FontTester";
}

string FontTester::info() {
  return "FontTester";
}

//TODO: check how this works
void FontTester::cursor_event(float x, float y) {
  mouseX = x;
  mouseY = y;
  //point2 = Vector2D((mouseX/screenW-.5)*2. * zoom, ((float)screenH)/(float)screenW*(.5-mouseY/screenH)*2. * zoom);
}


//TODO: can probably keep this if we implement ability to drag
void FontTester::scroll_event(float offset_x, float offset_y) {
  //zoom *= exp(offset_y / 10.);
}

//TODO: same for this
void FontTester::mouse_event(int key, int event, unsigned char mods) {
  switch(event) {
    case EVENT_PRESS:
      switch(key) {
        case MOUSE_LEFT:
          if (selectMode){
            //TODO: move the node
            movePoint();
          } 
          else
          {
            //TODO: try to select node
            if (selectPointAttempt()){
              msg("Selected point at " << (float)outline.points[selectedPointIndex].x / (float) slot->metrics.width - .5 << " " <<
                  (float)outline.points[selectedPointIndex].y / (float) slot->metrics.height - .5 << "\n");
              //cout << "[FontEditor] Selected point at " << (float)outline.points[selectedPointIndex].x / (float) slot->metrics.width - .5 << " " <<
              //    (float)outline.points[selectedPointIndex].y / (float) slot->metrics.height - .5 << "\n";
            }
            else
            {msg("No selection made.\n");}
          }
          //cout << "[FontEditor] Selected point at " << " " << endl;
          // mouse_pressed(LEFT);
          break;
        case MOUSE_RIGHT:
          // mouse_pressed(RIGHT);
          selectMode = false;
          break;
        case MOUSE_MIDDLE:
          // mouse_pressed(MIDDLE);
          break;
      }
      break;
    case EVENT_RELEASE:
      switch(key) {
        case MOUSE_LEFT:
          // mouse_released(LEFT);
          break;
        case MOUSE_RIGHT:
          // mouse_released(RIGHT);
          break;
        case MOUSE_MIDDLE:
          // mouse_released(MIDDLE);
          break;
      }
      break;
  }
}

//TODO: typing functionality? hrmmm maybe not
void FontTester::keyboard_event(int key, int event, unsigned char mods) {
  if (event != EVENT_PRESS) 
    return;

  if (key >= '1' && key <= '4') {
    //camera.mount_lens(key-'1');
    return;
  }
  /*switch (key) {
    case '-':
      if (numrays >= 1) numrays--;
      break;
  }*/
}

bool FontTester::selectPointAttempt(){
  //glVertex2f((float)outline.points[i].x / (float) slot->metrics.width - .5,
  //                  (float)outline.points[i].y / (float) slot->metrics.height - .5 );
  int bestPick = -1;
  float bestDistance = 100000;
  for (int i = 0; i < outline.n_points; i++)
  {
    float currentDistance = distance((float)outline.points[i].x / (float) slot->metrics.width - .5, (float)outline.points[i].y / (float) slot->metrics.height - .5, mouseX/screenW*2-1, -(mouseY/screenH*2-1));
    
    if (currentDistance < .03f)
    {
    }
    if ((float)currentDistance < (float)radius)
    {
    if (currentDistance < bestDistance)
    {
      bestDistance = currentDistance;
      bestPick = i; 
    }
    }
  }
  if (bestPick == -1)
    {return false;}
  selectMode = true;
  selectedPointIndex = bestPick;
  return true;
}

void FontTester::movePoint(){
  if (selectMode){
    FT_Vector temp;
    temp.x = ((mouseX/screenW *2 - 1) + .5) * (float)slot->metrics.width;
    temp.y = (-(mouseY/screenH *2 -1) +.5) * (float)slot->metrics.height;
    //outline.points[selectedPointIndex].x = ((mouseX/screenW *2 - 1) + .5) * (float)slot->metrics.width ;
    //outline.points[selectedPointIndex].y = ((mouseY/screenH *2 -1) +.5)* (float)slot->metrics.height;
    outline.points[selectedPointIndex] = temp;
    //*(outline.points + selectedPointIndex) = temp;
    cout<<"hollaholla\n";
  }
  cout << (mouseX/screenW *2 - 1) << " " << ((mouseY/screenH *2 -1)) << "\n";
  cout << (float)outline.points[selectedPointIndex].x / (float) slot->metrics.width - .5 << " " << (float)outline.points[selectedPointIndex].y / (float) slot->metrics.height - .5 << "\n";
}


} // namespace CGL
