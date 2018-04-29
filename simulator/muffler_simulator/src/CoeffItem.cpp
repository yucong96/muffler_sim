#include "CoeffItem.h"

#include <Eigen/Dense>

#ifdef TEST
#include <iostream>
#endif

using namespace std;
using namespace Eigen;

CoeffItem::CoeffItem(const int _n_num, const UtilTools *_tools) {
  set_nodenum_and_tools(_n_num, _tools);
}

CoeffItem::CoeffItem() { coeffitem_inited = false; }

void CoeffItem::set_nodenum_and_tools(const int _n_num,
                                      const UtilTools *_tools) {
  tools = _tools;
  n_num = _n_num;
  coeffitem_inited = true;
}

void CoeffItem::insert(NodeIndex n, double coefficient) {
  assert(coeffitem_inited);

  if (coeff.find(n) == coeff.end()) {
    coeff.insert(map<NodeIndex, double>::value_type(n, coefficient));
  } else {
    coeff[n] += coefficient;
  }
}

void CoeffItem::insert(NodeIndex n, double coefficient, const int flag) {
  assert(coeffitem_inited);

  if (flag == REAL) {
    insert(n, coefficient);
  } else {
    insert(n + n_num, coefficient);
  }
}

void CoeffItem::clear() {
  assert(coeffitem_inited);

  coeff.clear();
  constant.clear();
}

void CoeffItem::gradient_dot_norm(const Volume &v, const Face &surf_f,
                                  const double multiplier,
                                  const int is_normalize, const int flag) {
  assert(coeffitem_inited);

  // determine inner node
  NodeIndex inner_n = -1;
  for (int i = 0; i < VOLUME_NODE_NUM; i++) {
    if (!surf_f.contains(v[i])) {
      inner_n = v[i];
      break;
    }
  }
  assert(inner_n != -1);

  // compute surface norm
  Vector3d surf_norm;
  if (is_normalize == AREA_NORMALIZE) {
    surf_norm = tools->norm_vec(surf_f, inner_n);
    surf_norm = surf_norm / 2;
  } else if (is_normalize == UNIT_NORMALIZE) {
    surf_norm = tools->norm_vec(surf_f, inner_n);
    surf_norm /= surf_norm.norm();
  } else {
    assert(0);
  }

  // compute coefficient & add to CoeffItem
  double volume = tools->volume(v);
  for (int i = 0; i < 4; i++) {
    NodeIndex n = v[i];
    Face f;
    for (int j = 0; j < 3; j++) {
      f[j] = v[(i + 1 + j) % 4];
    }
    Vector3d norm = -tools->norm_vec(f, n);
    norm = norm / (6 * volume);
    double coefficient = norm.dot(surf_norm) * multiplier;
    insert(v[i], coefficient, flag);
  }
}

void CoeffItem::average(const Volume &v, const double multiplier, int flag) {
  assert(coeffitem_inited);

  for (int i = 0; i < 4; i++) {
    double coefficient = 1.0 / 4 * multiplier;
    insert(v[i], coefficient, flag);
  }
}

void CoeffItem::laplace(const vector<Volume> &v_set, const vector<Face> &f_set,
                        const vector<set<VolumeIndex>> &ring_1_v,
                        const vector<set<FaceIndex>> &ring_1_f,
                        const NodeIndex &n, const double multiplier, int flag) {
  assert(coeffitem_inited);

  // compute 1-ring volume
  double ring_1_v_volume = 0;
  for (set<VolumeIndex>::iterator it = ring_1_v[n].begin();
       it != ring_1_v[n].end(); it++) {
    ring_1_v_volume += tools->volume(v_set[*it]);
  }

  // \laplace = \sum{ \grad * \norm } / ring_1_v_volume (From Green Formula)
  for (set<FaceIndex>::iterator it = ring_1_f[n].begin();
       it != ring_1_f[n].end(); it++) {
    // construct each face and its corresponding volume
    Face opposite_f = f_set[*it];
    Volume near_v;
    for (int i = 0; i < 3; i++) {
      near_v[i] = opposite_f[i];
    }
    near_v[3] = n;

    // compute integral in each volume (using Green Formula)
    gradient_dot_norm(near_v, opposite_f, 1.0 / ring_1_v_volume, AREA_NORMALIZE,
                      flag);
  }
}

void CoeffItem::surface_laplace(
    const vector<Face> &f_set,
    const vector<set<FaceIndex>> &surface_node_near_f, const NodeIndex &n,
    const double multiplier, const int flag) {
  assert(coeffitem_inited);
  assert(surface_node_near_f[n].size() > 0); // n should be in the surface

  double area = 0;
  for (set<FaceIndex>::iterator it = surface_node_near_f[n].begin();
       it != surface_node_near_f[n].end(); it++) {
    Face f = f_set[*it];
    area += tools->area(f);
  }
  area /= 4;

  for (set<FaceIndex>::iterator it = surface_node_near_f[n].begin();
       it != surface_node_near_f[n].end(); it++) {
    Face f = f_set[*it];
    for (int i = 0; i < FACE_NODE_NUM; i++) {
      if (f[i] == n) {
        double cot1 = tools->cot(f, f[(i + 1) % FACE_NODE_NUM]);
        insert(f[(i + 2) % FACE_NODE_NUM], multiplier * cot1 / (2 * area),
               flag);
        double cot2 = tools->cot(f, f[(i + 2) % FACE_NODE_NUM]);
        insert(f[(i + 1) % FACE_NODE_NUM], multiplier * cot2 / (2 * area),
               flag);
        insert(f[i], -multiplier * (cot1 + cot2) / (2 * area), flag);
        break;
      }
    }
  }
}

void CoeffItem::own(const NodeIndex &n, const double multiplier, int flag) {
  assert(coeffitem_inited);

  insert(n, multiplier, flag);
}

void CoeffItem::add_constant(const double _constant) {
  assert(coeffitem_inited);

  constant.push_back(_constant);
}
