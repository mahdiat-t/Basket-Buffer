#include "iGraphics.h"
#include<time.h>


void iDraw() 
{
    iClear();
    iFilledCircle(200, 200, 50); // Drawing a circle
}

void iMouseMove(int mx, int my) 
{}
void iMouse(int button, int state, int mx, int my) 
{}
void iKeyboard(unsigned char key) 
{}
void iSpecialKeyboard(unsigned char key) 
{}




int main() {
    iInitialize(400, 400, "Hello TJ!");
}