#ifndef BRESENHAM_HPP
#define BRESENHAM_HPP

/* Bresenham Algorithm
   Coder: zachary hutchinson Dec 2018
   Taken from wikipedia.org's pseudo code.
*/

#include<iostream>
#include<vector>
#include<cmath>

struct Point {
  float x;
  float y;
  Point();
  Point(float _x, float _y);
};

void plotLineLow(Point a, Point b, std::vector<Point> & points);
void plotLineHigh(Point a, Point b, std::vector<Point> & points);
void plotLine(Point a, Point b, std::vector<Point> & points);

float Distance(Point a, Point b);


#endif