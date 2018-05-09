
def convert_log_to_json(model_name):
    json = {}
    with open('simulator/result/' + str(model_name) + '.log.txt', 'r') as log_file:
        for line in log_file.readlines():
            if 'input_energy' in line and 'tl' in line:
                strings = line.split(' ')
                freq = strings[2]
                input_energy = strings[5]
                inlet_energy = strings[8]
                output_energy = strings[11]
                tl = strings[14]
                json[freq] = tl.strip()
    return json


    
