// -*- mode: C++; -*-

#pragma once

#include "GridDescription.h"
#include <stdio.h>
#include <eigen2/Eigen/Geometry>

static const double default_eps_size = 1000.0;

template<typename Scalar>
struct any_op EIGEN_EMPTY_STRUCT {
  EIGEN_STRONG_INLINE any_op(const GridDescription &gdin,
                             Scalar (*func)(Cartesian)) : gd(gdin), fun(func) {
  }
  EIGEN_STRONG_INLINE const Scalar operator() (int row, int col) const {
    int n = row + col;
    const int z = n % gd.Nz;
    n = (n-z)/gd.Nz;
    const int y = n % gd.Ny;
    const int x = (n-y)/gd.Ny;
    const Relative rvec(x*gd.dx,y*gd.dy,z*gd.dz);
    return fun(gd.Lat.wignerSeitz(gd.Lat.toCartesian(rvec)));
  }
  GridDescription gd;
  Scalar (*fun)(Cartesian);
};
namespace Eigen {
  template<typename Scalar>
  struct ei_functor_traits<any_op<Scalar> > {
    enum {
      Cost = NumTraits<Scalar>::AddCost,
      PacketAccess = false,
      IsRepeatable = true 
    };
  };
} // namespace Eigen

class ReciprocalGrid;

class Grid : public VectorXd {
public:
  explicit Grid(const GridDescription &);
  Grid(const Grid &x);

  // We need to define this for our object to work
  typedef Eigen::VectorXd Base;
  template<typename OtherDerived>
  Grid &operator=(const Eigen::MatrixBase <OtherDerived>& other) {
    this->Base::operator=(other);
    return *this;
  }
  ReciprocalGrid fft() const;
  double operator()(int x, int y, int z) const {
    return (*this)[x*gd.NyNz + y*gd.Nz + z];
  }
  double &operator()(int x, int y, int z) {
    return (*this)[x*gd.NyNz + y*gd.Nz + z];
  }
  double operator()(const Cartesian &r) const {
    return (*this)(gd.Lat.toRelative(r));
  }
  double operator()(const Relative &r) const;
  
  void Set(double f(Cartesian));
  void epsSlice(const char *fname, Cartesian xmax, Cartesian ymax,
                Cartesian corner, int resolution) const;
  void epsNativeSlice(const char *fname,
                      Cartesian xmax, Cartesian ymax, Cartesian corner) const;
  Eigen::CwiseNullaryOp<any_op<double>, VectorXd> r2() const {
    return NullaryExpr(gd.NxNyNz, 1, r2_op);
  }
  Eigen::CwiseNullaryOp<any_op<double>, VectorXd> x() const {
    return NullaryExpr(gd.NxNyNz, 1, x_op);
  }
  Eigen::CwiseNullaryOp<any_op<double>, VectorXd> y() const {
    return NullaryExpr(gd.NxNyNz, 1, y_op);
  }
  Eigen::CwiseNullaryOp<any_op<double>, VectorXd> z() const {
    return NullaryExpr(gd.NxNyNz, 1, z_op);
  }
private:
  GridDescription gd;
  any_op<double> r2_op, x_op, y_op, z_op;
};