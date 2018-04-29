#ifndef __CONFIGREADER_H__
#define __CONFIGREADER_H__

#include "Object.h"

#include <string>
#include <vector>

class ConfigReader {
private:
  static const int MAX_BUFFER_LEN = 256;
  
public:
  int loadConfig(const std::string &filename, double &freq_start, double &freq_end, double &freq_step, double &speed, double &rho, Plain &inlet, Plain &outlet, int& test_num, std::vector<double> &inlet_v,
		 std::vector<double> &outlet_v, double& p0_real, double& p0_imag);
};

#endif
