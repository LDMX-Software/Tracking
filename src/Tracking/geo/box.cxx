#include "Tracking/geo/box.h" 

namespace tracking::geo { 

Box::Box(double dx, double dy, double dz)
    : Polyhedron(), dx_(dx), dy_(dy), dz_(dz) { 

  vertices_.push_back(std::make_shared<Point>(dx, dy, dz));
  vertices_.push_back(std::make_shared<Point>(-dx, -dy, dz));
  vertices_.push_back(std::make_shared<Point>(-dx, dy, dz));
  vertices_.push_back(std::make_shared<Point>(dx, -dy, dz));
  vertices_.push_back(std::make_shared<Point>(dx, dy, -dz));
  vertices_.push_back(std::make_shared<Point>(-dx, -dy, -dz));
  vertices_.push_back(std::make_shared<Point>(-dx, dy, -dz));
  vertices_.push_back(std::make_shared<Point>(dx, -dy, -dz));
     
}
}
