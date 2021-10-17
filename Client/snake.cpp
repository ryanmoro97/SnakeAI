#include "playSnake.h"


#define TFT_DC 9 //initialize tft display pins
#define TFT_CS 10

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC); //tft screen

// touch screen pins, obtained from the documentaion
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM  5  // can be a digital pin
#define XP  4  // can be a digital pin

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300); //Setup touchscreen


bool mode=0; // variable for running menu or game

void setup(){
	init(); 

	pinMode(JOY_SEL, INPUT); //joystick select input
	digitalWrite(JOY_SEL, HIGH); //initialize as HIGH

	Serial.begin(9600); //initialize serial monitor for communication

	tft.begin();
	
	tft.setRotation(-1); //rotation for landscape tft
}

// Draw title
void drawTitle(){
	tft.fillScreen(0);
	tft.setTextSize(4);
	tft.setCursor(90,40);
	tft.setTextColor(SNAKE_COLOR);
	tft.print("Snake!");
	
	tft.setTextSize(1);
}

// draws title and the highscores recieved from server
void startScreen(){
	int color = 0x0000;
	long timer = millis(); //long is used to avoid overflow
	char name[4];
	int i = 0;
	drawTitle();

	// read in name and score from the server
	while(Serial.available() > 0){
		char c = Serial.read();

		if (c == 'E')	{ // break when names and scores are done sending
			break;
		}

		// read in score and print to screen
        int score = Serial.parseInt();
        tft.setCursor(160,160-i*15);
        tft.print(score);

        // read in name and store to name
        for (int j = 0; j < 4; j++)	{
        	char c = Serial.read();
        	if (c != ',')	{
        		name[j-1] = c;
        	}

        }

        // print name to screen
	    tft.setCursor(130,160-i*15);
        tft.print(name);
        i++;
        c = Serial.read();
    }

	// wait for touch input while flickering instruction on screen
	while(true){
		if((color == 0xFFFF) && (millis() > timer + 1000)){
			color = 0x0000;
			timer = millis(); //Flicker text to black and white
		}
		else if((color == 0x0000) && (millis() > timer + 500)){
			color = 0xFFFF;
			timer = millis();
		}
	
		tft.setTextColor(color); //Print text
		tft.setCursor(80,180);
		tft.print("- Touch Screen to Start -");
		
		//once touched advance to game
		TSPoint touch = ts.getPoint();
		if(touch.z > MINPRESSURE && touch.z < MAXPRESSURE){
			mode=1;
			break;
		}
	}
}

// Draw arena for snake
void drawGameScreen(){ 
	tft.fillScreen(BORDER_COLOR);
	tft.fillRect(BORDER_WIDTH-1, 5 * BORDER_WIDTH -1, TFT_WIDTH - BORDER_WIDTH * 2, TFT_HEIGHT - BORDER_WIDTH * 6, 0x0000);
}

// draw gamescreen then start the game
void startGame(Snake& snake, Snake& computer, Pellet& pellet){
	drawGameScreen(); //Print background then run input checking code
	playSnake(snake, computer, pellet); 
}

//main game function
int main(){
	Snake player;
	Snake computer;
	Pellet pellet;
	setup(); 
	while(true){
		if(mode == 0){ //switch between title screen and game
			startScreen();
		}
		else if (mode == 1){
			startGame(player, computer, pellet);
			mode = 0;
		}
	}
	return 0;
}
