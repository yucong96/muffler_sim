#include "VtkReader.h"

#include "Constant.h"

#include <assert.h>
#include <fstream>
#include <iostream>

using namespace std;
using namespace Eigen;

int VtkReader::getData(const string &filename,
	Matrix<double, Dynamic, 3> &n_set,
	vector<Volume> &v_set) {

	int state = READ_NODES;
	int n_num, v_num;
	string buffer;
	int n_counter = 0;

	ifstream input(filename.c_str());
	if (!input.is_open()) {
		return ERR_FILE_OPEN;
	}

	bool isEnd = false;
	while (!isEnd && (input >> buffer)) {
		switch (state) {
		case READ_NODES:
			if (buffer == "POINTS") {
				input >> n_num;
				n_set = Matrix<double, Dynamic, 3>::Zero(n_num, 3);
				input >> buffer; // omit "float"
				for (int i = 0; i < n_num; i++) {
					for (int j = 0; j < 3; j++) {
						double num;
						input >> num;
						n_set(n_counter / 3, n_counter % 3) = num;
						n_counter++;
					}
				}
				state = READ_VOLUMES;
			}
			break;
		case READ_VOLUMES:
			if (buffer == "CELLS") {
				input >> v_num;
				int v_ele_num;
				input >> v_ele_num;
				if (v_ele_num == 5 * v_num) {
					v_set.reserve(v_num);
					for (int i = 0; i < v_num; i++) {
						input >> buffer; // omit "4"
						Volume v;
						for (int j = 0; j < 4; j++) {
							input >> v[j];
						}
						v.sort();
						v_set.push_back(v);
					}
					isEnd = true;
				}
				else {
					return ERR_FILE_FORMAT;
				}
			}
			else {
				return ERR_FILE_FORMAT;
			}
			break;
		default:
			assert(0); // No state other than READ_NODES or READ_VOLUMES
			break;
		}
	}

	input.close();

#ifdef LOG
	cout << "n_set: " << n_set.rows() << endl;
	cout << "v_set: " << v_set.size() << endl;
#endif
	return 0;
}

int VtkReader::writeData(const string& filename, Matrix<double, Dynamic, 3>& n_set, vector<Volume>& v_set, VectorXd& result, const int flag) {
  ofstream output(filename);
  if (!output.is_open()) {
    return ERR_FILE_OPEN;
  }

  int n_num = n_set.rows();
  //  double result_max = result.maxCoeff();
  //  double result_min = result.minCoeff();
  double result_max = 1;
  double result_min = -1;
  assert(result_max != NAN && result_min != NAN);
  assert(result_max != INFINITY && result_min != INFINITY);
  assert(result_max != result_min);

  output << "# vtk DataFile Version 2.0" << endl;
  output << "TET" << endl;
  output << "ASCII" << endl << endl;
  output << "DATASET UNSTRUCTURED_GRID" << endl;
  output << "POINTS " << n_num << " float" << endl;
  for (int i = 0; i < n_num; i++) {
    for (int j = 0; j < 3; j++) {
      output << n_set(i, j) << " ";
    }
    output << endl;
  }
  output << "CELLS " << v_set.size() << " " << v_set.size() * (VOLUME_NODE_NUM + 1) << endl;
  for (int i = 0; i < v_set.size(); i++) {
    output << VOLUME_NODE_NUM << " ";
    for (int j = 0; j < VOLUME_NODE_NUM; j++) {
      output << v_set[i][j] << " ";
    }
    output << endl;
  }
  output << "CELL_TYPES " << v_set.size() << endl;
  for (int i = 0; i < v_set.size(); i++) {
    output << "10" << endl;
  }
  output << "POINT_DATA " << n_num << endl;
  output << "SCALARS point_scalar_data float 1" << endl;
  output << "LOOKUP_TABLE data_color" << endl;
  for (int i = 0; i < n_num; i++) {
    if (flag == REAL) {
      output << (result(i) - result_min) / (result_max - result_min) << endl;
    } else {
      output << (result(i+n_num) - result_min) / (result_max - result_min) << endl;
    }
  }
  output << "LOOKUP_TABLE data_color 101" << endl;
  for (int i = 0; i <= 100; i++) {
    output << 1.0 * i / 100 << " " << 1.0 - 1.0 * i / 100 << " 0.0 1.0" << endl;
  }

  output.close();

	return 0;
}
