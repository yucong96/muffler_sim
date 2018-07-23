#!/bin/bash

PROJECT_PATH=/home/scallion/Documents/project/project_active/muffler_sim

CONFIG_PATH=${PROJECT_PATH}/simulator/config
MESH_PATH=${PROJECT_PATH}/simulator/mesh
RESULT_PATH=${PROJECT_PATH}/simulator/result
RESULT_MODEL_PATH=${PROJECT_PATH}/simulator/result/model
SIMULATOR_PATH=${PROJECT_PATH}/simulator/muffler_simulator
MESHGEN_PATH=${PROJECT_PATH}/simulator/mesh_gen

MODEL_NAME=${1}

#${MESHGEN_PATH}/mesh_gen.sh ${MODEL_NAME}
#echo "begin"
#date +%Y%m%d-%H:%M:%S.%N
${SIMULATOR_PATH}/build/bin/muffler ${CONFIG_PATH}/${MODEL_NAME} ${MESH_PATH}/${MODEL_NAME} ${RESULT_MODEL_PATH}/${MODEL_NAME} > ${RESULT_PATH}/${MODEL_NAME}.log.txt
#echo "first finish"
#date +%Y%m%d-%H:%M:%S.%N
#${SIMULATOR_PATH}/build2/bin/muffler ${CONFIG_PATH}/${MODEL_NAME} ${MESH_PATH}/${MODEL_NAME} ${RESULT_MODEL_PATH}/${MODEL_NAME} > ${RESULT_PATH}/${MODEL_NAME}.log.txt
#echo "second finish"
#date +%Y%m%d-%H:%M:%S.%N
