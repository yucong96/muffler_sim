
import os
from simulator.result import log_analyzor

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


def write_muffler_config(data, model_name):
    chambers = data['chambers']
    parameters = data['parameters']
    config_file = 'simulator/config/' + model_name + '.txt'
    freq_start    = parameters['Freq_start']
    freq_end      = parameters['Freq_end']
    freq_step     = parameters['Freq_step']
    rho           = parameters['Density']
    speed         = parameters['Speed']
    max_mesh_size = parameters['Mesh_size']
    with open(config_file, "w") as f:
        f.write('# This is the config file for muffler\n\n')
        f.write('chamber_num: ' + str(len(chambers)) + '\n')
        f.write('chamber_radius:')
        for chamber in chambers:
            f.write(' ' + str(chamber['radius']))
        f.write('\n')
        f.write('chamber_length:')
        for chamber in chambers:
            f.write(' ' + str(chamber['length']))
        f.write('\n')
        f.write('max_mesh_size: ' + str(max_mesh_size) + '\n\n')
        f.write('freq_start: ' + str(freq_start) + '\n')
        f.write('freq_end: ' + str(freq_end) + '\n')
        f.write('freq_step: ' + str(freq_step) + '\n')
        f.write('speed: ' + str(speed) + '\n')
        f.write('rho: ' + str(rho) + '\n')
        f.write('p0_real: 1\n')
        f.write('p0_imag: 0\n\n')
        f.write('end')

def run_sim(model_name):
    os.system('simulator/muffler_simulator/run.sh ' + model_name)

def get_sim_result(model_name):
    json = log_analyzor.convert_log_to_json(model_name)
    return json

def cp2homepage(model_name):
    file_list = os.listdir('simulator/result')
    for f in file_list:
        file_name = f.split('.')
        if file_name[0].split('_')[0] == model_name and file_name[len(file_name)-1] == 'vtk':
            file_path = os.path.join('simulator/result', f)
            tartget_path = os.path.join('homepage/static/model', f)
            os.system('cp ' + file_path + ' ' + tartget_path)
