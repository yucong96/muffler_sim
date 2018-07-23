from __future__ import print_function
import requests
import json
import hashlib
import os
from . import BasicService
from . import Setting


class WnClient:

    def __init__(self, server_url):
        self.server_url = server_url

    def create_pool(self, pool_name, docker, app_filepath):
        [info, index] = BasicService.create_pool(url=self.server_url, pool_name=pool_name, docker=docker,
                                                 app_filepath=app_filepath)
        print ("pool id: %d, pool access token: %s" % (index, info["access_token"]))
        return [index, info['access_token']]

    def delete_pool(self, pool_id, access_token=None):
        result = BasicService.delete_pool(url=self.server_url, pool_id=pool_id, access_token=access_token)
        return result

    def load_pool(self, local_path):
        if os.path.exists(local_path) and os.path.exists(local_path + "/.config.json"):
            return WnPool(server_url=self.server_url, local_path=local_path, config=local_path+"/.config.json")
        else:
            print ("local_path is invalid or config file missing")
            return None

    def pull(self, pool_id, local_path):
        if os.path.exists(local_path):
            print ("local path %s is already exist." % local_path)
            return None
        [info, index] = BasicService.query_pool(url=self.server_url, pool_id=pool_id)
        if index is None:
            print ("pool id %d is invalid." % pool_id)
            return None

        os.makedirs(local_path)
        return WnPool(server_url=self.server_url, local_path=local_path, pool_id=pool_id)


class WnPool:

    def __init__(self, server_url, local_path, pool_id=None, config=None):
        if pool_id is None and config is None:
            print ("pool_id and config cannot both be None.")

        self.local_path = local_path
        self.server_url = server_url

        if config is not None:
            self.config_file_name = config
            self.config = Setting.config_factory(self.config_file_name)
            self.pool_id = self.config.pool_id
        if pool_id is not None:
            self.pool_id = pool_id
            [info, index] = BasicService.query_pool(url=self.server_url, pool_id=pool_id)
            self.config = Setting.Config(pool_id=index, app_id=info["app"])
            # jobs = BasicService.query_job_belong_pool(self.server_url, self.pool_id)
            # for job_id, job_value in jobs.iteritems():
            #     self.config.update_job(job_id=job_id, config_id=job_value["config"],
            #                            data_id=job_value["data"], log_id=job_value["log"],
            #                            output_id=job_value["output"], status=job_value["status"])
            self.pull()
            # self.config.write_to_file(self.local_path+"/.config.json")

    def create_job(self, data_filepath, config_filepath=None, access_token=None):
        [info, index] = BasicService.create_job(url=self.server_url, pool_id=self.pool_id,
                                                data_filepath=data_filepath, config_filepath=config_filepath,
                                                access_token=access_token)
        print ("job created with id %d" % index)
        return [info, index]

    def stop_pool(self, access_token=None):
        result = BasicService.stop_pool(self.server_url, self.pool_id, access_token=access_token)
        print ("result: %s" % result)

    def resume_pool(self, access_token=None):
        result = BasicService.resume_pool(url=self.server_url, pool_id=self.pool_id, access_token=access_token)
        print ("result: %s" % result)

    def pull(self):
        jobs = BasicService.query_job_belong_pool(url=self.server_url, pool_id=self.pool_id)
        for job_id, job_value in jobs.items():
            if str(job_id) not in self.config.jobs:
                self.config.jobs[str(job_id)] = Setting.Job(job_id=job_id)
            new_job = Setting.Job(job_id=job_id, data_id=job_value["data"], config_id=job_value["config"],
                                  log_id=job_value["log"], output_id=job_value["output"],
                                  status=job_value["status"])
            BasicService.sync_job_with_file(url=self.server_url, folderpath=self.local_path,
                                            new_job=new_job, original_job=self.config.jobs[str(job_id)])
        self.config.write_to_file(self.local_path+"/.config.json")
        return self.config.jobs

    def print_status(self):
        for job_id, value in self.config.jobs.items():
            print ("job id: %s, status: %s" % (job_id, value.status))


if __name__ == "__main__":
    print ("not executable")
