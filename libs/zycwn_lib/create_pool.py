from zycwn import BasicApi
from zycwn import WnClient

import sys

def create_pool(app_filepath, docker_image_id):
    client = WnClient.WnClient(server_url='http://10.76.1.125:29000/')
    print('client set up')
    # 2 for '10.76.1.125:25000/netgen:0.2'
    [index, access_token] = client.create_pool(pool_name='muffler_get_data', docker=docker_image_id, app_filepath=app_filepath)
    return [index, access_token]

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('create_pool.py app_filepath docker_image_id')
    else:
        create_pool(sys.argv[1], sys.argv[2])
        
