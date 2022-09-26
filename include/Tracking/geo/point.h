#pragma once

#include <Eigen/Core>

namespace tracking::geo { 

class Point : public Eigen::Vector3d {
 public:
  /**
   */
  Point(double x, double y, double z) : Eigen::Vector3d(x, y, z) {};

  /**
   */
  Point(std::vector<double> v) : Eigen::Vector3d(v.data()) {}; 

  /**
   * This constructor allows for the construction of Point from Eigen 
   * expressions.
   */
  template< typename Derived > 
  Point(const Eigen::MatrixBase< Derived >& der) : Eigen::Vector3d(der) {}

  /** Destructor */
  ~Point() = default;

  /// This operator allows for the assignment of an Eigen expression to Point.
  template< typename Derived >
  Point& operator=(const Eigen::MatrixBase < Derived >& der) { 
    Eigen::Vector3d::operator=(der); 
    return *this; 
  } 

};// Point
} // namespace tracker::geo
