#!/bin/bash

PROJECT_PATH=/home/jchen/Documents/project/project_test/django/muffler_sim

CONFIG_PATH=${PROJECT_PATH}/simulator/config
MESH_PATH=${PROJECT_PATH}/simulator/mesh
RESULT_PATH=${PROJECT_PATH}/simulator/result
RESULT_MODEL_PATH=${PROJECT_PATH}/simulator/result/model
SIMULATOR_PATH=${PROJECT_PATH}/simulator/muffler_simulator
MESHGEN_PATH=${PROJECT_PATH}/simulator/mesh_gen

MODEL_NAME=${1}

${MESHGEN_PATH}/mesh_gen.sh ${MODEL_NAME}
${SIMULATOR_PATH}/build/bin/muffler ${CONFIG_PATH}/${MODEL_NAME} ${MESH_PATH}/${MODEL_NAME} ${RESULT_MODEL_PATH}/${MODEL_NAME} > ${RESULT_PATH}/${MODEL_NAME}.log.txt
