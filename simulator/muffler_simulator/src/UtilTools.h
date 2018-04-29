#ifndef __UTILTOOLS_H__
#define __UTILTOOLS_H__

#include "Object.h"

#include <Eigen/Dense>

class UtilTools {
private:
  bool utiltools_inited;
  const Eigen::Matrix<double, Eigen::Dynamic, 3> *n_set;

 public:
  UtilTools(const Eigen::Matrix<double, Eigen::Dynamic, 3> *_n_set);
  UtilTools();

  void set_model(const Eigen::Matrix<double, Eigen::Dynamic, 3> *_n_set);

  Eigen::Vector3d norm_vec(const Face &f) const;
  Eigen::Vector3d norm_vec(const Face &f, const NodeIndex &n) const;
  double area(const Face &f) const;
  double volume(const Volume &v) const;
  double cos(const Face &f, const NodeIndex& n) const;
  double cot(const Face &f, const NodeIndex& n) const;
  bool in_plain(const NodeIndex &n, const Plain &p) const;
};

#endif
