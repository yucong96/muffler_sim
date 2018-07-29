
import sys

from tf_simulator import tf_model

PROJECT_DIR = '/home/scallion/Documents/project/project_active/muffler_sim/'
TF_MODEL_PATH = PROJECT_DIR + 'tf_simulator/model_final/model'
TF_LOG_PATH = PROJECT_DIR + 'tf_simulator/log/muffler.txt'

def tensorflow_train(test_flag=False):
    if test_flag == False:
        tf_model.train()        
    else:
        tf_model.train('../tf_simulator/train', '../tf_simulator/validate', '../tf_simulator/log/nn_log.txt', '../tf_simulator/nn_model/model')

def tensorflow_predict(data, model_name, test_flag=False):
    chambers = data['chambers']
    parameters = data['parameters']
    freq_start    = int(parameters['Freq_start'])
    freq_end      = int(parameters['Freq_end'])
    freq_step     = int(parameters['Freq_step'])
    rho           = parameters['Density']
    speed         = parameters['Speed']

    batches_data = []
    for freq in range(freq_start, freq_end, freq_step):
        x_data = []
        x_data.append(float(chambers[1]['radius']))
        x_data.append(float(chambers[1]['length']))
        x_data.append(freq)
        y_data = 0
        batches_data.append([x_data, y_data])

    if test_flag == False:
        freq_tl = tf_model.predict(batches_data, TF_LOG_PATH, TF_MODEL_PATH)
    else:
        freq_tl = tf_model.predict(batches_data, TF_LOG_PATH, TF_MODEL_PATH)
    for freq in freq_tl:
        tl = float(freq_tl[freq])
        freq_tl[freq] = str(10*tl)
    return freq_tl
