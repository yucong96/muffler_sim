#ifndef __COEFFMATRIX_H__
#define __COEFFMATRIX_H__

#include "CoeffItem.h"
#include "Constant.h"
#include "Object.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <vector>

class CoeffMatrix {
 private:
  int rate = 2;
  int eq_num = 0;
  int now_eq_num = 0;
  Eigen::SparseMatrix<double> coeff_mat;
  Eigen::MatrixXd coeff_b;
  Eigen::SparseMatrix<double> coeff_mat_rotate;
  Eigen::MatrixXd coeff_b_rotate;

  void compute_eq_num(const std::vector<ElementType> &f_type,
                      const std::vector<ElementType> &n_type);

 public:
  CoeffMatrix() {}
  void init(const std::vector<ElementType> &f_type,
            const std::vector<ElementType> &n_type, const int test_num);
  void put(const CoeffItem &item);
  void set_rotations(const std::vector<double> &rotate_angles);
  void solve(std::vector<Eigen::VectorXd> &x);
  void clear();
};

#endif
