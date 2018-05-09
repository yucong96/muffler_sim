#include "Simulator.h"

#include <iostream>

using namespace std;
using namespace Eigen;

void Simulator::load_model(const string model_file) {
  err = vtk_io.getData(model_file + ".vtk", n_set, v_set);
  checkError("loading model");

  n_num = n_set.rows();
}

void Simulator::setup_utiltools() { tools.set_model(&n_set); }

void Simulator::setup_data() {
  filler.set_tools(&tools);
  filler.getFaceSet(v_set, f_set);
  filler.getFaceNearNode(v_set, f_set, face_near_n);
  filler.getFaceNearVolume(v_set, f_set, face_near_v);
  filler.getOneRingVolume(n_num, v_set, ring_1_v);
  filler.getOneRingNode(n_num, v_set, ring_1_n);
  filler.getOneRingFace(n_num, face_near_n, ring_1_f);
  filler.getFaceType(inlet, outlet, f_set, face_near_n, f_type);
  filler.getNodeType(n_num, f_set, f_type, n_type);
  filler.getSurfNodeNearFace(n_num, f_set, f_type, surface_node_near_f);
#ifdef TEST
  filler.getNodeTypeVec(n_type, n_type_vec);
#endif
  filler.clear();

#ifdef TEST
  err =
    vtk_io.writeData("../../log/n_type.vtk", n_set, v_set, n_type_vec, REAL);
  checkError("writing output model");
#endif
}

void Simulator::load_config(const string config_file) {
  err = config_reader.loadConfig(config_file + ".txt", freq_start, freq_end, freq_step,
                                 speed, rho, inlet, outlet, p0_real, p0_imag);
  checkError("loading config");

#ifdef LOG
  cout << "freq = " << freq_start << ":" << freq_end << ":" << freq_step
       << endl;
  cout << "speed = " << speed << endl;
  cout << "rho = " << rho << endl;
#endif
}

void Simulator::init_matrix() {
#ifdef LOG
  cout << "Build matrix ..." << endl;
#endif

  coeff_mat.init(f_type, n_type, test_num);
  coeff_item.set_nodenum_and_tools(n_num, &tools);
}

void Simulator::set_governing_equation() {
#ifdef LOG
  cout << "  inner part : ";
#endif

  for (int i = 0; i < n_type.size(); i++) {
    if (n_type[i] == INNER || n_type[i] == HARD) {

      // Below are the governing equation in the inner part
      // \laplace p + wavenum * wavenum * p = 0

      coeff_item.clear();
      coeff_item.laplace(v_set, f_set, ring_1_v, ring_1_f, i, 1.0, REAL);
      coeff_item.own(i, wavenum * wavenum, REAL);
      for (int i = 0; i < test_num; i++) {
        coeff_item.add_constant(0);
      }
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.laplace(v_set, f_set, ring_1_v, ring_1_f, i, 1.0, IMAG);
      coeff_item.own(i, wavenum * wavenum, IMAG);
      for (int i = 0; i < test_num; i++) {
        coeff_item.add_constant(0);
      }
      coeff_mat.put(coeff_item);
    }
#ifdef LOG
    if (n_type.size() > 100 && i % (n_type.size() / 10) == 0)
      cout << "#";
#endif
  }
#ifdef LOG
  cout << endl;
#endif
}

void Simulator::set_hard_bc() {
#ifdef LOG
  cout << "  hard wall part : ";
#endif
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == HARD) {
      assert(face_near_v[i].size() == 1);

      // Below are the boundary condition at hard wall
      // \grad p dot norm = 0

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v_set[*face_near_v[i].begin()], f_set[i],
                                   1.0, UNIT_NORMALIZE, REAL);
      for (int i = 0; i < test_num; i++) {
        coeff_item.add_constant(0);
      }
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v_set[*face_near_v[i].begin()], f_set[i],
                                   1.0, UNIT_NORMALIZE, IMAG);
      for (int i = 0; i < test_num; i++) {
        coeff_item.add_constant(0);
      }
      coeff_mat.put(coeff_item);
    }
#ifdef LOG
    if (f_type.size() > 100 && i % (f_type.size() / 10) == 0)
      cout << "#";
#endif
  }
#ifdef LOG
  cout << endl;
#endif
}

void Simulator::set_inlet_bc() {
#ifdef LOG
  cout << "  inlet part : ";
#endif
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == INLET) {
      Face f = f_set[i];
      Volume v = v_set[*face_near_v[i].begin()];

      // Below are the BEM boundary condition
      // \grad p dot norm + j * omega * rho * v = 0;

      /*coeff_item.clear();
        coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, REAL);
        for (int i = 0; i < test_num; i++) {
        coeff_item.add_constant(0);
        }
        coeff_mat.put(coeff_item);

        coeff_item.clear();
        coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, IMAG);
        for (int i = 0; i < test_num; i++) {
        coeff_item.add_constant(-omega * rho * inlet_v[i]);
        }
        coeff_mat.put(coeff_item);*/

      // Below are the simple boundary condition
      // \grad p dot norm + j * wavenum * p = 2 * j * wavenum * p0

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, REAL);
      coeff_item.average(v, -wavenum, IMAG);
      coeff_item.add_constant(-2 * wavenum * p0_imag);
      //coeff_item.add_constant(-wavenum * p0_imag);
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, IMAG);
      coeff_item.average(v, wavenum, REAL);
      coeff_item.add_constant(2 * wavenum * p0_real);
      //coeff_item.add_constant(wavenum * p0_real); 
      coeff_mat.put(coeff_item);
      
      // Below are the complex boundary condition
      // \grad p dot norm + j * wavenum * p + j * rho / (2* wavenum) *
      // \surface_laplace p =
      // (1 - wavenum) * j * wavenum * p_0

      /*coeff_item.clear();
	coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, REAL);
      coeff_item.average(v, -wavenum, IMAG);
      coeff_item.surface_laplace(f_set, surface_node_near_f, f[0],
                                 -rho / (2 * wavenum), IMAG);
      coeff_item.add_constant((wavenum - 1) * wavenum * p0_imag);
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, IMAG);
      coeff_item.average(v, wavenum, REAL);
      coeff_item.surface_laplace(f_set, surface_node_near_f, f[0],
                                 rho / (2 * wavenum), REAL);
      coeff_item.add_constant((1 - wavenum) * wavenum * p0_real);
      coeff_mat.put(coeff_item);*/
    }
#ifdef LOG
    if (f_type.size() > 100 && i % (f_type.size() / 10) == 0)
      cout << "#";
#endif
  }
#ifdef LOG
  cout << endl;
#endif
}

void Simulator::set_outlet_bc() {
#ifdef LOG
  cout << "  outlet part : ";
#endif
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == OUTLET) {
      Face f = f_set[i];
      Volume v = v_set[*face_near_v[i].begin()];

      // Below are the BEM boundary condition
      // \grad p dot norm + j * omega * rho * v = 0;

      /*coeff_item.clear();
	coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, REAL);
        for (int i = 0; i < test_num; i++) {
        coeff_item.add_constant(0);
        }
        coeff_mat.put(coeff_item);

        coeff_item.clear();
        coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, IMAG);
        for (int i = 0; i < test_num; i++) {
        coeff_item.add_constant(-omega * rho * outlet_v[i]);
        }
        coeff_mat.put(coeff_item);*/

      // Below are the simple boundary condition
      // \grad p dot norm + j * wavenum * p = 0

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, REAL);
      coeff_item.average(v, -wavenum, IMAG);
      coeff_item.add_constant(0);
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, IMAG);
      coeff_item.average(v, wavenum, REAL);
      coeff_item.add_constant(0);
      coeff_mat.put(coeff_item);

      // Below are the complex boundary condition
      // \grad p dot norm + j * wavenum * p + j * rho / (2*wavenum) *
      // \surface_laplace p = 0

      /*coeff_item.clear();
	coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, REAL);
      coeff_item.average(v, -wavenum, IMAG);
      coeff_item.surface_laplace(f_set, surface_node_near_f, f[0],
                                 -rho / (2 * wavenum), IMAG);
      coeff_item.add_constant(0);
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, IMAG);
      coeff_item.average(v, wavenum, REAL);
      coeff_item.surface_laplace(f_set, surface_node_near_f, f[0],
                                 rho / (2 * wavenum), REAL);
      coeff_item.add_constant(0);
      coeff_mat.put(coeff_item);*/
    }
#ifdef LOG
    if (f_type.size() > 100 && i % (f_type.size() / 10) == 0)
      cout << "#";
#endif
  }
#ifdef LOG
  cout << endl;
#endif
}

void Simulator::set_boundary_conditions() {
  set_hard_bc();
  set_inlet_bc();
  set_outlet_bc();
}

void Simulator::set_rotations() {
  vector<double> rotate_angles;
  /*rotate_angles.push_back(0.5 * PI);
    rotate_angles.push_back(1.0 * PI);
  rotate_angles.push_back(1.5 * PI);*/
  coeff_mat.set_rotations(rotate_angles);
}

void Simulator::solve_matrix() {
  coeff_mat.solve(result);

#ifdef TEST
  for (int i = 0; i < n_num; i++) {
    cout << n_type[i];
    for (int j = 0; j < test_num; j++) {
      cout << " " << result[j](i) << " " << result[j](i + n_num) << " ";
    }
    cout << endl;
  }
#endif
}

void Simulator::post_process(const string output_model_file) {

  if (test_num >= 2) {
    double A, B, C, D;
    double A_star = 0, B_star = 0, C_star = 0, D_star = 0;
    int A_num = 0, B_num = 0, C_num = 0, D_num = 0;

    for (int i = 0; i < n_num; i++) {
      if (n_type[i] == INLET) {
	A_star += result[0](i + n_num);
	A_num++;
	B_star += result[1](i + n_num);
	B_num++;
      }
      if (n_type[i] == OUTLET) {
        C_star += result[0](i + n_num);
        C_num++;
        D_star += result[1](i + n_num);
        D_num++;
      }
    }
    A_star /= A_num;
    B_star /= B_num;
    C_star /= C_num;
    D_star /= D_num;
    A = A_star / C_star;
    B = B_star - (A_star * D_star) / C_star;
    C = -1.0 / C_star;
    D = -D_star / C_star;

    double TL = (D_star + (B_star * C_star - D_star * A_star) / rho / speed +
                 rho * speed - A_star) /
      B_star / 2;
    if (TL < 0)
      TL = -TL;
    cout << "freq = " << freq << " TL = " << TL << endl;
    TL = 20 * log(TL) / log(10);

#ifdef LOG
    cout << "A=" << A << " "
         << "B=" << B << " "
         << "C=" << C << " "
         << "D=" << D << " " << endl;
#endif

#ifdef DELETE
    if (test_num > 2) {
      for (int i = 2; i < test_num; i++) {
        double p_i = 0, p_o = 0;
        int p_i_num = 0, p_o_num = 0;
        for (int j = 0; j < n_num; j++) {
          if (n_type[j] == INLET) {
            p_i += result[i](j + n_num);
            p_i_num++;
          }
          if (n_type[j] == OUTLET) {
            p_o += result[i](j + n_num);
            p_o_num++;
          }
        }
        cout << "real:     inlet = " << p_i / p_i_num
             << " outlet = " << p_o / p_o_num << endl;
        cout << "estimate: inlet = "
             << A_star * inlet_v[i] + B_star * outlet_v[i]
             << " outlet = " << C_star * inlet_v[i] + D_star * outlet_v[i]
             << endl;
      }
    }
#endif
  }

  double input_energy = 0, inlet_energy = 0, outlet_energy = 0;
  for (int i=0; i<f_set.size(); i++) {
    if (f_type[i] == OUTLET) {
      Face f = f_set[i];
      double area = tools.area(f);
      double energy = 0;
      for (int j=0; j<FACE_NODE_NUM; j++) {
	energy += sqrt(result[0](f[j])*result[0](f[j]) + result[0](f[j]+n_num)*result[0](f[j]+n_num));
      }
      energy = (energy / FACE_NODE_NUM) * (energy / FACE_NODE_NUM);
      outlet_energy += energy * area;
    }
    if (f_type[i] == INLET) {
      Face f = f_set[i];
      double area = tools.area(f);
      double energy = p0_real*p0_real + p0_imag*p0_imag;
      input_energy += energy * area;
      energy = 0;
      for (int j=0; j<FACE_NODE_NUM; j++) {
	energy += sqrt(result[0](f[j])*result[0](f[j]) + result[0](f[j]+n_num)*result[0](f[j]+n_num));
      }
      energy = (energy / FACE_NODE_NUM) * (energy / FACE_NODE_NUM);
      inlet_energy += energy * area;
    }
  }
  double tl = input_energy / outlet_energy;
  cout << "freq = " << freq
       << " input_energy = " << input_energy
       << " inlet_energy = " << inlet_energy
       << " outlet_energy = " << outlet_energy
       << " tl = " << tl << endl;
  
  for (int i = 0; i < test_num; i++) {
    err = vtk_io.writeData(output_model_file + "_real_" + to_string(int(freq)) + ".vtk",
			   n_set, v_set, result[i], REAL);
    checkError("writing output model");
    err = vtk_io.writeData(output_model_file + "_imag_" + to_string(int(freq)) + ".vtk",
			   n_set, v_set, result[i], IMAG);
    checkError("writing output model");
  }
}

void Simulator::simulate(const string config_file, const string model_file,
                         const string output_model_file) {
  load_config(config_file);
  load_model(model_file);
  setup_utiltools();
  setup_data();
  for (freq = freq_start; freq < freq_end; freq += freq_step) {
    omega = 2 * PI * freq;
    wavenum = omega / speed;

    init_matrix();
    set_governing_equation();
    set_boundary_conditions();
    set_rotations();
    solve_matrix();
    post_process(output_model_file);

    coeff_mat.clear();
    result.clear();
  }
}
