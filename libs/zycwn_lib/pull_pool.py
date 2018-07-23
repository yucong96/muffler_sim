from zycwn import BasicApi
from zycwn import WnClient
import sys

def pull_pool(pool_id, load_path):
    client = WnClient.WnClient(server_url='http://10.76.1.125:29000/')
    pool = client.pull(pool_id=int(pool_id), local_path=load_path)

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print('pull_pool.py pool_id load_path')
    else:
        pull_pool(sys.argv[1], sys.argv[2])
