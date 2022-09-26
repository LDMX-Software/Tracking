#pragma once

#include <memory> 
#include <vector>

#include "Tracking/geo/polyhedron.h" 

namespace tracking::geo { 
class Box : public Polyhedron { 
 public:
  /**
   */
  Box(double dx, double dy, double dz); 
  
  /**
   */  
  ~Box() = default; 
  
  /**
   */  
  std::vector< std::shared_ptr<Point> > vertices() { 
    return vertices_; 
  };         

 private: 
  ///
  double dx_; 
  /// 
  double dy_; 
  /// 
  double dz_;
  ///
  std::vector< std::shared_ptr<Point> > vertices_; 
};
}
