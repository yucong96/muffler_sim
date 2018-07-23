from zycwn import BasicApi
from zycwn import WnClient

import sys

def print_status(pool_path):
    client = WnClient.WnClient(server_url="http://10.76.1.125:29000/")
    pool = client.load_pool(local_path=pool_path)
    jobs = pool.pull()
    pool.print_status()
    return jobs
    
if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('print_status.py pool_path')
    else:
        print_status(sys.argv[1])
	
