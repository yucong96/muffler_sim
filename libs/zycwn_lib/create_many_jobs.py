from zycwn import BasicApi
from zycwn import WnClient

import sys
import os

def create_many_jobs(pool_path, data_file, conf_dir, access_token):
    client = WnClient.WnClient(server_url='http://10.76.1.125:29000/')
    pool = client.load_pool(local_path=pool_path)
    for conf_file in os.listdir(conf_dir):
        conf_file = os.path.join(conf_dir, conf_file)	
        pool.create_job(data_filepath=data_file, config_filepath=conf_file, access_token=access_token)

if __name__ == '__main__':
    if len(sys.argv) != 5:
        print('create_many_jobs.py pool_path data_file conf_dir access_token')
    else:
        create_many_jobs(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
                
