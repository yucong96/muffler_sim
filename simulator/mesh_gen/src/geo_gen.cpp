#include <fstream>
#include <iostream>
#include <string>

using namespace std;

const int WHOLE_CHAMBER = 0;
const int LEFT_IN_CHAMBER = 1;
const int RIGHT_IN_CHAMBER = 2;
const int BOTH_IN_CHAMBER = 3;

void init(ofstream &output) {
  output << "# A muffler" << endl;
  output << "algebraic3d" << endl;
}

void finish(ofstream &output, int chamber_num, double max_mesh_size) {
  output << "solid muffer = ";
  for (int i=0; i<chamber_num; i++) {
    output << "chamber" << i << " ";
    if (i!=chamber_num-1) {
      output << "or ";
    }
  }
  output << " -maxh=" << max_mesh_size << ";" << endl;
  output << "tlo muffer;" << endl;
}

void drawWholeChamber(ofstream &output, const int index, const double x,
		      const double z, const double radius, const double length,
		      const double total_length) {
  output << "solid chamber" << index << " = cylinder(" << x << ", "
	 << total_length << ", " << z << "; " << x << ", "
	 << total_length + length << ", " << z << "; " << radius << ")"
	 << endl;
  output << "\t" << "and plane(" << x << ", " << total_length << ", " << z
	 << "; 0, -1, 0)" << endl;
  output << "\t" << "and plane(" << x << ", " << total_length + length << ", " << z
	 << "; 0, 1, 0);" << endl;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "geo_gen: please use right format \"geo_gen {input.txt} {output.geo}\"" << endl;
    return 1;
  }

  string filename = argv[1];
  ifstream input(filename.c_str());
  if (!input.is_open()) {
    cerr << "geo_gen: input file not exists" << endl;
    return 1;
  }

  string output_filename = argv[2];
  ofstream output(output_filename.c_str());
  if (!output.is_open()) {
    cerr << "geo_gen: output file open failed" << endl;
    return 1;
  }

  init(output);

  int chamber_num = 0;
  double total_length = 0;
  input >> chamber_num;
  for (int i = 0; i < chamber_num; i++) {
    int flag = 0;
    input >> flag;
    switch (flag) {
    case WHOLE_CHAMBER: {
      double x, z, radius, length;
      input >> x >> z >> radius >> length;
      drawWholeChamber(output, i, x, z, radius, length, total_length);
      total_length += length;
      break;
    }
    case LEFT_IN_CHAMBER: {
      
      break;
    }
    case RIGHT_IN_CHAMBER: {

      break;
    }
    case BOTH_IN_CHAMBER: {

      break;
    }
    default:
      break;
    }
  }

  double max_mesh_size;
  input >> max_mesh_size;
  
  finish(output, chamber_num, max_mesh_size);
}
