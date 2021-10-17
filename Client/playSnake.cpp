#include "playSnake.h"

int score = 0; 
bool pelletPlaced; // determines whether to place pellet or not
bool gameOver; //runs appropriate function if collision is detected

int pX = 15;// initial position of pellets
int pY = 5;

// draws three letters of name in the end game screen
void drawLetters(int* name){
    tft.fillRect(160,110,80,16,0);
    tft.setTextSize(2);
    for(int i = 0; i < 3; i++){
        char c = name[i];
        tft.setCursor(160+i*20, 110);
        tft.print(c);
    }   
}

// sends the names recorded to the server
void sendName(int* letters){
    char name[3];
    for(int i = 0; i < 3; i++){
        char c = letters[i];
        Serial.print(c);
    }
    Serial.println();//name);   
}

void printGameOver() {
    tft.fillRect(BORDER_WIDTH-1, 5 * BORDER_WIDTH -1, TFT_WIDTH - BORDER_WIDTH * 2,
                         TFT_HEIGHT - BORDER_WIDTH * 6, 0x0000); // draw black background
    tft.setCursor(75,40);
    tft.setTextSize(3);
    tft.setTextColor(0xFFFF);
    tft.print("Game Over!");

    tft.setTextSize(2);
    tft.setCursor(110,80);
    tft.print("score: ");
    tft.setCursor(190,80);
    tft.print(score);

    tft.setCursor(100,110);
    tft.print("Name: ");

    tft.setTextSize(1);
    tft.setCursor(100,180);
    tft.print("- Enter a name -");
}

// prints the game over screen and prompts user to enter a name
void endGame(){
    int name[3] = {65,65,65}; //name chars
    printGameOver();

    int checkJoy; // for checking joystick position
    int select = digitalRead(JOY_SEL); //reading of joystick click
    drawLetters(name);  
    while(true){
        int i;
        for(i = 0; i < 3; i++){
            while(select == HIGH){
                checkJoy = analogRead(JOY_VERT_ANALOG); // check analog stick
                select = digitalRead(JOY_SEL);
                if(checkJoy < JOY_CENTER - JOY_DEADZONE){ // up, increase ascii
                    name[i]++;
                    if(name[i] == 91){ // if reaching Z, go back to A
                        name[i] = 65;
                    }
                    drawLetters(name);
                    delay(300); // delay to avoid multiple scrolls
                }
                else if(checkJoy > JOY_CENTER + JOY_DEADZONE){ // down, decrease ascii
                    name[i]--;
                    if(name[i] == 64){ // if reaching A, go back to Z
                        name[i] = 90;
                    }
                    drawLetters(name);
                    delay(300);
                }
            }
            while(select == LOW){
                select = digitalRead(JOY_SEL);
                delay(100);
            } //wait until value is high again to avoid double click
        }
        if(i == 3){ // send name to server when done selecting name
            sendName(name);
            break;
        }
    }

    tft.fillRect(100,180,100,15,0);
    tft.setTextSize(1);
    tft.setCursor(40,180);
    tft.print("- Touch Screen to Save Name & Restart -");
    
    while(true){
		TSPoint touch = ts.getPoint(); //Set gameOver to true when screen is touched, so code returns to title screen
		if(touch.z > MINPRESSURE && touch.z < MAXPRESSURE){
			gameOver = true;
			pelletPlaced = true; //Prevents pellet from being drawn on end screen
			break;
		}
	}
}

// Resets variables on begin of play and draws time and score counter
void resetGame(Snake& player, Snake& computer){
	player.length = 3; // initial length of snakes are 3
    computer.length = 3;

	score = 0;
	tft.setTextColor(0xFFFF,BORDER_COLOR); //Color for score text
	tft.setTextSize(2);// size for score
	tft.setCursor(20,5);
	tft.print("TIME:");
	tft.print("0");
	tft.setCursor(195,5); 
	tft.print("SCORE:");
	tft.setCursor(270, 5);
	tft.print(score);
	tft.setTextSize(1); //reset text size

	pelletPlaced = false; //start with no pellets
	for(int i=0; i<GRID_WIDTH; i++){
		for(int j=0; j<GRID_HEIGHT; j++){ //Clear grid snakes will be on
			player.snakeBody[i][j] = 0;
            computer.snakeBody[i][j] = 0;
		}
	}
	
	gameOver = 0; //Go back to title when gameOver = 1
}

// checks for player's collision by checking for boundaries and running into itself
bool collision(Snake& player, int xScale, int yScale){
	if((player.snakeX[0] + xScale > GRID_WIDTH -1) || (player.snakeX[0] + xScale < 0) ||
		 (player.snakeY[0] + yScale > GRID_HEIGHT -1) || (player.snakeY[0] + yScale < 0) ||
			 player.snakeBody[player.snakeX[0]+ xScale][player.snakeY[0] + yScale] == 1){
             return true;
	}
	return false; 
	// return gameOver;
}

void Snake::drawSnake(int x, int y, int color){ //scales grid to screen and draws square
	tft.fillRect(x*10+4,y*10+24,BODYSIZE,BODYSIZE,color);
}

void Snake::moveSnake(int xScale, int yScale, int color,Snake& player, Snake& computer, int snakeType){
	// gives priority of drawing the tale depending on the parameter 's'
    // this is implemented to avoid bugs with snake bodies drawing messy and wrong when on top of each other
    if(snakeType == 0){ // give priority to the player
        player.drawSnake(snakeX[length-1], snakeY[length-1], 0x0000); //Draws black where snakes tail was
    }
	else{ // give priority to the computer
        if(player.snakeBody[snakeX[length-1]][snakeY[length-1]] != 1){
            computer.drawSnake(snakeX[length-1], snakeY[length-1], 0x0000);
        }
    }
    snakeBody[snakeX[length-1]][snakeY[length-1]] = 0; //Sets tail coordinate to 0
	
    //Sets each snake body coordinate to the one that was ahead of it and ensures that coordinate is set to 1 to avoid bugs
	for(int i=length-1; i>0; i--){
			snakeX[i] = snakeX[i-1]; 
			snakeY[i] = snakeY[i-1];
			snakeBody[snakeX[i]][snakeY[i]] = 1;
	}

	snakeX[0] += xScale; // moves head of snake in direction specified by joystick input or path from server
	snakeY[0] += yScale;

    // same idea as above, but with the snake heads
	if(snakeType == 0){
        player.drawSnake(snakeX[0], snakeY[0], color);
    }
    else{
        if(player.snakeBody[computer.snakeX[0]][computer.snakeY[0]] != 1){
            computer.drawSnake(snakeX[0], snakeY[0], color);
        }
    }
	snakeBody[snakeX[0]][snakeY[0]] = 1; //draw new snake head and put a 1 on corresponding grid cell
}

void Pellet::drawPellet(int x, int y){
	tft.fillRect(x*10+6,y*10+26,6,6,PELLET_COLOR); //scale pellet grid positon to screen
}

void Pellet::placePellet(Snake& player, Snake& computer) {
	if(!pelletPlaced){ //place pellet in random position where the snake is not currently at
		pX = random(1,30);
        pY = random(1,20);
        //if new position is under snake find a new one
        while(player.snakeBody[pX][pY]==1 || computer.snakeBody[pX][pY]==1){
            pX = random(1,30);
			pY = random(1,20); 

		}
		drawPellet(pX,pY);

        // send the pellet and snake coordinates to the server to find new path 
        Serial.println("P");
		Serial.print(pX);
		Serial.print(",");
		Serial.print(pY);
		Serial.print(",");
		Serial.print(computer.snakeX[0]);
		Serial.print(",");
		Serial.print(computer.snakeY[0]);
		Serial.print("\n");
		pelletPlaced = true;
    }
}

// draws score on screen
void viewScore()   {
    tft.fillRect(270,5,80,14, BORDER_COLOR);
    tft.setCursor(270, 5);
    tft.setTextSize(2);
    tft.setTextColor(0xFFFF,BORDER_COLOR);
    tft.print(score);
    tft.setTextSize(1);
}

// set up initial positions of snakes' heads
void setupSnake(Snake& player, Snake& computer, Pellet& pellet) {
    player.snakeBody[player.snakeX[0]][player.snakeY[0]] = 1;
    computer.snakeBody[player.snakeX[0]][player.snakeY[0]] = 1;
    player.snakeX[0] = GRID_WIDTH/2 -10;
    player.snakeY[0] = GRID_HEIGHT/2;
    computer.snakeX[0] = GRID_WIDTH/2 + 10;
    computer.snakeY[0] = GRID_HEIGHT/2;
}

void playSnake(Snake& player, Snake& computer, Pellet& pellet){
	resetGame(player, computer); // reset variables
	long startTime = millis(); // time for duration of game
	
	int xScale = 0;
	int yScale = -1; //Set player's initial direction upwards
	int compXscale = 0;
	int compYscale = 0; // Set computer's initial direction to nowhere

    int s;
	int d = up; // direction of player
	int prevD = d; // previous direction of player to avoid running back to itself

    setupSnake(player, computer, pellet); // set up snakes' positions
	player.drawSnake(player.snakeX[0], player.snakeY[0], SNAKE_COLOR);
    computer.drawSnake(computer.snakeX[0], computer.snakeY[0], ENEMY_COLOR);

	while(true){

		int v = analogRead(JOY_VERT_ANALOG); // check vertical and horizontal positions of stick
		int h = analogRead(JOY_HORIZ_ANALOG);

		pellet.placePellet(player, computer);

        // GAME OVER if there is a collision, or if score goes below -5
        if (collision(player, xScale, yScale) == true || score < -5)    {
            
            Serial.println("E"); // tell server that game is over to break out of its loop
            while(Serial.available())   { // eat up all remaining bytes incoming to avoid bugs
                Serial.read();
            }
            Serial.println("E"); // tell the server once again that the game is over to get score
            Serial.println(score); // send score to server
            gameOver = 1;
            endGame(); // gameover screen
            break;
        }

        int a,b;
        delay(10);
        Serial.println("A"); // send acknowledgement to server
    	while(Serial.available() > 0){ // recieve the path for the computer from server
            a = Serial.parseInt();
            compXscale = a-computer.snakeX[0]; // determine which direction the snake should go
            b = Serial.parseInt();
            compYscale = b-computer.snakeY[0];
    		break;
    	}

		if((v > JOY_CENTER + JOY_DEADZONE) && (prevD != down)){ //sets direction unless it is opposite of previous direction
			d = up; // set directions
			xScale = 0;
			yScale = -1;
		}
		else if((h > JOY_CENTER + JOY_DEADZONE) && (prevD != left)){
			d = right;
			xScale = 1;
			yScale = 0;
		}
		else if((v < JOY_CENTER - JOY_DEADZONE) && (prevD != up)){
			d = down;
			xScale = 0;
			yScale = 1;
		}
		else if((h < JOY_CENTER - JOY_DEADZONE) && (prevD != right)){
			d = left;
			xScale = -1;
		    yScale = 0;
		}
        prevD = d; // set current direction to previous direction
		
        delay(100);
        player.moveSnake(xScale,yScale,SNAKE_COLOR,player,computer, s = 0); //move snake every .1 seconds              
        computer.moveSnake(compXscale,compYscale,ENEMY_COLOR,player,computer, s = 1);

        // increase score and length when player eats the pellet
        if(player.snakeX[0] == pX && player.snakeY[0] == pY){
            score+=5;
            viewScore(); // update new score on screen
            if(player.length<200){ //don't let length exceed 200
                player.length+=1;
            }
			pelletPlaced = false;
		}
		else if(computer.snakeX[0] == pX && computer.snakeY[0] == pY){ // opposite when computer eats pellet
			computer.snakeBody[pX][pY] = 2;
            score--;
            viewScore();
			pelletPlaced = false;
		}

		tft.setTextColor(0xFFFF,BORDER_COLOR); //Color for score text
		tft.setTextSize(2);// size for score
		tft.setCursor(20,5); //update timer
		tft.print("TIME:");
		tft.print((millis()-startTime)/1000);
		tft.setTextSize(1);
	}
}