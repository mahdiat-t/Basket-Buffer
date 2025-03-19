#include "iGraphics.h"
#include <time.h>
#include <stdio.h>
#include <string.h>

// Constants
#define MAX_NAME_LENGTH 500 
#define MAX_STORED 5    // Can store 5 high scores
#define screen_width 980
#define screen_height 600

// Global variables
char userInput[MAX_NAME_LENGTH];
int inputIndex = 0;
bool inputMode = false;
bool name_entered = false;

bool show_leaderboard = false;
bool printingState = true;

int len = 0;    
int hoopX = 620;
int hoopY = 385; // Hoop position
float ballX = 325, ballY = 100; // Ball position
int ballRadius = 20; // Ball radius
int cx, cy;
int score = 0;
bool scored = false;

bool inAir = false;
double g = 9.8;
double vX = 0.0;
double vY = 0;
int v = 60;
float t = 0.25;


int levelTime = 10; // Time in seconds for Level 1
int timeLeft = levelTime; // Remaining time
bool timeOver = false;    // Flag for game over
bool timerPaused = false;
int totalTime = 45;      // Total time for the level in seconds
int currentTime = totalTime;
int extraTime = 0;    // Remaining time
/* int timeBarWidth = 300;  // Maximum width of the time bar
int timeBarHeight = 20;
int timeBarX = 10;  // Starting x
int timeBarY = 550;  // y  */
bool paused = false;
bool gameStarted = false;  // Track if the game has started
bool test_case = false;

int currentLevel = 1; // Start at Level 1
int levelScoreThreshold = 2; // Points required to advance to the next level

int selectedMenuOption = 0;  // Tracks the currently highlighted menu option

// Function declarations
void drawMenu();
void drawInstructions();
void drawBall();
void DrawOuterEllipse();
void drawHoopnet();
void drawBoundary();
void drawObstacles();
void drawLeaderboard();
void drawGameOver();
void drawLevelComplete();
void drawPauseMenu();
void iDraw();
void iMouse(int button, int state, int mx, int my);
void iMouseMove(int mx, int my);
void playGame();
void level1();
void level2();
void level3();
void level4();
void projectile_activator();
void resetGameState();
void read_high_scores();
void write_high_scores();
void add_new_score();
void show_high_score();
void updateTimer();
void toggleTimerPause();
void decrementTimer();
void CollisionWObs();
void moveObstacles();

// Structure for GameState
struct GameState {
    int state;       // 0 for MENU, 1 for PLAY, 2 for INSTRUCTIONS, 3 for EXIT
    const char* name; // Optional: name of the state for debugging or display
};

// Define the states
GameState MENU = {0, "MENU"};
GameState PLAY = {1, "PLAY"};
GameState LEADERBOARD = {2, "LEADERBOARD"};
GameState INSTRUCTIONS = {3, "INSTRUCTIONS"};
GameState EXIT = {4, "EXIT"};
GameState GAME_OVER = {5, "GAME_OVER"}; // New Game Over state
GameState PAUSED = {6, "PAUSED"};  // New Pause State
GameState LEVEL_COMPLETE = {7, "LEVEL_COMPLETE"};

GameState currentState = MENU; // Initialize current state


struct hscore {
    char name[30];
    int score;
} high_score[MAX_STORED];

// Function definitions

void read_high_scores() {
    FILE *fp = fopen("Score.txt", "r");
    if (fp != NULL) {
        len = 0;
        while (fscanf(fp, "%s %d", high_score[len].name, &high_score[len].score) != EOF && len < MAX_STORED) 
        {
            len++;
        }
        fclose(fp);
    } else {
        printf("Failed to open file.\n");
    }
}

void write_high_scores() {
    FILE *fp = fopen("Score.txt", "w");
    if (fp != NULL) {
        for (int i = 0; i < len; i++) {
            fprintf(fp, "%s %d\n", high_score[i].name, high_score[i].score);
        }
        fclose(fp);
    } else {
        printf("Failed to open file.\n");
    }
}

void add_new_score() {
    int pos = -1;
    for (int i = 0; i < len; i++) {
        if (high_score[i].score < score) {
            pos = i;
            break;
        }
    }
    if (pos == -1 && len < MAX_STORED) {
        pos = len;
        len++;
    }

    if (pos != -1) {
        for (int i = len - 1; i > pos; i--) {
            high_score[i] = high_score[i - 1];
        }
        strcpy(high_score[pos].name, userInput);
        high_score[pos].score = score;
    }
}

void show_high_score()
{
    int y = 200;
    for (int i = 0; i < len; i++) {
        char scoreText[100];
        iSetColor(0,255,0);
        sprintf(scoreText, "%d. %s: %d", i + 1, high_score[i].name, high_score[i].score);
        iText(100, y, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);
        y -= 30;
    }
}

struct Obstacle {
    float x, y, width, height, speed, horiSpeed;
};
Obstacle obstacles[] = {
    {390, 200, 50, 10, 3, 1}, // Obstacle 1
    {570, 300, 60, 10, 2, 1}  // Obstacle 2
};
int numObstacles = sizeof(obstacles) / sizeof(obstacles[0]);

void CollisionWObs() {
    
    for (int i = 0; i < numObstacles; i++) {
        Obstacle obs = obstacles[i];

        // Check if the ball intersects with the obstacle
        if (ballX + ballRadius > obs.x && ballX - ballRadius < obs.x + obs.width &&
            ballY + ballRadius > obs.y && ballY - ballRadius < obs.y + obs.height) {

            //flag if decrease score
            vX = -vX;

            if (ballY < obs.y + obs.height / 2) {
                // Ball hit the top part of the obstacle
                vY = -fabs(vY); // Bounce upward slightly
            } else {
                // Ball hit the bottom part of the obstacle
                vY = fabs(vY); // Bounce downward slightly
            }

            // Decrease score, but don't let it go below 0
            /* if (score > 0) {
                score--;
            } */
        }
    }

    // Keep the ball within the game screen
    if (ballX - ballRadius < 305 || ballX + ballRadius > 670) {
       ballX = 305 + ballRadius;  // Ball hits the left or right wall
    }
}

void moveObstacles() {
    for (int i = 0; i < numObstacles; i++) {
        obstacles[i].y += obstacles[i].speed;

        // Reverse direction if the obstacle hits the screen boundary
        if (obstacles[i].y < 0 || obstacles[i].y + obstacles[i].height > screen_height) {
            obstacles[i].speed = -obstacles[i].speed;
        }

        if(currentLevel==4){
            // Zigzag movement (left to right and right to left)
            obstacles[i].x += obstacles[i].horiSpeed;

            // Reverse direction if the obstacle hits the left or right boundary
            if (obstacles[i].x < 305 || obstacles[i].x + obstacles[i].width > 670) {
                obstacles[i].horiSpeed = -obstacles[i].horiSpeed;
            }
        }
    }
}

void drawObstacles() {
    for (int i = 0; i < numObstacles; i++) {
        iFilledRectangle(obstacles[i].x, obstacles[i].y, obstacles[i].width, obstacles[i].height);
    }
}

void drawMenu() {
    iClear();
    iShowBMP(0, 0, "bgmain.bmp");
    iSetColor(0, 0, 0);

    if (selectedMenuOption == 0) {
        iRectangle(422, 256, 173, 50);  // Highlight Start Game
    } else if (selectedMenuOption == 1) {
        iRectangle(421, 191, 176, 48);  // Highlight Leaderboard
    } else if (selectedMenuOption == 2) {
        iRectangle(418, 120, 180, 48);  // Highlight Instructions
    } else if (selectedMenuOption == 3) {
        iRectangle(422, 53, 173, 50);  // Highlight Exit
    }
}

void drawLeaderboard(){
    iSetColor(0,0,0);
    iFilledRectangle(0,0,screen_width,screen_height);
    iSetColor(255,0,255);
    iText(100,300,"LEADERBOARD:-",GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(255,0,0);

    read_high_scores();
    show_high_score();
} 

void drawInstructions() {
    iClear();
    iSetColor(255, 255, 255);
    iText(50, 500, "Instructions", GLUT_BITMAP_TIMES_ROMAN_24);
    iText(50, 450, "1. Press SPACE to throw the ball.", GLUT_BITMAP_HELVETICA_18);
    iText(50, 400, "2. Aim for the hoop to score points.", GLUT_BITMAP_HELVETICA_18);
    iText(50, 350, "3. The game progresses to the level 2 when you score 5 points", GLUT_BITMAP_HELVETICA_18);
    iText(50, 300, "4. The game progresses to the level 3 when you score 10 points", GLUT_BITMAP_HELVETICA_18);
    iText(50, 400, "5. Press p to pause", GLUT_BITMAP_HELVETICA_18);
    iText(50, 350, "6. When paused, press p or r to return back to the game and esc to the menu.", GLUT_BITMAP_HELVETICA_18);
    iText(50, 300, "7. Enjoy the game!", GLUT_BITMAP_HELVETICA_18);

    iSetColor(255, 255, 255);
    iFilledRectangle(50, 50, 100, 50); // Back button
    iSetColor(0, 0, 0);
    iText(75, 65, "Back", GLUT_BITMAP_HELVETICA_18);
}

void drawBall() {
    iSetColor(255, 255, 255); // White color to create the ring effect
    iFilledEllipse(hoopX, hoopY, 35, 5); // Inner ellipse
}

void DrawOuterEllipse() {
    iSetColor(255, 0, 0); // Red color for the rim
    iFilledEllipse(hoopX, hoopY, 40, 10); // Outer ellipse
}

void drawHoopnet() {
    iFilledRectangle(hoopX + 40, hoopY - 28, 6, 100); // Backboard
    iSetColor(255, 255, 255); // White color for the net
    for (int i = 0; i <= 35; i += 7) {
        iLine(hoopX - 35 + i, hoopY - 10, hoopX - 20 + i, hoopY - 50); // Diagonal lines from left to right
        iLine(hoopX + 35 - i, hoopY - 10, hoopX + 20 - i, hoopY - 50); // Diagonal lines from right to left
    }
}

void drawBoundary() {
    iSetColor(255, 255, 255);
    iLine(305, 0, 305, screen_height);
    iLine(670, 0, 670, screen_height);
}

void drawGameOver() {
    iClear();
    iSetColor(0, 0, 0); // Black background
    iFilledRectangle(0, 0, screen_width, screen_height);
    
    iSetColor(255, 0, 0); // Red text
    iText(400, 300, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
    
    iSetColor(255, 255, 255); // White text for instructions
    iText(350, 250, "Press SPACE to try again", GLUT_BITMAP_HELVETICA_18);
    iText(350, 200, "Press ESC to return to menu", GLUT_BITMAP_HELVETICA_18);


    inputMode = true;
    if (inputMode) {
        iSetColor(255, 0, 0);
        iText(100, 450, "Enter your name: ", GLUT_BITMAP_TIMES_ROMAN_24);
        iText(300, 450, userInput, GLUT_BITMAP_TIMES_ROMAN_24);
    } 
}

void drawLevelComplete() {
    iClear();
    iSetColor(0, 255, 0); // Green text for completion
    char levelCompleteText[50];
    sprintf(levelCompleteText, "Level %d Complete!", currentLevel - 1);
    iText(350, 300, levelCompleteText, GLUT_BITMAP_TIMES_ROMAN_24);

    iSetColor(255, 255, 255);
    iText(300, 250, "Press SPACE to start the next level", GLUT_BITMAP_HELVETICA_18);
}

void drawPauseMenu() {
    iClear();  // Clear the screen

    iSetColor(0, 0, 0);  // Black background
    iFilledRectangle(0, 0, screen_width, screen_height);  // Fill the screen

    iSetColor(255, 0, 0);  // Red text color
    iText(350, 500, "PAUSED", GLUT_BITMAP_TIMES_ROMAN_24);  // Display "PAUSED" text

    // Option 1: Resume Game
    iSetColor(255, 255, 255);  // White color for button
    /* iFilledRectangle(350, 300, 200, 50);  // Resume button
    iSetColor(0, 0, 0);  // Black text for button label */
    iText(390, 315, "Press R to Resume", GLUT_BITMAP_HELVETICA_18);

    // Option 2: Return to Menu
    iSetColor(255, 255, 255);
    //iFilledRectangle(350, 200, 200, 50);  // Back to Menu button
    iText(390, 215, "Press ESC to return to menu", GLUT_BITMAP_HELVETICA_18);
}

bool yes = false;
void projectile_activator() {
    if (!paused) { // Only activate projectile logic if not paused
        if (inAir) {
            // Time step (should match the timer interval)
            float t = 0.25;

            // Update vertical position and velocity
            ballY += vY * t - 0.5 * g * (t * t); // Vertical motion with gravity
            vY -= g * t; // Update vertical velocity due to gravity

            // Prevent the ball from going below the ground
            if (ballY <= 50) {
                ballY = 50;
                vY = -vY * 0.6; // Bounce with energy loss
            }

            //printf("ballX = %d, ballY = %d, vY = %d\n",ballX, ballY, vY);

            if(ballX>= 582 && ballX <= 585 && ballY>=375)
            {
                yes = true;
                //printf("yes\n");
            }
            
            // Scoring conditions
            if (ballX >= hoopX - 30 && ballX <= hoopX + 30 &&
                ballY >= hoopY - 20 && ballY <= hoopY -2 && yes) {
                
                scored = true;
                score++;
                //printf("noice\n");
                test_case = false;
                yes = false;
            }

            // Collision with the hoop rim
            if ((ballX >= hoopX - 60 && ballX <= hoopX - 20 &&
                 ballY >= hoopY - 10 && ballY <= hoopY + 10)) {
                vY = v - 10;
                // scored = false;
            }

            // Horizontal motion
            ballX += 3*0.99;
            if (ballX + ballRadius > 670) {
                ballX = 305 + ballRadius; // Reset ball position (if necessary)
            }
        }
    }
}


void toggleTimerPause(){
    timerPaused=!timerPaused;
}

void updateTimer() {
    if (!paused && currentTime > 0) {
        currentTime--;

        // Extra 5 seconds logic for Level 1
        if (currentLevel == 1 && score == 3 && extraTime == 0) {
            //printf("Score: %d, ExtraTime: %d, CurrentTime: %d\n", score, extraTime, currentTime);
            extraTime = 5; // Mark that the bonus has been given
            currentTime += 5; // Add 5 seconds
        }

        if (currentTime <= 0) {
            timeOver = true;
            currentState = GAME_OVER;
        }
    }
}

void decrementTimer() {
    if (gameStarted && !paused && currentTime > 0 && currentState.state==PLAY.state) {
        currentTime--; // Decrement time when not paused
    } else if (currentTime == 0) {
        timeOver = true; // Timer is up
        currentState = GAME_OVER; // Transition to the Game Over state
        inputMode = true;
    }
}

void playGame() {
    iClear();
    iShowBMP(0, 0, "bg.bmp");
    DrawOuterEllipse();
    drawBall();
    drawHoopnet();
    drawBoundary();
    if (currentLevel == 3 || currentLevel == 4) {
        drawObstacles();
        moveObstacles();
        CollisionWObs();
    }
    iSetColor(255, 165, 0); // Orange color
    iFilledCircle(ballX, ballY, ballRadius);
    iSetColor(255, 255, 0);

    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    iText(10, 570, scoreText, GLUT_BITMAP_TIMES_ROMAN_24);

    char timerText[50];
    sprintf(timerText, "Time Left: %d seconds", currentTime);
    iText(750, 570, timerText, GLUT_BITMAP_TIMES_ROMAN_24);

    // Display the level
    char levelText[50];
    sprintf(levelText, "Level: %d", currentLevel);
    iText(750, 530, levelText, GLUT_BITMAP_TIMES_ROMAN_24);

    // Check level progression
    if (score >= levelScoreThreshold) {
        currentLevel++;
        currentState = LEVEL_COMPLETE;
        // Adjust gameplay dynamics for the new level
        if (currentLevel == 2) {
            level2();
        } else if (currentLevel == 3) {
            level3();
        } else if (currentLevel == 4) {
            level4();
        }
        resetGameState(); // Reset variables for the new level
}

    if (paused) {
    iText(400, 300, "RESUME (R)", GLUT_BITMAP_TIMES_ROMAN_24);
    iText(400, 270, "RESTART (S)", GLUT_BITMAP_TIMES_ROMAN_24);
    iText(400, 240, "QUIT (Q)", GLUT_BITMAP_TIMES_ROMAN_24);
    }

    if (scored) {
        iSetColor(0, 0, 255);
        iText(350, 500, "SCORE!!", GLUT_BITMAP_TIMES_ROMAN_24);
    }

}

void level1()
{
    g = 9.8;
    totalTime=45;
    currentTime = totalTime;
    levelScoreThreshold=2;
}

void level2()
{
    g = 11.0;
    t=.30;
    totalTime=30;
    currentTime=totalTime;
    levelScoreThreshold=2;
}

void level3()
{
    totalTime = 25;
    currentTime=totalTime;
    levelScoreThreshold=2;
}

void level4()
{
    totalTime = 25;
}

void resetGameState() {
    score = 0;
    ballX = 325;
    ballY = 100;
    currentTime = totalTime;
    timeOver = false;
    scored = false;
    extraTime = 0;
    paused = false;
    inAir = false;
}

void iDraw() {
    if (currentState.state == MENU.state) {
        drawMenu();  // Main menu
    } else if (currentState.state == LEADERBOARD.state) {
    drawLeaderboard();
    } else if (currentState.state == INSTRUCTIONS.state) {
        drawInstructions();  // Instructions screen
    } else if (currentState.state == PLAY.state) {
        playGame();  // Gameplay
    } else if (currentState.state == PAUSED.state) {
        drawPauseMenu();  // Pause menu
    } else if (currentState.state == LEVEL_COMPLETE.state) {
        drawLevelComplete();
    } else if (currentState.state == PAUSED.state) {
        drawPauseMenu();  // Pause menu
    } else if (currentState.state == GAME_OVER.state) {
        drawGameOver();  // Game Over screen

        if(printingState) {
            read_high_scores();
            add_new_score();
            // write_high_scores();
            printingState = false;
        }
            if(name_entered){
                iSetColor(255,255,255);
                iText(50,50,"NAME ENTERED",GLUT_BITMAP_TIMES_ROMAN_10);
            }
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

        if(mx>=436 && mx<=556 && my>=225 && my <= 255)
        {
            currentState = PLAY;
            gameStarted = true;
        }

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

void iKeyboard(unsigned char key) {
    
    if (currentState.state == MENU.state) {
        currentLevel = 1;
        if ((currentState.state == PLAY.state) || (currentState.state == PAUSED.state) || currentState.state == GAME_OVER.state || currentState.state == LEADERBOARD.state) {
        if (key == 27) {
            currentState = MENU; // Switch to MENU state       
            resetGameState();
            inAir = false;
            currentLevel= 1;
        }
    }
    if (key == '\r') { // Enter key to select the highlighted menu option
        if (selectedMenuOption == 0){
            currentState = PLAY; // Start Game
            gameStarted = true;
        }
        else if (selectedMenuOption == 1) 
        {
            currentState = LEADERBOARD; // Show Leaderboard   
        }
            
        else if (selectedMenuOption == 2) currentState = INSTRUCTIONS; // Show Instructions
        else if (selectedMenuOption == 3) exit(0); // Exit Game
    }
    }


    if (currentState.state == PLAY.state) {
        if (key == ' ') {  // Space to throw the ball
            if (!paused) {  // Only allow throwing the ball if the game is not paused
                inAir = true;
                vY = v;
                scored = false;
            }
        } else if (key == 'p') {  // 'P' key to pause or resume the game
            paused = !paused;  // Toggle the pause state

            if (paused) {
                currentState = PAUSED;  // Switch to PAUSED state
            } else {
                currentState = PLAY;  // Resume the game
                gameStarted = 1;
            }
        }
    } else if (currentState.state == PAUSED.state) {
        if (key == 'r') {  // Resume game when 'r' is pressed
            paused = false;
            currentState = PLAY;  // Resume the game
        } else if (key == 27) {  // ESC to return to menu
            paused = false;  // Unpause the game before returning to the menu
            currentState = MENU;  // Switch to MENU state
            
            // Reset the game state when returning to menu
            resetGameState();
            //inAir = false;
            currentLevel = 1;
        }
    } else if (currentState.state == GAME_OVER.state) {
        if (key == ' ') {  // Space to restart the game
            // Reset game state as if starting fresh
            resetGameState(); // Ensure the game is unpaused
            
            currentLevel=1;
            currentState = PLAY;  // Start the game
            inputMode = false;
        } else if (key == 27) {  // ESC to return to menu
            currentState = MENU;  // Return to menu
            
            resetGameState();
        }
    }
    else if(currentState.state == LEADERBOARD.state)
    {
        if(key == 27)
        {
            currentState = MENU;
            // resetGameState();
        }
    }
    else if(currentState.state == INSTRUCTIONS.state)
    {
        if(key == 27)
        {
            currentState = MENU;
            // resetGameState();
        }
    }
    else if (currentState.state == LEVEL_COMPLETE.state) {
        if (key == ' ') {
            currentState = PLAY;
        }
    }
        if (inputMode) {
        if (key == '\r') { // Enter key to finalize input
            // printf("pressed Q");
            inputMode = false;
            name_entered = true;
            read_high_scores();
            add_new_score();
            write_high_scores();
            
        } else if (key == '\b') { // Backspace key to delete character
            if (inputIndex > 0) {
                inputIndex--;
                userInput[inputIndex] = '\0';
            }
        } else if (inputIndex < MAX_NAME_LENGTH - 1) {
            userInput[inputIndex] = key;
            inputIndex++;
            userInput[inputIndex] = '\0';
        }
    }
}

void iSpecialKeyboard(unsigned char key) {
    if (currentState.state == MENU.state) {
        if (key == GLUT_KEY_UP) {
            selectedMenuOption = (selectedMenuOption - 1 + 4) % 4;  // Navigate up, 4 options now
        } else if (key == GLUT_KEY_DOWN) {
            selectedMenuOption = (selectedMenuOption + 1) % 4;  // Navigate down, 4 options now
        }
    } else if (currentState.state == PLAY.state) {
        if (key == GLUT_KEY_LEFT) {
            ballX -= 10;  // Move ball left
        } else if (key == GLUT_KEY_RIGHT) {
            ballX += 10;  // Move ball right
        }
    }
}


int main()
{
    iSetTimer(10, projectile_activator);
    iSetTimer(1000, decrementTimer);

    iInitialize(screen_width, screen_height, "Basket Buffer - Level One"); // Initialize the graphics window
}