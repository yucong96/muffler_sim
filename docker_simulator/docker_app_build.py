import sys
sys.path.append('../libs/zycwn_lib')

import os

import create_pool
import pull_pool

if __name__ == '__main__':
    # create app
    app_filepath = 'app.zip'
    docker_image_id = 2
    [pool_id, pool_token] = create_pool.create_pool(app_filepath, docker_image_id)
    pool_id_path = 'pool_id.txt'
    with open(pool_id_path, 'w') as f:
        f.write(str(pool_id).strip())
        f.close()
    pool_token_path = 'pool_token.txt'
    with open(pool_token_path, 'w') as f:
        f.write(str(pool_token).strip())
        f.close()
    # create config dir
    config_dir = 'config'
    os.system('mkdir ' + config_dir)
    with open(config_dir+'/empty_data.txt', 'w') as f:
        f.write(' ')
        f.close()
    # create pool dir
    pool_dir = 'pool'
    pull_pool.pull_pool(pool_id, pool_dir)
