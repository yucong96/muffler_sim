
#include "Constant.h"
#include "Simulator.h"

#include <iostream>
#include <map>
#include <sstream>

int err = 0;

void checkError(const std::string comment) {
  if (err == 0)
    return;
  std::cerr << "Error: in " << comment << ": ";
  switch (err) {
  case ERR_FILE_OPEN:
    std::cerr << "open file failed" << std::endl;
    break;
  case ERR_FILE_FORMAT:
    std::cerr << "wrong file format" << std::endl;
    break;
  case ERR_WRONG_ARG_NUMBER:
    std::cerr << "usage: ./muffler model-name" << std::endl;
  default:
    std::cerr << "unexpected error" << std::endl;
    break;
  }
  exit(0);
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    err = ERR_WRONG_ARG_NUMBER;
  }
  checkError("argument");

  std::string model_name(argv[1]);

#ifdef WINDOWS
  std::string config_file = "..\\config\\config.txt";
  std::string model_file = "..\\tet\\muffler.vtk";
  std::string output_model_file = "..\\file\\output_muffler.vtk";
#endif

#ifdef LINUX
  std::string config_file = "../../config/config.txt";
  std::string model_file = "../../tet/" + model_name + ".vtk";
  std::string output_model_file = "../../output/" + model_name;
#endif

  // pipeline(default_config_file, config_file, model_file, output_model_file);
  Simulator sim;
  sim.simulate(config_file, model_file, output_model_file);

#ifdef WINDOWS
  system("pause");
#endif

  return 0;
}
