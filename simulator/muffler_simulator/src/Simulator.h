#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "Constant.h"
#include "Object.h"
#include "ConfigReader.h"
#include "VtkReader.h"
#include "DataFiller.h"
#include "UtilTools.h"
#include "CoeffItem.h"
#include "CoeffMatrix.h"

extern int err;
extern void checkError(const std::string comment);

class Simulator {
private:
  // physical parameter

  ConfigReader config_reader;

  double freq, freq_start, freq_end, freq_step, speed, rho;
  double omega, wavenum;

  Plain inlet, outlet;

  int test_num = 1;
  double p0_real, p0_imag;

  // model parameter

  VtkReader vtk_io;
  UtilTools tools;
  DataFiller filler;
  
  int n_num;

  Eigen::Matrix<double, Eigen::Dynamic, 3> n_set;
  std::vector<Volume> v_set;
  std::vector<Face> f_set;

  std::vector<std::set<VolumeIndex>> ring_1_v;
  std::vector<std::set<NodeIndex>> ring_1_n;
  std::vector<std::set<FaceIndex>> ring_1_f;

  std::vector<std::set<NodeIndex>> face_near_n;
  std::vector<std::set<VolumeIndex>> face_near_v;

  std::vector<std::set<FaceIndex>> surface_node_near_f;

  std::vector<ElementType> f_type;
  std::vector<ElementType> n_type;
#ifdef TEST
  Eigen::VectorXd n_type_vec;
#endif

  // matrix
  
  CoeffMatrix coeff_mat;
  CoeffItem coeff_item;
  std::vector<Eigen::VectorXd> result;
  
  // function
  
  void load_model(const std::string model_file);
  void setup_utiltools();
  void setup_data();
  void load_config(const std::string config_file);
  void init_matrix();
  void set_governing_equation();
  void set_boundary_conditions();
  void set_hard_bc();
  void set_inlet_bc();
  void set_outlet_bc();
  void set_rotations();
  void solve_matrix();
  void post_process(const std::string output_model_file);
  
public:
  void simulate(const std::string config_file, const std::string model_file,
		const std::string output_model_file);
};

#endif
