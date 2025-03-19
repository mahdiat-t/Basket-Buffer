#include "iGraphics.h"

int cx, cy;
//int x,y;

struct GameState
{
    int state;       // 0 for MENU, 1 for PLAY, 2 for INSTRUCTIONS, 3 for EXIT
    const char* name; // Optional: name of the state for debugging or display
};

// Define the states
GameState MENU = {0, "MENU"};
GameState PLAY = {1, "PLAY"};
GameState INSTRUCTIONS = {2, "INSTRUCTIONS"};
GameState EXIT = {3, "EXIT"};

// Initialize current state
GameState currentState = MENU;

void drawMenu()
{
    iClear();
    iShowBMP(0,0,"menuu.bmp");
    iSetColor(0, 0, 0);
    iText(350, 400, "Bouncy Dunk Game", GLUT_BITMAP_TIMES_ROMAN_24);
    
    iSetColor(255, 255, 255);
    iFilledRectangle(100, 400, 200, 50); // Start Game button
    iSetColor(0, 0, 0);
    iText(115, 415, "Start Game", GLUT_BITMAP_HELVETICA_18);
    
    iSetColor(255, 255, 255);
    iFilledRectangle(100, 300, 200, 50); // Instructions button
    iSetColor(0, 0, 0);
    iText(115, 315, "Instructions", GLUT_BITMAP_HELVETICA_18);
    
    iSetColor(255, 255, 255);
    iFilledRectangle(100, 200, 200, 50); // Exit button
    iSetColor(0, 0, 0);
    iText(115, 215, "Exit", GLUT_BITMAP_HELVETICA_18);

}

void drawInstructions()
{
    iClear();
    iSetColor(255, 255, 255);
    iText(50, 500, "Instructions", GLUT_BITMAP_TIMES_ROMAN_24);
    iText(50, 450, "1. Press SPACE to throw the ball.", GLUT_BITMAP_HELVETICA_18);
    iText(50, 400, "2. Aim for the hoop to score points.", GLUT_BITMAP_HELVETICA_18);
    iText(50, 350, "3. The game progresses to the next level when you score 5 points.", GLUT_BITMAP_HELVETICA_18);
    iText(50, 300, "4. Enjoy the game!", GLUT_BITMAP_HELVETICA_18);
    
    iSetColor(255, 255, 255);
    iFilledRectangle(50, 50, 100, 50); // Back button
    iSetColor(0, 0, 0);
    iText(75, 65, "Back", GLUT_BITMAP_HELVETICA_18);
}

void iDraw()
{
    if (currentState.state == MENU.state)
    {
        drawMenu();
    }
    else if (currentState.state == INSTRUCTIONS.state)
    {
        drawInstructions();
    }
}

void iMouseMove(int mx, int my)
{
}

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (currentState.state == MENU.state)
        {
            if (mx >= 100 && mx <= 300)
            {
                if (my >= 400 && my <= 450) // Start Game button
                {
                    currentState = PLAY;
                }
                else if (my >= 300 && my <= 350) // Instructions button
                {
                    currentState = INSTRUCTIONS;
                }
                else if (my >= 200 && my <= 250) // Exit button
                {
                    exit(0); // Exit the game
                }
            }
        }
        else if (currentState.state == INSTRUCTIONS.state)
        {
            if (mx >= 50 && mx <= 150 && my >= 50 && my <= 100) // Back button
            {
                currentState = MENU;
            }
        }
    }

    /* if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        cx = mx;
        cy = my;
        /* x = mx;
        y = my; */
        //printf("(x,y) = %d, %d\n", cx, cy);
    //}*/
}


void iSpecialKeyboard(unsigned char key)
{
    // Placeholder for special keyboard inputs
}


void iKeyboard(unsigned char key)
{
    // Handle keyboard inputs here if needed
}

int main()
{
    iInitialize(980, 600, "Menu Example");
    return 0;
}
