// Deft is a density functional package developed by the research
// group of Professor David Roundy
//
// Copyright 2010 The Deft Authors
//
// Deft is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// You should have received a copy of the GNU General Public License
// along with deft; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// Please see the file AUTHORS for a list of authors.

#include "LineMinimizer.h"
#include <stdio.h>

class SteepestDescentType : public MinimizerInterface {
protected:
  double step, orig_step;
  LineMinimizer linmin;
public:
  SteepestDescentType(Functional f, const GridDescription &gdin, double kT, VectorXd *data,
                      LineMinimizer lm, double stepsize = 0.1)
    : MinimizerInterface(f, gdin, kT, data), step(stepsize), orig_step(step), linmin(lm) {}
  void minimize(Functional newf, const GridDescription &gdnew, VectorXd *newx = 0) {
    step = orig_step;
    MinimizerInterface::minimize(newf, gdnew, newx);
  }

  bool improve_energy(bool verbose = false);
  void print_info(const char *prefix="") const;
};

class PreconditionedSteepestDescentType : public SteepestDescentType {
public:
  PreconditionedSteepestDescentType(Functional f, const GridDescription &gdin, double kT,
                                    VectorXd *data, LineMinimizer lm, double stepsize = 0.1)
    : SteepestDescentType(f, gdin, kT, data, lm, stepsize) {}

  bool improve_energy(bool verbose = false);
};

bool SteepestDescentType::improve_energy(bool verbose) {
  iter++;
  //printf("I am running SteepestDescent::improve_energy\n");
  const double E0 = energy();
  if (isnan(E0)) {
    // There is no point continuing, since we're starting with a NaN!
    // So we may as well quit here.
    if (verbose) {
      printf("The initial energy is a NaN, so I'm quitting early.\n");
      fflush(stdout);
    }
    return false;
  }
  const VectorXd d = -grad();
  const double d2 = -d.dot(d);
  // Let's immediately free the cached gradient stored internally!
  invalidate_cache();

  Minimizer lm = linmin(f, gd, kT, x, d, d2, &step);
  for (int i=0; i<100 && lm.improve_energy(verbose); i++) {
    if (verbose) lm.print_info("\t");
  }
  if (verbose) {
    //lm->print_info();
    print_info();
    printf("grad*oldgrad = %g\n", grad().dot(d)/d2);
  }
  return (energy() < E0);
}

void SteepestDescentType::print_info(const char *prefix) const {
  MinimizerInterface::print_info(prefix);
  printf("%sstep = %g\n", prefix, step);
}

bool PreconditionedSteepestDescentType::improve_energy(bool verbose) {
  iter++;
  //printf("I am running PreconditionedSteepestDescent::improve_energy\n");
  const double E0 = energy();
  if (isnan(E0)) {
    // There is no point continuing, since we're starting with a NaN!
    // So we may as well quit here.
    if (verbose) {
      printf("The initial energy is a NaN, so I'm quitting early.\n");
      fflush(stdout);
    }
    return false;
  }
  const VectorXd d = -pgrad();
  const double gdotd = d.dot(grad());
  // Let's immediately free the cached gradient stored internally!
  invalidate_cache();

  Minimizer lm = linmin(f, gd, kT, x, d, gdotd, &step);
  for (int i=0; i<100 && lm.improve_energy(verbose); i++) {
    if (verbose) lm.print_info("\t");
  }
  if (verbose) {
    //lm->print_info();
    print_info();
    printf("grad*direction = %g\n", grad().dot(d)/gdotd);
  }
  return (energy() < E0);
}


Minimizer SteepestDescent(Functional f, const GridDescription &gdin, double kT, VectorXd *data,
                          LineMinimizer lm, double stepsize) {
  return Minimizer(new SteepestDescentType(f, gdin, kT, data, lm, stepsize));
}

Minimizer PreconditionedSteepestDescent(Functional f, const GridDescription &gdin, double kT,
                                        VectorXd *data, LineMinimizer lm, double stepsize) {
  return Minimizer(new PreconditionedSteepestDescentType(f, gdin, kT, data, lm, stepsize));
}
