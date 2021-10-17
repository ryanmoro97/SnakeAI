#include <cassert>
#include <cstdlib>
#include <fstream>
#include "server_util.h"
#include <iostream>
using namespace std;

//////ADDED CODE/////
// returns the manhattan distance between two points
// will always return 1 between adjacent vertices
long long manhattan(const Point& pt1, const Point& pt2) {
  return abs(pt1.x-pt2.x) + abs(pt1.y-pt2.y);
}

//////ADDED CODE/////
// finds the id of the point that belongs to the point with given x,y
int findVertex(const Point& pt, const unordered_map<int, Point>& points) {
  for (const auto& check : points) {
    if (pt.x == check.second.x  && pt.y == check.second.y){
      return check.first;
    }
  }
}

//////ADDED CODE/////
//build the graph in a text file according to level
void writeGraph(int level, unordered_map<int, Point>& points){
  ofstream outfile ("level.txt");
  int count = 0; // counter for vertex ID

  // makes vertices
  for (int i = 1; i < 22; i++)    {
      for (int j = 1; j < 32; j++)    {
          outfile << "V" << "," << count << "," << j << "," << i << endl;
          points[count].x = j;
          points[count].y = i;
          count++;
      }
  }
  // makes edges
  for (int i = 0; i < 651; i++)   {
      // directions based on the vertex IDs
      int left = i-1;
      int right = i+1;
      int up = i-31;
      int down = i+31;

      // adds edges to its surrounding points
      // checks if a point is at the edge of the map or not to avoid adding edges outside of the map
      if (points[left].x > 0 && points[i].x > points[left].x)   {
          outfile << "E," << i << "," << left << endl;
      }
      if (points[right].x < 32 && points[i].x < points[right].x)   {
          outfile << "E," << i << "," << right << endl;
      }
      if (points[up].y > 0 && points[i].y > points[up].y)   {
          outfile << "E," << i << "," << up << endl;
      }
      if (points[down].y < 22 && points[i].y < points[down].y)   {
          outfile << "E," << i << "," << down << endl;
      }
  }    
  outfile.close();
}

//////ADDED CODE/////
// Read the graph from a file that is previously created
// store the weighted graph in g and the mapping
// of vertex IDs to x,y coordinates in "points".
// Will not clear g nor points beforehand.
void readGraph(const string& filename, WDigraph& g, unordered_map<int, Point>& points) {
  ifstream fin(filename);
  string line;

  while (getline(fin, line)) {
    // split the string around the commas, there will always be 4 substrings
    string p[4];
    int at = 0;
    for (auto c : line) {
      if (c == ',') {
        // start new string
        ++at;
      }
      else {
        // append character to the string we are building
        p[at] += c;
      }
      // don't want to index out of bounds!
      assert(at < 4);
    }

    if (p[0] == "V") {
      // new vertex

      // get vertex ID
      int id = stoi(p[1]);

      // get x/y of the vertex
      points[id].x = static_cast<int>(stod(p[2]));

      points[id].y = static_cast<int>(stod(p[3]));
      //add vertex to wDigraph we are constructing
      g.addVertex(id);
    }
    else {
      // new directed edge

      // get endpoints
      int u = stoi(p[1]), v = stoi(p[2]);
      g.addEdge(u, v, manhattan(points[u], points[v]));
      //add edge to wDigraph we are constructing
    }
  }
}
