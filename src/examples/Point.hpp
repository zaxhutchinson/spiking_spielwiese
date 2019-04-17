
#ifndef POINT_HPP
#define POINT_HPP

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