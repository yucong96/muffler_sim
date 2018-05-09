#include "CoeffMatrix.h"

#include <iostream>
#include <map>
#include <assert.h>
#include <math.h>

#include <Eigen/SparseCholesky>
#include <Eigen/SparseLU>

using namespace std;
using namespace Eigen;

void CoeffMatrix::compute_eq_num(const vector<ElementType>& f_type, const vector<ElementType>& n_type) {

#ifdef TEST_LAPLACE
  eq_num = 7;
  return;
#endif
  
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == OUTLET || f_type[i] == INLET || f_type[i] == HARD) {
      eq_num++;
    } 
  }
  
  for (int i = 0; i < n_type.size(); i++) {
    if (n_type[i] == INNER || n_type[i] == HARD) {
      eq_num++;
    }
  }
}

void CoeffMatrix::init(const vector<ElementType>& f_type, const vector<ElementType>& n_type, const int test_num) {
  compute_eq_num(f_type, n_type);
  coeff_mat = SparseMatrix<double>(eq_num * rate, n_type.size() * rate); // 2 for complex number
#ifdef LOG
  cout << "The SparseMatrix is " << eq_num * rate << " * " << n_type.size() * rate << endl;
#endif
  coeff_b = MatrixXd::Zero(eq_num * rate, test_num);
}

void CoeffMatrix::put(const CoeffItem& item) {
  assert(now_eq_num < eq_num * rate);

  for (map<NodeIndex, double>::const_iterator iter = item.coeff.begin(); iter != item.coeff.end(); iter++) {
    coeff_mat.coeffRef(now_eq_num, iter->first) = iter->second;
  }
  for (int i=0; i<item.constant.size(); i++) {
    coeff_b(now_eq_num, i) = item.constant[i];
  }
  now_eq_num++;
}

void CoeffMatrix::set_rotations(const vector<double>& rotate_angles) {
  int n_num = coeff_mat.cols() / rate;
  int rotate_num = rotate_angles.size();

  coeff_mat_rotate = SparseMatrix<double>((rotate_num+1) * coeff_mat.rows(), coeff_mat.cols());
  coeff_b_rotate = MatrixXd((rotate_num+1) * coeff_b.rows(), coeff_b.cols());
  
  vector<Triplet<double>> triplet_list;
  for (int pi=0; pi<coeff_mat.outerSize(); pi++) {
    for (SparseMatrix<double>::InnerIterator it(coeff_mat, pi); it; ++it) {
      triplet_list.push_back(Triplet<double>(it.row(), it.col(), it.value()));
    }
  }

  coeff_b_rotate.middleRows(0, coeff_b.rows()) = coeff_b;
  
  for (int ri=0; ri<rotate_num; ri++) {
    double phi = rotate_angles[ri];

    // construct rotate_mat
    SparseMatrix<double> rotate_mat = SparseMatrix<double>(n_num * rate, n_num * rate);
    for (int j=0; j<n_num; j++) {
      rotate_mat.coeffRef(j, j) = cos(phi);
      rotate_mat.coeffRef(j, j+n_num) = -sin(phi);
      rotate_mat.coeffRef(j+n_num, j) = sin(phi);
      rotate_mat.coeffRef(j+n_num, j+n_num) = cos(phi);
    }
  
    // concatenate the new matrix with rotation
    SparseMatrix<double> single_rotate_mat = coeff_mat * rotate_mat;
    for (int pi=0; pi<single_rotate_mat.outerSize(); pi++) {
      for (SparseMatrix<double>::InnerIterator it(single_rotate_mat, pi); it; ++it) {
	triplet_list.push_back(Triplet<double>(it.row() + (ri+1)*coeff_mat.rows(), it.col(), it.value()));
      }
    }

    // construct rotate_b
    SparseMatrix<double> rotate_b = SparseMatrix<double>(coeff_b.rows(), coeff_b.rows());
    for (int j=0; j<coeff_b.rows() / 2; j++) {
      rotate_b.coeffRef(j, j) = cos(phi);
      rotate_b.coeffRef(j, j + rotate_b.rows()/2) = -sin(phi);
      rotate_b.coeffRef(j + rotate_b.rows()/2, j) = sin(phi);
      rotate_b.coeffRef(j + rotate_b.rows()/2, j + rotate_b.rows()/2) = cos(phi);
    }
    
    // concatenate the new b with rotation
    MatrixXd single_rotate_b = rotate_b * coeff_b;
    coeff_b_rotate.middleRows((ri+1) * coeff_b.rows(), coeff_b.rows()) = single_rotate_b; 
  }

  coeff_mat_rotate.reserve(triplet_list.size());
  coeff_mat_rotate.setFromTriplets(triplet_list.begin(), triplet_list.end());
}

void CoeffMatrix::solve(vector<VectorXd>& x) {

  SparseMatrix<double> A = coeff_mat_rotate.transpose() * coeff_mat_rotate;
#ifdef LOG
  cout << "A = " << A.rows() << "*" << A.cols() << endl;
#endif
  
  SimplicialLDLT<SparseMatrix<double>> solver;
  solver.compute(A);
  assert(solver.info() == Success);
#ifdef LOG
  cout << "solver.compute() finish" << endl;
#endif
  
  MatrixXd b = coeff_mat_rotate.transpose() * coeff_b_rotate;
  for (int i=0; i<b.cols(); i++) {
    VectorXd _x = solver.solve(b.col(i));
    assert(solver.info() == Success);
    x.push_back(_x);
  }
#ifdef LOG
  cout << "solver.solve() finish" << endl;
#endif

#ifdef TEST
  double error = 0;
  for (int i=0; i<x.size(); i++) {
    VectorXd bias = coeff_mat * x[i] - coeff_b.col(i);
    error += bias.norm();
  }
  cout << "Error: " << error << endl;
#endif

}

void CoeffMatrix::clear() {
  eq_num = 0;
  now_eq_num = 0;
}
