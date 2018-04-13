#!/bin/bash

MODEL_NAME=${1}
./geo_gen ./geo_config/${MODEL_NAME}.txt ./geo/${MODEL_NAME}.geo
netgen -geofile=./geo/${MODEL_NAME}.geo -meshfile=./vol/${MODEL_NAME}.vol -batchmode
./vol2vtk ./vol/${MODEL_NAME}.vol ./vtk/${MODEL_NAME}.vtk


