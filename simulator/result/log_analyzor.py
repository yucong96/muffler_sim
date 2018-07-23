
import math

PROJECT_DIR = '/home/scallion/Documents/project/project_active/muffler_sim/'
LOG_DIR = PROJECT_DIR + 'simulator/result/'

def convert_log_to_json(model_name):
    json = {}
    with open(LOG_DIR + str(model_name) + '.log.txt', 'r') as log_file:
        for line in log_file.readlines():
            if 'input_energy' in line and 'tl' in line:
                strings = line.split(' ')
                freq = strings[2]
                input_energy = strings[5]
                inlet_energy = strings[8]
                output_energy = strings[11]
                tl = float(strings[14].strip())
                tl = math.log10(tl) * 10
                json[freq] = str(tl)
    return json


    
