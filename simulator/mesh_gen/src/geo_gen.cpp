#include <fstream>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

const int MAX_BUFFER_LEN = 256;

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

  string command;
  char buffer[256];

  int chamber_num = 0;
  vector<double> chamber_radius;
  vector<double> chamber_length;
  double max_mesh_size;
  while(1) {
    input >> command;
    if (command == "chamber_num:") {
      input >> chamber_num;
    } else if (command == "chamber_radius:") {
      for (int i=0; i<chamber_num; i++) {
	double radius;
	input >> radius;
	chamber_radius.push_back(radius);
      }
    } else if (command == "chamber_length:") {
      for (int i=0; i<chamber_num; i++) {
	double length;
	input >> length;
	chamber_length.push_back(length);
      }
    } else if (command == "max_mesh_size:") {
      input >> max_mesh_size;
    } else if (command == "freq_start:" || command == "freq_end:" || command == "freq_step:" || command == "speed:" || command == "rho:" || command == "p0_real:" || command == "p0_imag:") {
      input.getline(buffer, MAX_BUFFER_LEN); // ignore
    } else if (command == "#") {
      input.getline(buffer, MAX_BUFFER_LEN); // ignore
    } else if (command == "end") {
      break;
    } else {
      cout << "geo_gen: config format error" << endl;
    }
  }

  double total_length = 0;
  for (int i = 0; i < chamber_num; i++) {
    drawWholeChamber(output, i, 0, 0, chamber_radius[i], chamber_length[i], total_length);
    total_length += chamber_length[i];
  }

  finish(output, chamber_num, max_mesh_size);
}
