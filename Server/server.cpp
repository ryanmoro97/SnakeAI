#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <list>
#include "dijkstra.h"
#include "wdigraph.h"
#include "server_util.h"
#include "serialport.h"
#include "bstmap.h"
using namespace std;

// balances the trees of given names with scores
//remove and reinsert 2 items such that the insertion organizes tree with Olog(n) search time
void balanceTree(BSTMap<int, string>& tree){
  int score1 = tree.minScore();
  string name1 = tree.minName();
  tree.remove(score1);
  int score2 = tree.minScore();
  string name2 = tree.minName();
  tree.remove(score2);
  tree[score1] = name1;
  tree[score2] = name2;
}

// updates the scores that the program recieves
void updateScores(BSTMap<int, string>& tree, int score, string name){
    //max 5 scores, so if scores arent maxed -> add score
    if(tree.size() < 5){
        tree[score] = name;
    }
    //if there are 5 scores check if it should be on scoreboard
    //if it should be then remove min, insert new
    else if(score > tree.minScore()){
        int oldScore = tree.minScore();
        tree.remove(oldScore);
        tree[score] = name;
        //check if new tree is balanced, if not -> rebalance
        if(!tree.isBalanced()){
          balanceTree(tree);
        }
    }
}

// sends the scores and name in order to the client to display highscores
void sendScores(const BSTMap<int, string>& tree) {
    SerialPort Serial("/dev/ttyACM0");
    //print scores in order to be parsed and displayed
    for (BSTIterator<int, string> iter = tree.begin(); iter != tree.end(); ++iter) {
        cout << iter.key() << "," << iter.item() << "," << endl;
        string current = "," + to_string(iter.key()) + "," + iter.item() + ",";
        Serial.writeline(current);
    }
    //send "end" acknowledgement so client knows server is done sending scores
    Serial.writeline("E");
}

int main()  {
    SerialPort Serial("/dev/ttyACM0"); //initialize serial port
    unordered_map<int, Point> points; //map to store vertex ID with x,y coordinates
    WDigraph graph; //graph that we will be building with vertices and edges
    BSTMap<int, string> scores; //highscore bst 

    int level = 1;
    string name;
    string line, ack;
    writeGraph(level, points); //create level 1 grid graph text file 
    readGraph("level.txt", graph, points); //create wDigraph from parsing text file
    Point sPoint, ePoint; //Points (x,y) of beginning and end of path to find
    
    while (true) {
        line = "A"; //avoid misread line from serial at start
        while(line[0] == 'A' || line[0] == 'P'){ //eats up instruction when required and finds next
            line = Serial.readline(0); //next instruction
            cout << "line: " << line << endl;
            if (line[0] == 'E') { //End of game
                line = Serial.readline(0); //read score
                // cout << "score: " << line << endl;
                int score = stoll(line);
                string name = Serial.readline(0); //read name
                name = name.substr(0,3); //get rid of garbage on end

                updateScores(scores, score, name); //check if score goes on highscore board
                sendScores(scores); //send updated BST to client to display
                line = "A"; //reset instruction and wait for new one
            }
        }

        int i = 0;
        string pointsInfo[4]; // pX, pY, computer head X, computer head Y
        for(char& c : line){ //parse 4 items by comma
            if(c == '\n'){
                break;
            }
            if(c != ','){
              pointsInfo[i].push_back(c);
              }
            else if (c == ',')  {
              i++;
            }
        }
        ePoint.x = stoll(pointsInfo[0]); //pX
        ePoint.y = stoll(pointsInfo[1]); //pY
        sPoint.x = stoll(pointsInfo[2]); //computer head X 
        sPoint.y = stoll(pointsInfo[3]); //computer head Y
        int start = findVertex(sPoint, points); //find starting point that computer is at
        int end = findVertex(ePoint, points); //find new x,y of pellet which is the computers new endpoint
        unordered_map<int, PII> tree; //create search tree for dijkstra to return
        dijkstra(graph, start, tree); //find new path 

        //reverse path to order start to end
        list<int> path;
        while(end != start){
            path.push_front(end);
            end = tree[end].second;
        }

        for(auto v: path){ //send all the coordinates of the snakes new path in order
            ack = Serial.readline(0); //wait for acknowledgement from server before sending next x,y
            //this avoids overflowing the serial monitor with instructions
            // cout << "ack: " << ack << endl;
            if(ack[0] == 'A'){ //received acknowledgement
                //get x,y of next vertex in path
                int x = points.at(v).x; 
                int y = points.at(v).y;
                //create string of next x,y to send
                string next_coord = to_string(x)+','+to_string(y)+',';
                // cout << next_coord << endl;
                Serial.writeline(next_coord);
            }  
            else if (ack[0] == 'P') { //pellet eaten, break to find new path
                break;
            }
            else if (ack[0] == 'E') { //game over, break to send scores before restarting game
                break;
            }
        }

    }    
    return 0;
}