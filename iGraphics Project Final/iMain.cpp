#include "iGraphics.h"

int hoopX = 620, hoopY = 385; // Hoop position
int ballX = 325, ballY = 100; // Ball position
int ballRadius = 20;          // Ball radius
int score = 0;
int cx, cy;
bool inAir = false;

double g = 9.8;
double vY = 0;
int v = 60;
bool scored = false;

#define screen_width 980
#define screen_height 600

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

// Function declarations
void drawMenu();
void drawInstructions();
void playGame();
void drawBall();
void DrawOuterEllipse();
void DrawBackboard();
void drawHoopnet();
void drawBoundary();
void level2();
void projectile_activator();

void drawMenu()
{
    iClear();
    iShowBMP(0, 0, "menuu.bmp");
    //iSetColor(255,234,123);
    iText(350, 400, "Basket Buffer", GLUT_BITMAP_TIMES_ROMAN_24);

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

void drawBall()
{
    iSetColor(255, 255, 255);            // White color to create the ring effect
    iFilledEllipse(hoopX, hoopY, 35, 5); // Inner ellipse
}

void DrawOuterEllipse()
{
    iSetColor(255, 0, 0);                 // Red color for the rim
    iFilledEllipse(hoopX, hoopY, 40, 10); // Outer ellipse
}

void DrawBackboard()
{
    iSetColor(0, 0, 0);                                // Black color for the backboard
    iFilledRectangle(hoopX - 50, hoopY + 10, 100, 10); // Backboard
}

void drawHoopnet()
{
    // Draw the backboard
    iFilledRectangle(660, 357, 6, 100);
    // Draw the net with lines
    iSetColor(255, 255, 255); // White color for the net
    for (int i = 0; i <= 35; i += 7)
    {
        iLine(hoopX - 35 + i, hoopY - 10, hoopX - 20 + i, hoopY - 50); // Diagonal lines from left to right
        iLine(hoopX + 35 - i, hoopY - 10, hoopX + 20 - i, hoopY - 50); // Diagonal lines from right to left
    }
}

void drawBoundary()
{
    iSetColor(255, 255, 255);
    iLine(305, 0, 305, screen_height);
    iLine(670, 0, 670, screen_height);
}
bool test_case = false;
void projectile_activator()
{
    if (inAir)
    {
        // Update vertical position and velocity
        float t = .25;
        ballY += vY * t - 0.5 * g * (t * t); // Vertical motion with gravity
        vY -= g * t;                         // Update vertical velocity due to gravity

        // Prevent the ball from going below the ground
        if (ballY <= 50)
        {
            ballY = 50;
            vY = 0;        // Reset vertical velocity
            inAir = false; // Ball is no longer in the air
            g = 9.8;
        }
        if(ballX >= 550 && ballX <= 560 && ballY >= 390 && ballY <= 590)
        {
            test_case = true;
        }
        if ((ballX >= hoopX - 30) && (ballX <= hoopX + 30) &&
            (ballY >= hoopY - 5) && (ballY <= hoopY + 5) && test_case)
        {
            scored = true;
            score++;
            printf("%d\n",vY);
            test_case = false;
        }
        if ((ballX >= hoopX - 60) && (ballX <= hoopX - 20) &&
            (ballY >= hoopY - 10) && (ballY <= hoopY + 10))
        {
            inAir = true;
            vY = v - 10;
            scored = false;
        }
    }
    ballX += 3;
    if (ballX + ballRadius > 670)
    {
        ballX = 305 + ballRadius;
    }
}

void level2()
{
    iSetColor(0, 0, 0);
    iFilledRectangle(0, 0, screen_width, screen_height);
    iSetColor(255, 255, 255);
    iText(350, 500, "LEVEL 2!!", GLUT_BITMAP_TIMES_ROMAN_24);
}

void playGame()
{
    iClear();
    // Placeholder for game logic. Here you can add your game's drawing and updating code.
    iShowBMP(0, 0, "bg1.bmp");
    DrawBackboard();
    DrawOuterEllipse();
    drawBall();
    drawHoopnet();
    drawBoundary();
    iSetColor(255, 165, 0); // Orange color

    iFilledCircle(ballX, ballY, ballRadius);
    iSetColor(255, 255, 0);
    char scoreText[50];
    sprintf(scoreText, "score: %d", score);
    iText(10, 570, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);
    if (scored)
    {
        iSetColor(0, 0, 255);
        iText(350, 500, "SCORE!!", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    if (score >= 10)
    {
        level2();
    }
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
    else if (currentState.state == PLAY.state)
    {
        playGame();
    }
}

void iMouseMove(int mx, int my)
{}

void iMouse(int button, int state, int mx, int my)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        cx = mx;
        cy = my;
        printf("(x,y) = %d, %d\n", cx, cy);

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
}

void iKeyboard(unsigned char key)
{
    if (key == ' ')
    {
        inAir = true;
        vY = v;
        scored = false;
    }
}

void iSpecialKeyboard(unsigned char key)
{
}

int main()
{
    iSetTimer(10, projectile_activator);
    iInitialize(screen_width, screen_height, "Bouncy Dunk Game - Level One"); // Initialize the graphics window
    return 0;
}
