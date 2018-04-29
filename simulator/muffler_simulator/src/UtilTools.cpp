
#include "UtilTools.h"

#include "Constant.h"

#include <assert.h>

using namespace Eigen;

UtilTools::UtilTools(const Eigen::Matrix<double, Eigen::Dynamic, 3> *_n_set) {
  set_model(_n_set);
}

UtilTools::UtilTools() {
  n_set = NULL;
  utiltools_inited = false;
}

void UtilTools::set_model(const Eigen::Matrix<double, Eigen::Dynamic, 3> *_n_set) {
  n_set = _n_set;
  utiltools_inited = true; 
}

Vector3d UtilTools::norm_vec(const Face &f) const {
  assert(utiltools_inited);
  assert(f[0] < n_set->rows() && f[1] < n_set->rows() && f[2] < n_set->rows());

  Vector3d e1 = n_set->row(f[0]) - n_set->row(f[1]);
  Vector3d e2 = n_set->row(f[0]) - n_set->row(f[2]);
  Vector3d norm_res = e1.cross(e2);

  return norm_res;
}

Vector3d UtilTools::norm_vec(const Face &f, const NodeIndex &n) const {
  assert(utiltools_inited);
  assert(n < n_set->rows());

  Vector3d norm_res = norm_vec(f);

  Vector3d e = n_set->row(f[1]) - n_set->row(n);
  if (e.dot(norm_res) < 0) {
    norm_res = -norm_res;
  }

  return norm_res;
}

double UtilTools::area(const Face &f) const {
  assert(utiltools_inited);
  assert(f[0] < n_set->rows() && f[1] < n_set->rows() && f[2] < n_set->rows());

  double area_res = 0.5 * norm_vec(f).norm();
  if (area_res < 0)
    area_res = -area_res;
  return area_res;
}

double UtilTools::volume(const Volume &v) const {
  assert(utiltools_inited);
  assert(v[0] < n_set->rows() && v[1] < n_set->rows() &&
	 v[2] < n_set->rows() && v[3] < n_set->rows());

  Matrix3d vol;
  vol.col(0) = n_set->row(v[0]) - n_set->row(v[1]);
  vol.col(1) = n_set->row(v[0]) - n_set->row(v[2]);
  vol.col(2) = n_set->row(v[0]) - n_set->row(v[3]);

  double volume_res = 1.0 / 6 * vol.determinant();
  if (volume_res < 0)
    volume_res = -volume_res;
  return volume_res;
}

double UtilTools::cos(const Face &f, const NodeIndex& n) const {
  assert(f[0] < n_set->rows() && f[1] < n_set->rows() &&
	 f[2] < n_set->rows());

  double cosine = INF;
  for (int i = 0; i < FACE_NODE_NUM; i++) {
    if (f[i] == n) {
      Vector3d e1 = n_set->row(f[(i + 1) % FACE_NODE_NUM]) - n_set->row(f[i]);
      Vector3d e2 = n_set->row(f[(i + 2) % FACE_NODE_NUM]) - n_set->row(f[i]);
      cosine = e1.dot(e2) / sqrt(e1.dot(e1) * e2.dot(e2));
      break;
    }
  }
  if (cosine == INF) {
    assert(0); // UtilTools::cos(), node is not in the face.
  }
  return cosine;
}

double UtilTools::cot(const Face &f, const NodeIndex& n) const {
  assert(f[0] < n_set->rows() && f[1] < n_set->rows() &&
	 f[2] < n_set->rows());

  double cosine = cos(f, n);
  double sine = sqrt(1 - cosine*cosine);
  double cotan = 1.0 * cosine / sine;
  return cotan;
}


bool UtilTools::in_plain(const NodeIndex &n, const Plain &p) const {

  // compute ax+by+cz+d
  double bias = 0;
  for (int i=0; i<3; i++) {
    bias += n_set->coeff(n,i) * p[i];
  }
  bias += p[3];

  // check if ax+by+cz+d == 0
  if (bias < TOLERANCE && bias > -TOLERANCE) {
    return true;
  }
  return false;
}
