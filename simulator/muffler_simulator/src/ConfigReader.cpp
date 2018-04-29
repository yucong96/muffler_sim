#include "ConfigReader.h"

#include "Constant.h"

#include <fstream>
#include <iostream>

using namespace std;

int ConfigReader::loadConfig(const string &filename, double &freq_start,
                             double &freq_end, double &freq_step, double &speed,
                             double &rho, Plain &inlet, Plain &outlet,
                             int &test_num, vector<double> &inlet_v,
                             vector<double> &outlet_v, double& p0_real, double& p0_imag) {
  string command;
  char buffer[256];

  ifstream input(filename.c_str());
  if (!input.is_open()) {
    return ERR_FILE_OPEN;
  }

  while (1) {
    input >> command;
    if (command == "freq_start:") {
      input >> freq_start;
    } else if (command == "freq_end:") {
      input >> freq_end;
    } else if (command == "freq_step:") {
      input >> freq_step;
    } else if (command == "speed:") {
      input >> speed;
    } else if (command == "rho:") {
      input >> rho;
    } else if (command == "inlet:") {
      double plain_coeff;
      for (int i = 0; i < 4; i++) { // 4 for a,b,c,d
	input >> plain_coeff;
	inlet[i] = plain_coeff;
      }
    } else if (command == "outlet:") {
      double plain_coeff;
      for (int i = 0; i < 4; i++) { // 4 for a,b,c,d
        input >> plain_coeff;
        outlet[i] = plain_coeff;
      }
    } else if (command == "test_num:") {
      input >> test_num;
    } else if (command == "inlet_particle_velocity_or_p0_real:") {
      assert(test_num > 0);
      if (test_num == 1) {
	input >> p0_real;
      } else {
	for (int i = 0; i < test_num; i++) {
	  double velocity;
	  input >> velocity;
	  inlet_v.push_back(velocity);
	}
      }
    } else if (command == "outlet_particle_velocity_or_p0_imag:") {
      assert(test_num > 0);
      if (test_num == 1) {
	input >> p0_imag;
      } else {
	for (int i = 0; i < test_num; i++) {
	  double velocity;
	  input >> velocity;
	  outlet_v.push_back(velocity);
	}
      }
    } else if (command == "#") {
      input.getline(buffer, MAX_BUFFER_LEN);
    } else if (command == "end") {
      break;
    } else {
      return ERR_FILE_FORMAT;
    }
  }

  return 0;
}
