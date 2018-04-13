
import os

def write_model_config(data_set):
    with open('./simulator/mesh_gen/geo_config/muffler.txt', 'w') as geo_config_file:
        geo_config_file.write(str(len(data_set['chamber'])) + '\n');
        for data in data_set['chamber']:
            geo_config_file.write('0 0 0 ' + str(data['radius']) + ' ' + str(data['length']) + '\n')
        geo_config_file.write(str(data_set['max_mesh_size']));

def mesh_gen(model_name):
    os.system('cd simulator/mesh_gen && ./mesh_gen.sh ' + model_name)

def pre_extract_surface(model_name):
    os.system('cd simulator/extract_surface && pvpython extract_surface.py extract_pure_surf ../mesh_gen/vtk/' + model_name + '.vtk ./pre_surf/' + model_name + '.vtk')
    output_file = '/static/model/' + model_name + '_pre_surf.vtk'
    os.system('cp simulator/extract_surface/pre_surf/' + model_name + '.vtk homepage' + output_file)
    return output_file
