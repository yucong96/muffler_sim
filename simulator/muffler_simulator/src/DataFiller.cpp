#include "DataFiller.h"

#include <fstream>
#include <iostream>
using namespace Eigen;
using namespace std;

DataFiller::DataFiller(const UtilTools *_tools) {
  tools = _tools;
  f_temp_map_inited = false;
}

DataFiller::DataFiller() {
  tools = NULL;
  f_temp_map_inited = false;
}

void DataFiller::set_tools(const UtilTools *_tools) { tools = _tools; }

void DataFiller::getFaceTempMap(const vector<Volume> &v_set) {
  for (int i = 0; i < v_set.size(); i++) {
    for (int j = 0; j < 4; j++) {
      Face f;
      for (int k = 0; k < 3; k++) {
        f[k] = v_set[i][(j + 1 + k) % 4];
      }
      f.sort();
      if (f_temp_map.find(f) == f_temp_map.end()) {
        vector<VolumeIndex> new_vector;
        new_vector.push_back(i);
        f_temp_map[f] = new_vector;
      } else {
        f_temp_map[f].push_back(i);
      }
    }
  }
  f_temp_map_inited = true;
}

void DataFiller::clearFaceTempMap() {
  f_temp_map.clear();
  map<Face, vector<VolumeIndex>> empty_map;
  f_temp_map.swap(empty_map);
  f_temp_map_inited = false;
}

void DataFiller::clear() { clearFaceTempMap(); }

void DataFiller::getFaceSet(const vector<Volume> &v_set, vector<Face> &f_set) {
  if (!f_temp_map_inited) {
    getFaceTempMap(v_set);
  }

  for (map<Face, vector<VolumeIndex>>::iterator it = f_temp_map.begin();
       it != f_temp_map.end(); it++) {
    f_set.push_back(it->first);
  }
#ifdef LOG
  cout << "f_set: " << f_set.size() << endl;
#ifdef WINDOWS
  saveData("..\\log\\f_set.txt", f_set);
#endif
#ifdef LINUX
  saveData("../../log/f_set.txt", f_set);
#endif
#endif
}

void DataFiller::getFaceNearNode(const vector<Volume> &v_set,
                                 const vector<Face> &f_set,
                                 vector<set<NodeIndex>> &face_near_n) {
  if (!f_temp_map_inited) {
    getFaceTempMap(v_set);
  }

  face_near_n.reserve(f_set.size());
  for (int i = 0; i < f_set.size(); i++) {
    set<NodeIndex> _set;
    face_near_n.push_back(_set);
  }

  for (int i = 0; i < f_set.size(); i++) {
    for (int j = 0; j < f_temp_map[f_set[i]].size(); j++) {
      VolumeIndex v_index = f_temp_map[f_set[i]][j];
      Volume v = v_set[v_index];
      for (int j = 0; j < 4; j++) {
        if (!f_set[i].contains(v[j])) {
          face_near_n[i].insert(v[j]);
          break;
        }
      }
    }
  }

#ifdef LOG
  cout << "face_near_n: " << face_near_n.size() << endl;
#ifdef WINDOWS
  saveData("..\\log\\face_near_n.txt", face_near_n);
#endif
#ifdef LINUX
  saveData("../../log/face_near_n.txt", face_near_n);
#endif
#endif
}

void DataFiller::getFaceNearVolume(const vector<Volume> &v_set,
                                   const vector<Face> &f_set,
                                   vector<set<VolumeIndex>> &face_near_v) {
  if (!f_temp_map_inited) {
    getFaceTempMap(v_set);
  }

  face_near_v.reserve(f_set.size());
  for (int i = 0; i < f_set.size(); i++) {
    set<VolumeIndex> _set;
    face_near_v.push_back(_set);
  }

  for (int i = 0; i < f_set.size(); i++) {
    for (int j = 0; j < f_temp_map[f_set[i]].size(); j++) {
      face_near_v[i].insert(f_temp_map[f_set[i]][j]);
    }
  }
  
#ifdef LOG
  cout << "face_near_v: " << face_near_v.size() << endl;
#ifdef WINDOWS
  saveData("..\\log\\face_near_v.txt", face_near_v);
#endif
#ifdef LINUX
  saveData("../../log/face_near_v.txt", face_near_v);
#endif
#endif
}

void DataFiller::getOneRingVolume(const int &n_num, const vector<Volume> &v_set,
                                  vector<set<VolumeIndex>> &ring_1_v) {
  ring_1_v.reserve(n_num);
  for (int i = 0; i < n_num; i++) {
    set<VolumeIndex> _set;
    ring_1_v.push_back(_set);
  }

  for (int i = 0; i < v_set.size(); i++) {
    for (int j = 0; j < 4; j++) {
      ring_1_v[v_set[i][j]].insert(i);
    }
  }

#ifdef LOG
  cout << "ring_1_v: " << ring_1_v.size() << endl;
#ifdef WINDOWS
  saveData("..\\log\\ring_1_v.txt", ring_1_v);
#endif
#ifdef LINUX
  saveData("../../log/ring_1_v.txt", ring_1_v);
#endif
#endif
}

void DataFiller::getOneRingNode(const int &n_num, const vector<Volume> &v_set,
                                vector<set<NodeIndex>> &ring_1_n) {
  ring_1_n.reserve(n_num);
  for (int i = 0; i < n_num; i++) {
    set<NodeIndex> _set;
    ring_1_n.push_back(_set);
  }

  for (int i = 0; i < v_set.size(); i++) {
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 3; k++) {
        ring_1_n[v_set[i][j]].insert(v_set[i][(j + 1 + k) % 4]);
      }
    }
  }

#ifdef LOG
  cout << "ring_1_n: " << ring_1_n.size() << endl;
#ifdef WINDOWS
  saveData("..\\log\\ring_1_n.txt", ring_1_n);
#endif
#ifdef LINUX
  saveData("../../log/ring_1_n.txt", ring_1_n);
#endif
#endif
}

void DataFiller::getOneRingFace(const int &n_num,
                                const vector<set<NodeIndex>> &face_near_n,
                                vector<set<FaceIndex>> &ring_1_f) {
  ring_1_f.reserve(n_num);
  for (int i = 0; i < n_num; i++) {
    set<FaceIndex> _set;
    ring_1_f.push_back(_set);
  }

  for (int i = 0; i < face_near_n.size(); i++) {
    for (set<NodeIndex>::iterator it = face_near_n[i].begin();
         it != face_near_n[i].end(); it++) {
      ring_1_f[*it].insert(i);
    }
  }

#ifdef LOG
  cout << "ring_1_f: " << ring_1_f.size() << endl;
#ifdef WINDOWS
  saveData("..\\log\\ring_1_f.txt", ring_1_f);
#endif
#ifdef LINUX
  saveData("../../log/ring_1_f.txt", ring_1_f);
#endif
#endif
}

void DataFiller::getFaceType(const Plain &inlet, const Plain &outlet,
                             const vector<Face> &f_set,
                             const vector<set<NodeIndex>> &face_near_n,
                             vector<ElementType> &f_type) {
  assert(tools != NULL);

  f_type.reserve(face_near_n.size());
  for (int i = 0; i < face_near_n.size(); i++) {
    ElementType _type;
    f_type.push_back(_type);
  }

  int hard_num = 0, inlet_num = 0, outlet_num = 0, inner_num = 0;

  for (int i = 0; i < face_near_n.size(); i++) {
    switch (face_near_n[i].size()) {
    case 1: {
      f_type[i] = HARD;
      hard_num++;

      bool isInlet = true;
      for (int j = 0; j < FACE_NODE_NUM; j++) {
        if (!tools->in_plain(f_set[i][j], inlet)) {
          isInlet = false;
          break;
        }
      }
      if (isInlet) {
        f_type[i] = INLET;
        hard_num--;
        inlet_num++;
        break;
      }

      bool isOutlet = true;
      for (int j = 0; j < FACE_NODE_NUM; j++) {
        if (!tools->in_plain(f_set[i][j], outlet)) {
          isOutlet = false;
          break;
        }
      }
      if (isOutlet) {
        f_type[i] = OUTLET;
        hard_num--;
        outlet_num++;
        break;
      }

      break;
    }
    case 2: {
      f_type[i] = INNER;
      inner_num++;
      break;
    }
    default:
      assert(0); // face_near_n[i].size() == 1|2
      break;
    }
  }

#ifdef LOG
  cout << "f_type: " << f_type.size() << " hard: " << hard_num
       << " inlet: " << inlet_num << " outlet: " << outlet_num
       << " inner: " << inner_num << endl;
#ifdef WINDOWS
  saveData("..\\log\\f_type.txt", f_type);
#endif
#ifdef LINUX
  saveData("../../log/f_type.txt", f_type);
#endif
#endif
}

void DataFiller::getNodeType(const int &n_num, const vector<Face> &f_set,
                             const vector<ElementType> &f_type,
                             vector<ElementType> &n_type) {
  n_type.reserve(n_num);
  for (int i = 0; i < n_num; i++) {
    ElementType _type = INNER;
    n_type.push_back(_type);
  }

  for (int i = 0; i < f_type.size(); i++) {
    for (int j = 0; j < FACE_NODE_NUM; j++) {
      if (f_type[i] > n_type[f_set[i][j]]) {
        n_type[f_set[i][j]] = f_type[i];
      }
    }
  }

  int hard_num = 0, inlet_num = 0, outlet_num = 0, inner_num = 0;
  for (int i = 0; i < n_type.size(); i++) {
    switch (n_type[i]) {
    case INNER:
      inner_num++;
      break;
    case HARD:
      hard_num++;
      break;
    case INLET:
      inlet_num++;
      break;
    case OUTLET:
      outlet_num++;
      break;
    default:
      break;
    }
  }

#ifdef LOG
  cout << "n_type: " << n_type.size() << " hard: " << hard_num
       << " inlet: " << inlet_num << " outlet: " << outlet_num
       << " inner: " << inner_num << endl;
#ifdef WINDOWS
  saveData("..\\log\\n_type.txt", n_type);
#endif
#ifdef LINUX
  saveData("../../log/n_type.txt", n_type);
#endif
#endif
}

void DataFiller::getSurfNodeNearFace(const int &n_num,
                                     const vector<Face> &f_set,
                                     const vector<ElementType> &f_type,
                                     vector<set<FaceIndex>> &surf_node_near_f) {
  surf_node_near_f.reserve(n_num);
  for (int i = 0; i < n_num; i++) {
    set<FaceIndex> _set;
    surf_node_near_f.push_back(_set);
  }

  for (int i = 0; i < f_set.size(); i++) {
    if (f_type[i] == HARD || f_type[i] == INLET || f_type[i] == OUTLET) {
      Face f = f_set[i];
      for (int j = 0; j < FACE_NODE_NUM; j++) {
        surf_node_near_f[f[j]].insert(i);
      }
    }
  }
}

#ifdef TEST
void DataFiller::getNodeTypeVec(const vector<ElementType> &n_type,
                                VectorXd &n_type_vec) {
  n_type_vec.resize(n_type.size());
  for (int i = 0; i < n_type.size(); i++) {
    switch (n_type[i]) {
    case INNER:
      n_type_vec(i, 0) = 0;
      break;
    case HARD:
      n_type_vec(i, 0) = 1;
      break;
    case INLET:
    case OUTLET:
      n_type_vec(i, 0) = 0.5;
      break;
    default:
      assert(0);
      break;
    }
  }
}
#endif

int DataFiller::saveData(const string filename, const vector<Face> &vec_face) {
  ofstream output(filename.c_str());
  if (!output.is_open()) {
    return ERR_FILE_OPEN;
  }

  for (int i = 0; i < vec_face.size(); i++) {
    output << i << ":\t";
    for (int j = 0; j < FACE_NODE_NUM; j++) {
      output << vec_face[i][j] << " ";
    }
    output << endl;
  }

  output.close();
  return 0;
}

int DataFiller::saveData(const string filename,
                         const vector<set<int>> &vec_set_int) {
  ofstream output(filename.c_str());
  if (!output.is_open()) {
    return ERR_FILE_OPEN;
  }

  for (int i = 0; i < vec_set_int.size(); i++) {
    output << i << ":\t";
    for (set<int>::iterator it = vec_set_int[i].begin();
         it != vec_set_int[i].end(); it++) {
      output << *it << " ";
    }
    output << endl;
  }

  output.close();
  return 0;
}

int DataFiller::saveData(const string filename,
                         const vector<ElementType> &vec_type) {
  ofstream output(filename.c_str());
  if (!output.is_open()) {
    return ERR_FILE_OPEN;
  }

  for (int i = 0; i < vec_type.size(); i++) {
    output << i << ":\t";
    switch (vec_type[i]) {
    case INNER:
      output << "INNER" << endl;
      break;
    case HARD:
      output << "HARD" << endl;
      break;
    case INLET:
      output << "INLET" << endl;
      break;
    case OUTLET:
      output << "OUTLET" << endl;
      break;
    default:
      assert(0);
      break;
    }
  }

  output.close();
  return 0;
}
