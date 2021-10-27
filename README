# SnakeAI

A classic snake game, but the player will be playing against an "AI" that will find the shortest path to the pellet. 
The map has a size of 31x21, and you need to try to get more pellets than the snake without dying. 
If the enemy snake gets the pellets first, you will lose points! 

The game was implemented using graphs, and assigned vertices and edges throughout the map like a grid, 
and dikjstra's algorithm is used by the computer to calculate the shortest path.

The game is made using communications between the server (computer) and the client (arduino). 
    - The server calculates/sends the shortest paths and uses BST to organize scores
    - The client plays the game and sends the coordinates/scores+names


Running Instructions:
    1. Make upload the snakegame to the arduino from the Client file in the terminal by typing "make upload"
    2. Make the server in the Server file by typing "make server"
    3. Run the server by typing "./server"

Assumptions:
    - We could have used other searches such as breadth first search or depth first search since all the cost is the same in our game.
    - If queues were used, might have had better runtime, but not by much since the map is so small.

Required Components:
    - Arduino Mega
    - Joystick
    - adafruit tft display

Wiring Instructions:

TFT Display
Board Pin <---> Arduino Pin
===========================
GND             GND
Vin             5V
3Vo             NOT CONNECTED
CLK             52
MISO            50
MOSI            51
CS              10
D/C             9
RST             NOT CONNECTED
Lite            NOT CONNECTED
Y+              A2 (analog pin)
X+              4  (digital pin)
Y-              5  (digital pin)
X-              A3 (analog pin)
IM0 - IM3       NOT CONNECTED (they expect a 3.3v signal, DON'T CONNECT!)
CCS             6
CD              NOT CONNECTED

Joystick
Board Pin <---> Arduino Pin
===========================
GND             GND
+5V             5V
VRx             A1
VRy             A0
SW              8
