#ifndef __COEFFITEM_H__
#define __COEFFITEM_H__

#include "Constant.h"
#include "Object.h"
#include "UtilTools.h"

#include <map>
#include <set>
#include <vector>

class CoeffItem {
private:
  bool coeffitem_inited;
  const UtilTools* tools;
  int n_num;

  std::map<NodeIndex, double> coeff;
  std::vector<double> constant;

 private:
  void insert(NodeIndex n, double coefficient);
  void insert(NodeIndex n, double coefficient, const int flag);

 public:
  CoeffItem(const int _node_num, const UtilTools* _tools);
  CoeffItem();
  void set_nodenum_and_tools(const int _node_num, const UtilTools* _tools);

  friend class CoeffMatrix;
  
  void clear();
  void gradient_dot_norm(const Volume &v, const Face &surf_f,
                         const double multiplier, const int is_normalize, const int flag = REAL);
  void average(const Volume &v, const double multiplier, const int flag = REAL);
  void laplace(const std::vector<Volume> &v_set, const std::vector<Face> &f_set,
               const std::vector< std::set<VolumeIndex> > &ring_1_v,
               const std::vector< std::set<FaceIndex> > &ring_1_f,
               const NodeIndex &n, const double multiplier, const int flag = REAL);
  void surface_laplace(const std::vector<Face> &f_set, const std::vector<std::set<FaceIndex>> &surface_node_near_f, const NodeIndex &n, const double multiplier, const int flag = REAL);
  void own(const NodeIndex& n, const double multiplier, const int flag = REAL);
  void add_constant(const double _constant);
};

#endif
