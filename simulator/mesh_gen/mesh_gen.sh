#!/bin/bash

PROJECT_PATH=/home/jchen/Documents/project/project_test/django/muffler_sim

CONFIG_PATH=${PROJECT_PATH}/simulator/config
MESH_PATH=${PROJECT_PATH}/simulator/mesh
MESHGEN_PATH=${PROJECT_PATH}/simulator/mesh_gen

MODEL_NAME=${1}
GEO_CONFIG_PATH=${CONFIG_PATH}/${MODEL_NAME}.txt
GEO_PATH=${MESHGEN_PATH}/geo/${MODEL_NAME}.geo
VOL_PATH=${MESHGEN_PATH}/vol/${MODEL_NAME}.vol
VTK_PATH=${MESH_PATH}/${MODEL_NAME}.vtk

echo ${GEO_CONFIG_PATH}
${MESHGEN_PATH}/geo_gen ${GEO_CONFIG_PATH} ${GEO_PATH}
netgen -geofile=${GEO_PATH} -meshfile=${VOL_PATH} -batchmode
${MESHGEN_PATH}/vol2vtk ${VOL_PATH} ${VTK_PATH}

