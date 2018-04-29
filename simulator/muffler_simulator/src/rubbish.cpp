void pipeline(const std::string default_config_file,
              const std::string config_file, const std::string model_file,
              const std::string output_model_file) {

  ////////////////////-Physical Coefficient-//////////////
  double freq;
  double speed;
  double omega;
  double rho;
  double impedance;
  double wavenum;
  double p0_real, p0_imag;

  Plain inlet;
  Plain outlet;

  ////////////////////-Load Config-///////////////////////
  ConfigReader config_reader;
  err = config_reader.loadConfig(default_config_file, freq, speed, rho, p0_real,
                                 p0_imag, inlet, outlet);
  checkError("loading default config");
  err = config_reader.loadConfig(config_file, freq, speed, rho, p0_real,
                                 p0_imag, inlet, outlet);
  checkError("loading config");
  omega = 2 * PI * freq;
  impedance = rho * speed;
  wavenum = omega / speed;

#ifdef TEST
  std::cout << "freq = " << freq << std::endl;
  std::cout << "speed = " << speed << std::endl;
  std::cout << "rho = " << rho << std::endl;
#endif

  ////////////////////-Data Structure-////////////////////
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

  ////////////////////////-Start-/////////////////////////

  VtkReader vtk_io;
  err = vtk_io.getData(model_file, n_set, v_set);
  if (err != 0) {
    return;
  }
  n_num = n_set.rows();

  UtilTools tools(n_set, n_num);

  DataFiller filler(tools);
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

#ifdef LOG
  std::cout << "Build matrix ..." << std::endl;
#endif
  CoeffMatrix coeff_mat;
  CoeffItem coeff_item(n_num, tools);
  coeff_mat.init(f_type, n_type);

#ifdef TEST_LAPLACE
  for (int i = 0; i < n_type.size(); i++) {
    if (n_type[i] == INNER) {
      coeff_item.clear();
      coeff_item.laplace(v_set, f_set, ring_1_v, ring_1_f, i, 1.0, REAL);
      coeff_item.own(i, 1, REAL);
      coeff_item.constance(0);
      coeff_mat.put(coeff_item);
    }
  }
  for (int i = 0; i < n_type.size(); i++) {
    if (n_type[i] == HARD) {
      coeff_item.clear();
      coeff_item.own(i, 1, REAL);
      switch (i) {
      case 1:
        coeff_item.constance(1);
        break;
      case 2:
        coeff_item.constance(1);
        break;
      case 3:
        coeff_item.constance(0);
        break;
      case 6:
        coeff_item.constance(3);
        break;
      default:
        coeff_item.constance(1);
        break;
      }
      coeff_mat.put(coeff_item);
    }
  }
#endif

#ifdef NORMAL
#ifdef LOG
  std::cout << "  inner part : ";
#endif
  // inner part: \laplace p + omega^2 / speed^2 * p = 0
  for (int i = 0; i < n_type.size(); i++) {
    if (n_type[i] == INNER || n_type[i] == HARD) {
      coeff_item.clear();
      coeff_item.laplace(v_set, f_set, ring_1_v, ring_1_f, i, 1.0, REAL);
      coeff_item.own(i, (omega * omega) / (speed * speed), REAL);
      coeff_item.constance(0);
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.laplace(v_set, f_set, ring_1_v, ring_1_f, i, 1.0, IMAG);
      coeff_item.own(i, (omega * omega) / (speed * speed), IMAG);
      coeff_item.constance(0);
      coeff_mat.put(coeff_item);
    }
#ifdef LOG
    if (n_type.size() > 100 && i % (n_type.size() / 10) == 0)
      std::cout << "#";
#endif
  }
#ifdef LOG
  std::cout << std::endl;
#endif

#ifdef LOG
  std::cout << "  hard wall part : ";
#endif
  // hard wall reflection: \grad p dot norm = 0
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == HARD) {
      assert(face_near_v[i].size() == 1);
      coeff_item.clear();
      coeff_item.gradient_dot_norm(v_set[*face_near_v[i].begin()], f_set[i],
                                   1.0, UNIT_NORMALIZE, REAL);
      coeff_item.constance(0);
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v_set[*face_near_v[i].begin()], f_set[i],
                                   1.0, UNIT_NORMALIZE, IMAG);
      coeff_item.constance(0);
      coeff_mat.put(coeff_item);
    }
#ifdef LOG
    if (f_type.size() > 100 && i % (f_type.size() / 10) == 0)
      std::cout << "#";
#endif
  }
#ifdef LOG
  std::cout << std::endl;
#endif

#ifdef LOG
  std::cout << "  inlet part : ";
#endif
  double v_i_real = 0.7;
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == INLET) {
      Face f = f_set[i];
      Volume v = v_set[*face_near_v[i].begin()];

      // Below are the BEM boundary condition
      // \grad p dot norm + j * omega * rho * v = 0;
  
      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, REAL);
      coeff_item.constance(0);
      coeff_mat.put(coeff_item);
      
      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, IMAG);
      coeff_item.constance(-omega * rho * v_i_real);
      coeff_mat.put(coeff_item);

      // Below are the complex boundary condition
      // \grad p dot norm + j * wavenum * p + j * rho / (2* wavenum) * \surface_laplace p =
      // 1 - j wavenum * wavenum * p_0

      /* coeff_item.clear();
	 coeff_item.gradient_dot_norm(v, f, 1.0, REAL);
	 coeff_item.average(v, -wavenum, IMAG);
	 coeff_item.surface_laplace(f_set, surface_node_near_f, f[0], -rho / (2*wavenum), IMAG);
	 coeff_item.constance((wavenum-1) * wavenum * p0_imag);
	 coeff_mat.put(coeff_item);

	 coeff_item.clear();
	 coeff_item.gradient_dot_norm(v, f, 1.0, IMAG);
	 coeff_item.average(v, omega / speed, REAL);
	 coeff_item.surface_laplace(f_set, surface_node_near_f, f[0], rho / (2*wavenum), REAL);
	 coeff_item.constance((1-wavenum) * wavenum * p0_real);
	 coeff_mat.put(coeff_item); */
    } 
#ifdef LOG
    if (f_type.size() > 100 && i % (f_type.size() / 10) == 0) std::cout << "#";
#endif
  }
#ifdef LOG
  std::cout << std::endl;
#endif

#ifdef LOG
  std::cout << "  outlet part : ";
#endif
  double v_o_real = 0.3;
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == OUTLET) {
      Face f = f_set[i];
      Volume v = v_set[*face_near_v[i].begin()];

      // Below are the BEM boundary condition
      // \grad p dot norm + j * omega * rho * v = 0;
      
      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, REAL);
      coeff_item.constance(0);
      coeff_mat.put(coeff_item);

      coeff_item.clear();
      coeff_item.gradient_dot_norm(v, f, 1.0, UNIT_NORMALIZE, IMAG);
      coeff_item.constance(-omega * rho * v_o_real);
      coeff_mat.put(coeff_item);

      // Below are the complex boundary condition
      // \grad p dot norm + j * wavenum * p + j * rho / (2*wavenum) * \surface_laplace p = 0
      
      /* coeff_item.clear();
	 coeff_item.gradient_dot_norm(v, f, 1.0, REAL);
	 coeff_item.average(v, -wavenum, IMAG);
	 coeff_item.surface_laplace(f_set, surface_node_near_f, f[0], -rho / (2*wavenum), IMAG);
	 coeff_item.constance(0);
	 coeff_mat.put(coeff_item);

	 coeff_item.clear();
	 coeff_item.gradient_dot_norm(v, f, 1.0, IMAG);
	 coeff_item.average(v, omega / speed, REAL);
	 coeff_item.surface_laplace(f_set, surface_node_near_f, f[0], rho / (2*wavenum), REAL);
	 coeff_item.constance(0);
	 coeff_mat.put(coeff_item); */
    }

#ifdef LOG
    if (f_type.size() > 100 && i % (f_type.size() / 10) == 0) std::cout << "#";
#endif
  }  

#ifdef LOG
  std::cout << std::endl;
#endif
#endif

  Eigen::VectorXd result;
  coeff_mat.solve(result);
#ifdef TEST
  /*if (rate == 1) {
    for (int i = 0; i < result.rows(); i++) {
    cout << x(i) << endl;
    }
    }*/

  for (int i = 0; i < result.rows() / 2; i++) {
    std::cout << n_type[i] << " " << result(i) << " "
              << result(i + result.rows() / 2) << std::endl;
  }

#endif

  // result.conservativeResize(n_num);

  double E_input = 0;
  double E_inlet = 0;
  double E_outlet = 0;
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == INLET) {
      Face f = f_set[i];
      double f_area = tools.area(f);
      E_input += (p0_real * p0_real / (2 * rho * speed)) * f_area;
      for (int j = 0; j < FACE_NODE_NUM; j++) {
        double p = result[f[j]];
        E_inlet += (p * p / (2 * rho * speed)) * (f_area / FACE_NODE_NUM);
      }
    }
  }
  for (int i = 0; i < f_type.size(); i++) {
    if (f_type[i] == OUTLET) {
      Face f = f_set[i];
      double f_area = tools.area(f);
      for (int j = 0; j < FACE_NODE_NUM; j++) {
        double p = result[f[j]];
        E_outlet += (p * p / (2 * rho * speed)) * (f_area / FACE_NODE_NUM);
      }
    }
  }
  std::cout << "E_input = " << E_input << std::endl;
  std::cout << "E_inlet = " << E_inlet << std::endl;
  std::cout << "E_outlet = " << E_outlet << std::endl;

  err = vtk_io.writeData(output_model_file + "_real.vtk", n_set, v_set, result,
                         REAL);
  checkError("writing output model");
  err = vtk_io.writeData(output_model_file + "_imag.vtk", n_set, v_set, result,
                         IMAG);
  checkError("writing output model");
}
