#ifndef __DATAFILLER_H__
#define __DATAFILLER_H__

#include "Constant.h"
#include "Object.h"
#include "UtilTools.h"

#include <Eigen/Dense>
#include <map>
#include <set>
#include <vector>

class DataFiller {
private:
  bool f_temp_map_inited;
  std::map<Face, std::vector<VolumeIndex>> f_temp_map;
  const UtilTools *tools;

  void getFaceTempMap(const std::vector<Volume> &v_set);
  void clearFaceTempMap();
  int saveData(const std::string filename, const std::vector<Face> &vec_face);
  int saveData(const std::string filename,
               const std::vector<std::set<int>> &vec_set_int);
  int saveData(const std::string filename,
               const std::vector<ElementType> &vec_type);

 public:
  DataFiller(const UtilTools *_tools);
  DataFiller();
  void set_tools(const UtilTools *_tools);
  
  void clear();
  void getFaceSet(const std::vector<Volume> &v_set, std::vector<Face> &f_set);
  void getFaceNearNode(const std::vector<Volume> &v_set,
		       const std::vector<Face> &f_set,
		       std::vector<std::set<NodeIndex>> &face_near_n);
  void getFaceNearVolume(const std::vector<Volume> &v_set,
			 const std::vector<Face> &f_set,
			 std::vector<std::set<VolumeIndex>> &face_near_v);
  void getOneRingVolume(const int &n_num, const std::vector<Volume> &v_set,
			std::vector<std::set<VolumeIndex>> &ring_1_v);
  void getOneRingNode(const int &n_num, const std::vector<Volume> &v_set,
		      std::vector<std::set<NodeIndex>> &ring_1_n);
  void getOneRingFace(const int &n_num,
		      const std::vector<std::set<NodeIndex>> &face_near_n,
		      std::vector<std::set<FaceIndex>> &ring_1_f);
  void getFaceType(const Plain &inlet, const Plain &outlet,
		   const std::vector<Face> &f_set,
		   const std::vector<std::set<NodeIndex>> &face_near_n,
		   std::vector<ElementType> &f_type);
  void getNodeType(const int &n_num, const std::vector<Face> &f_set,
		   const std::vector<ElementType> &f_type,
		   std::vector<ElementType> &n_type);
  void getSurfNodeNearFace(const int &n_num, const std::vector<Face> &f_set,
			   const std::vector<ElementType> &f_type,
			   std::vector<std::set<FaceIndex>> &surf_node_near_n);
#ifdef TEST
void getNodeTypeVec(const std::vector<ElementType> &n_type,
		    Eigen::VectorXd &n_type_vec);
#endif
};

#endif
