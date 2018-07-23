import json


def j_default(o):
    return o.__dict__


class Job:

    def __init__(self, job_id, config_id=None, data_id=None, log_id=None, output_id=None, status=None):
        self.job_id = int(job_id)
        self.config_id = config_id
        self.data_id = data_id
        self.log_id = log_id
        self.output_id = output_id
        self.status = status

    def update(self, config_id=None, data_id=None, log_id=None, output_id=None, status=None):
        if config_id:
            self.config_id = config_id
        if data_id:
            self.data_id = data_id
        if log_id:
            self.log_id = log_id
        if output_id:
            self.output_id = output_id
        if status:
            self.status = status


class Config:

    def __init__(self, pool_id, app_id):
        self.pool_id = int(pool_id)
        self.app_id = int(app_id)
        self.jobs = {}

    def update_job(self, job_id, config_id=None, data_id=None, log_id=None, output_id=None, status=None):
        if job_id in self.jobs:
            self.jobs[str(job_id)].update(config_id=config_id, data_id=data_id, log_id=log_id,
                                          output_id=output_id, status=status)
        else:
            self.jobs[str(job_id)] = Job(job_id=job_id, config_id=config_id, data_id=data_id, log_id=log_id,
                                         output_id=output_id, status=status)

    def write_to_file(self, file_path):
        with open(file_path, 'w') as outfile:
            json.dump(self, outfile, default=j_default, indent=4)


def config_factory(config_json_file_path):
    with open(config_json_file_path, "r") as json_file:
        config_json = json.load(json_file)
    config = Config(pool_id=config_json["pool_id"], app_id=config_json["app_id"])
    if "jobs" in config_json:
        for key, value in config_json["jobs"].items():
            config.update_job(job_id=key)
            if "config_id" in value:
                config.update_job(job_id=key, config_id=value["config_id"])
            if "data_id" in value:
                config.update_job(job_id=key, data_id=value["data_id"])
            if "output_id" in value:
                config.update_job(job_id=key, output_id=value["output_id"])
            if "log_id" in value:
                config.update_job(job_id=key, log_id=value["log_id"])
            if "status" in value:
                config.update_job(job_id=key, status=value["status"])
    return config
