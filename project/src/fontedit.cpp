#include "fontedit.h"

using namespace std;

namespace CGL {


/****** Helpers ******/
  

// Extract the R, G, or B channel out of an RGBA color stored in a single 32bit integer
static uint32_t red_channel(uint32_t color) {
    return (255 & (color >> (0)));
}

static uint32_t green_channel(uint32_t color) {
    return (255 & (color >> (8)));
}

static uint32_t blue_channel(uint32_t color) {
    return (255 & (color >> (16)));
}

float bez_quad(float t, float b0, float b1, float b2){
  return (1-t)*(1-t)*b0 + 2*t*(1-t)*b1 + t*t*b2;
}

void draw_circle(float radius, float mouseX, float mouseY, float screenW, float screenH, int lineAmount)
{
	float tau = 2.0f * PI;
	glColor4f(0, 1, 0, 1);
	glBegin(GL_LINE_LOOP);
		for(int i = 0; i <= lineAmount;i++) { 
			glVertex2f(
			    mouseX/screenW * 2 + (radius * cosf(i *  tau / lineAmount)) - 1, 
			    -(mouseY/screenH * 2 + (radius * sinf(i * tau / lineAmount)) - 1)
			);
		}
		
	glEnd();
}

float distance(float x1, float y1, float x2, float y2){
  return sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
}

} // namespace CGL

