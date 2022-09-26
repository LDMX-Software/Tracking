#pragma once

#include <memory> 
#include <vector>

#include "Tracking/geo/point.h"

namespace tracking::geo { 

class Polyhedron { 

 public:
  /**
   *
   */
  virtual std::vector< std::shared_ptr<Point> > vertices() = 0;  

};
}

