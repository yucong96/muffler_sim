
import os
import sys
from simulator.result import log_analyzor
from simulator.result import vtk2json


PROJECT_DIR = '/home/scallion/Documents/project/project_active/muffler_sim/'
CONFIG_DIR = PROJECT_DIR + 'simulator/config/'
MESHGEN_DIR = PROJECT_DIR + 'simulator/mesh_gen/'
SIMULATOR_PATH = PROJECT_DIR + 'simulator/muffler_simulator/run.sh'
MESH_DIR = PROJECT_DIR + 'simulator/mesh/'
RESULT_MODEL_DIR = PROJECT_DIR + 'simulator/result/model/'
RESULT_SURF_DIR = PROJECT_DIR + 'simulator/result/surf/'
RESULT_CLIP_DIR = PROJECT_DIR + 'simulator/result/clip/'
EXTRACT_SURFACE_PATH = PROJECT_DIR + 'simulator/extract_surface'
STATIC_DIR = 'static/model/'
PROJECT_STATIC_DIR = PROJECT_DIR + STATIC_DIR


def write_muffler_config(data, model_config_file):
    chambers = data['chambers']
    parameters = data['parameters']
    freq_start    = parameters['Freq_start']
    freq_end      = parameters['Freq_end']
    freq_step     = parameters['Freq_step']
    rho           = parameters['Density']
    speed         = parameters['Speed']
    max_mesh_size = parameters['Mesh_size']
    with open(model_config_file, "w") as f:
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

        
def mesh_gen(model_name, model_config_path, model_mesh_path):
    os.system(MESHGEN_DIR + 'mesh_gen.sh ' + MESHGEN_DIR + ' ' + model_name + ' ' + model_config_path + ' ' + model_mesh_path)

    
def extract_surf(model_path, surf_path=None, clip_path=None):
    if surf_path != None:
        os.system('pvpython ' + EXTRACT_SURFACE_PATH + '/extract_surface.py extract_pure_surf ' + model_path + ' ' + surf_path)
    if clip_path != None:
        os.system('pvpython ' + EXTRACT_SURFACE_PATH + '/extract_surface.py extract_clip_surf ' + model_path + ' ' + clip_path)


def copy(origin_path, target_path):
    os.system('cp ' + origin_path + ' ' + target_path)

    
def run_sim(model_name):
    os.system(SIMULATOR_PATH + ' ' + model_name)

    
def get_sim_result(model_name):
    json = log_analyzor.convert_log_to_json(model_name)
    return json


def post_process(model_name, freq):
    file_list = os.listdir(RESULT_MODEL_PATH)
    for f in file_list:
        basename = f.split('.')[0]
        postfix = f.split('.')[-1]
        if basename.split('_')[0] == model_name and postfix == 'vtk':
            model_path = os.path.join(RESULT_MODEL_PATH, f)
            surf_path = os.path.join(RESULT_SURF_PATH, f)
            clip_path = os.path.join(RESULT_CLIP_PATH, f)
            surf_json_path = os.path.join(STATIC_DIR, basename + '_surf.json')
            clip_json_path = os.path.join(STATIC_DIR, basename + '_clip.json')
            extract_surf(model_path, surf_path, clip_path)
            vtk2json.vtk2json(surf_path, surf_json_path)
            vtk2json.vtk2json(clip_path, clip_json_path)

            

def preview_model(data, model_name):
    model_config_path = CONFIG_DIR + model_name + '.txt'
    model_mesh_path = MESH_DIR + model_name + '.vtk'
    model_surf_path = RESULT_SURF_DIR + model_name + '.vtk'
    model_static_path = PROJECT_STATIC_DIR + model_name + '.vtk'
    model_front_path = STATIC_DIR + model_name + '.vtk'
    write_muffler_config(data, model_config_path)
    mesh_gen(model_name, model_config_path, model_mesh_path)
    extract_surf(model_mesh_path, surf_path=model_surf_path)
    copy(model_surf_path, model_static_path)
    return model_front_path


def single_sim(data, model_name):
    freq = data['parameters']['Freq_start']
    model_config_path = CONFIG_DIR + model_name + '.txt'
    model_mesh_path = MESH_DIR + model_name + '.vtk'
    model_result_path = {}
    model_result_path['real'] = RESULT_MODEL_DIR + model_name + '_real_' + str(freq) + '.vtk'
    model_result_path['imag'] = RESULT_MODEL_DIR + model_name + '_imag_' + str(freq) + '.vtk'
    model_surf_path = {}
    model_surf_path['real'] = RESULT_SURF_DIR + model_name + '_' + str(freq) + '_real.vtk'
    model_surf_path['imag'] = RESULT_SURF_DIR + model_name + '_' + str(freq) + '_imag.vtk'
    model_clip_path = {}
    model_clip_path['real'] = RESULT_CLIP_DIR + model_name + '_' + str(freq) + '_real.vtk'
    model_clip_path['imag'] = RESULT_CLIP_DIR + model_name + '_' + str(freq) + '_imag.vtk'
    model_json_path = {}
    model_json_path['real-surf'] = PROJECT_STATIC_DIR + model_name + '_' + str(freq) + '_real_surf.json'
    model_json_path['real-clip'] = PROJECT_STATIC_DIR + model_name + '_' + str(freq) + '_real_clip.json'
    model_json_path['imag-surf'] = PROJECT_STATIC_DIR + model_name + '_' + str(freq) + '_imag_surf.json'
    model_json_path['imag-clip'] = PROJECT_STATIC_DIR + model_name + '_' + str(freq) + '_imag_clip.json'
    model_front_path = {}
    model_front_path['real-surf'] = STATIC_DIR + model_name + '_' + str(freq) + '_real_surf.json'
    model_front_path['real-clip'] = STATIC_DIR + model_name + '_' + str(freq) + '_real_clip.json'
    model_front_path['imag-surf'] = STATIC_DIR + model_name + '_' + str(freq) + '_imag_surf.json'
    model_front_path['imag-clip'] = STATIC_DIR + model_name + '_' + str(freq) + '_imag_clip.json'
    write_muffler_config(data, model_config_path)
    mesh_gen(model_name, model_config_path, model_mesh_path)
    run_sim(model_name)
    freq_tl = get_sim_result(model_name)
    extract_surf(model_result_path['real'], surf_path=model_surf_path['real'], clip_path=model_clip_path['real'])
    vtk2json.vtk2json(model_surf_path['real'], model_json_path['real-surf'])
    vtk2json.vtk2json(model_clip_path['real'], model_json_path['real-clip'])
    extract_surf(model_result_path['imag'], surf_path=model_surf_path['imag'], clip_path=model_clip_path['imag'])
    vtk2json.vtk2json(model_surf_path['imag'], model_json_path['imag-surf'])
    vtk2json.vtk2json(model_clip_path['imag'], model_json_path['imag-clip'])
    return freq_tl, model_front_path
    

def simulate(data, model_name):
    model_config_path = CONFIG_DIR + model_name + '.txt'
    model_mesh_path = MESH_DIR + model_name + '.vtk'
    write_muffler_config(data, model_config_path)
    mesh_gen(model_name, model_config_path, model_mesh_path)
    run_sim(model_name)
    freq_tl = get_sim_result(model_name)
    post_process(model_name)
