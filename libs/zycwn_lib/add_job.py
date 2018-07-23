from zycwn import BasicApi
from zycwn import WnClient

import sys
import os

def add_job(pool_path, data_file, conf_file, access_token):
    client = WnClient.WnClient(server_url='http://10.76.1.125:29000/')
    pool = client.load_pool(local_path=pool_path)
    [info, index] = pool.create_job(data_filepath=data_file,
		                    config_filepath=conf_file, access_token=access_token)
    return [info, index]
    
if __name__ == '__main__':
    if len(sys.argv) != 5:
        print('add_job.py pool_path data_file conf_file access_token')
    else:
        add_job(sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4])
