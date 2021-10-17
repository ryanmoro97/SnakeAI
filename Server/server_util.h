#ifndef _SERVER_UTIL_H_
#define _SERVER_UTIL_H_

#include <unordered_map>
#include <string>
#include "wdigraph.h"

using namespace std;

//ADDED CODE - for all in server_util.cpp

// for storing coordinates of individual points
struct Point {
    long long x, y;
};

// returns the manhattan distance between two points
long long manhattan(const Point& pt1, const Point& pt2);

// finds the id of the point that is closest to the given point "pt"
int findVertex(const Point& pt, const unordered_map<int, Point>& points);

//construct the graph with vertex ids having xy coords, and build edges 
void writeGraph(int level, unordered_map<int, Point>& points);

// read the graph from the file that has the same format as the "Edmonton graph" file
void readGraph(const string& filename, WDigraph& g, unordered_map<int, Point>& points);

#endif
