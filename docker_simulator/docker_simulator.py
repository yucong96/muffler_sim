
from simulator import simulator as sim
import print_status
import add_job

PROJECT_DIR = '/home/scallion/Documents/project/project_active/muffler_sim/'
CONFIG_DIR = PROJECT_DIR + 'docker_simulator/config.temp/'
POOL_DIR = PROJECT_DIR + 'docker_simulator/pool/'
TOKEN_PATH = PROJECT_DIR + 'docker_simulator/pool_token.txt'
EMPTY_DATA_PATH = PROJECT_DIR + 'docker_simulator/config.temp/empty_data.txt'

def get_token(token_path):
    with open(token_path, 'r') as f:
        token = f.readline().strip()
        return token

def simulate(data, model_name):
    model_config_path = CONFIG_DIR + model_name + '.txt'
    sim.write_muffler_config(data, model_config_path)
    access_token = get_token(TOKEN_PATH)
    [info, index] = add_job.add_job(POOL_DIR, EMPTY_DATA_PATH, model_config_path, access_token)
    url = '10.76.1.116:8000' + '/result/?id=' + str(index)
    return url

def get_result(model_id):
    err = 0
    result_path = None
    jobs = print_status.print_status(POOL_DIR)
    value = jobs.get(model_id, None)
    if value == None:
        err = 1  # job id not exists
    elif value.status == 'Standby' or value.status == 'Executing':
        err = 2  # job hasn't finished
    elif value.status == 'Finish':
        err = 0
        result_path = POOL_DIR + str(model_id) + '/output'
        print(result_path)
    else:
        err = 3  # unexpected error
    return [err, result_path]
