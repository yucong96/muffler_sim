#include <fstream>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cerr << "vol2vtk: please use right format \"vol2vtk {input.vol} {output.vtk}\"" << endl;
    return 1;
  }

  string buffer;
  
  string vol_file = argv[1];
  ifstream input(vol_file.c_str());
  if (!input.is_open()) {
    cerr << "vol2vtk: input file not exists" << endl;
    return 1;
  }

  string vtk_file = argv[2];
  ofstream output(vtk_file.c_str());
  if (!output.is_open()) {
    cerr << "vol2vtk: output file cannot open" << endl;
    return 1;
  }
  output << "# vtk DataFile Version 2.0" << endl;
  output << "TET" << endl;
  output << "ASCII" << endl << endl;
  output << "DATASET UNSTRUCTURED_GRID" << endl;
  
  while (input >> buffer) {
    if (buffer == "points") {
      int n_num;
      input >> n_num;
      output << "POINTS " << n_num << " float" << endl;
      for (int i=0; i<n_num; i++) {
	float x, y, z;
	input >> x >> y >> z;
	output << x << " " << y << " " << z << endl;
      }
    }
  }
  input.clear();
  input.seekg(0, ios::beg);
  while (input >> buffer) {
    if (buffer == "volumeelements") {
      int v_num;
      input >> v_num;
      output << "CELLS " << v_num << " " << v_num * 5 << endl;
      for (int i=0; i<v_num; i++) {
	int temp, num, v1, v2, v3, v4;
	input >> temp >> num >> v1 >> v2 >> v3 >> v4;
	output << num << " " << v1-1 << " " << v2-1 << " " << v3-1 << " " << v4-1 << endl; 
      }
      output << "CELL_TYPES " << v_num << endl;
      for (int i = 0; i < v_num; i++) {
	output << "10" << endl;
      } 
    }
  }

  input.close();
  output.close();

  return 0;
}
