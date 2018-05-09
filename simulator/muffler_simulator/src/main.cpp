
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

  if (argc != 4) {
    err = ERR_WRONG_ARG_NUMBER;
  }
  checkError("argument");

  std::string config_file(argv[1]);
  std::string model_file(argv[2]);
  std::string output_model_file(argv[3]);

  Simulator sim;
  sim.simulate(config_file, model_file, output_model_file);

#ifdef WINDOWS
  system("pause");
#endif

  return 0;
}
