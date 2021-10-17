#ifndef _PLAYSNAKE_H_
#define _PLAYSNAKE_H_

#include <Arduino.h>
#include "Adafruit_GFX.h"    // Core graphics library
#include "Adafruit_ILI9341.h" // Hardware-specific library
#include <TouchScreen.h>

extern Adafruit_ILI9341 tft; //Allow playSnake to access tft commands and touchscreen commands
extern TouchScreen ts;

//Screen size definitions
#define TFT_WIDTH 320
#define TFT_HEIGHT 240

//Touchscreen definitions
#define MINPRESSURE 10
#define MAXPRESSURE 1000
//Joystick definitions
#define JOY_VERT_ANALOG A1
#define JOY_HORIZ_ANALOG A0
#define JOY_SEL 8
#define JOY_DEADZONE 64
#define JOY_CENTER 512
#define JOY_STEPS_PER_PIXEL 64

// Game definitions
#define SNAKE_COLOR 0xA6C3 
#define ENEMY_COLOR 0xF800
#define BORDER_COLOR 0x001F
#define BORDER_WIDTH 5
#define PELLET_COLOR 0x07FF

#define BODYSIZE 10

#define GRID_WIDTH 31
#define GRID_HEIGHT 21

#define up 0 //Direction snake is moving
#define right 1
#define down 2
#define left 3

class Snake {   
public: 

    // draws snake at a given coordinate, with a given color
    void drawSnake(int x, int y, int color);

    // moves the snake by the x and the y scale; if yscale = 1 and xscale = 0, the snake moves one unit down
    void moveSnake(int xScale, int yScale, int colors, Snake& player, Snake& computer, int snakeType);

    // snake definitions 
    int snakeBody[GRID_WIDTH][GRID_HEIGHT]; // snakebody[i][j] = 0 if snakebody is not in the map coordinate
                                            // snakebody[i][j] = 1 if snakebody is placed in that position
    int snakeX[200]; // array of snake's x positions
    int snakeY[200]; // array of snake's y positions
    int length; // length of snake
};

class Pellet  {
public:

    // draws pellet at x and y of the map coordinate
    void drawPellet(int x, int y);

    // places pellet where the player and the computer are not positioned
    // sends the coordinate of the snake and the pellet to communicate with server as well
    void placePellet(Snake& player, Snake& computer);
};

// the "main" function of the snake where all the functions are called
void playSnake(Snake& snake, Snake& computer, Pellet& pellet);


#endif
