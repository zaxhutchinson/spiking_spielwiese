#include"Point.hpp"

Point::Point() {
  
}

Point::Point(float _x, float _y) {
    x=_x;
    y=_y;
}

void plotLineLow(Point a, Point b, std::vector<Point> & points) {
  int dx = b.x - a.x;
  int dy = b.y - a.y;
  int yi = 1;
  if(dy < 0) {
    yi = -1;
    dy = -dy;
  }

  int D = 2*dy - dx;
  int y = a.y;

  for(int x = a.x; x < b.x; x++) {
    points.push_back(Point(x,y));
    if(D > 0) {
      y = y + yi;
      D = D - 2*dx;
    }
    D = D + 2*dy;
  }
}
void plotLineHigh(Point a, Point b, std::vector<Point> & points) {
  int dx = b.x-a.x;
  int dy = b.y-a.y;
  int xi = 1;

  if(dx < 0) {
    xi = -1;
    dx = -dx;
  }

  int D = 2*dx - dy;
  int x = a.x;

  for(int y = a.y; y < b.y; y++) {
    points.push_back(Point(x,y));
    if(D > 0) {
      x = x + xi;
      D = D - 2*dy;
    }
    D = D + 2*dx;
  }
}

void plotLine(Point a, Point b, std::vector<Point> & points) {
  if(std::abs(b.y-a.y) < std::abs(b.x-a.x)) {
    if( a.x > b.x ) {
      plotLineLow(b, a, points);
    }
    else {
      plotLineLow(a,b,points);
    }
  }
  else {
    if(a.y > b.y) {
      plotLineHigh(b,a,points);
    }
    else {
      plotLineHigh(a,b,points);
    }
  }
}
    
float Distance(Point a, Point b) {
  return std::sqrt( std::pow(b.x-a.x,2.0) + std::pow(b.y-a.y, 2.0) );
}