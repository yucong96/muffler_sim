from . import BasicApi
from . import Setting
import os


def upload_file(url, filepath, filename=None, file_origin=None, file_type=None):
    md5 = BasicApi.generate_file_md5(filename=filepath)
    [info, index] = BasicApi.query_file(url=url, md5=md5)
    if index is not None:
        return [info, index]
    else:
        original_filename = str(filepath)[str(filepath).rfind('/') + 1:]
        [info, index] = BasicApi.upload_file(url=url, upload_filename=filepath, filename=filename,
                                             file_origin=file_origin, file_type=file_type, md5=md5,
                                             original_filename=original_filename)
    return [info, index]


def create_pool(url, pool_name, docker, app_filepath):
    [info, index] = upload_file(url=url, filepath=app_filepath, file_type="app")
    [info, index] = BasicApi.create_pool(url=url, pool_name=pool_name, docker_id=docker, app_id=index)
    return [info, index]


def delete_pool(url, pool_id, access_token=None):
    result = BasicApi.delete_pool(url, pool_id=pool_id, access_token=access_token)
    return result


def create_job(url, pool_id, data_filepath, config_filepath=None, access_token=None):
    config_index = None
    if config_filepath is not None:
        [config_info, config_index] = upload_file(url=url, filepath=config_filepath, file_type="config")
    [data_info, data_index] = upload_file(url=url, filepath=data_filepath, file_type="data")
    [info, index] = BasicApi.create_job(url=url, pool_id=pool_id, data_id=data_index,
                                        config_id=config_index, access_token=access_token)
    return [info, index]


def stop_pool(url, pool_id, access_token=None):
    result = BasicApi.stop_pool(url=url, pool_id=pool_id, access_token=access_token)
    return result


def resume_pool(url, pool_id, access_token=None):
    result = BasicApi.resume_pool(url=url, pool_id=pool_id, access_token=access_token)
    return result


def query_pool(url, pool_id):
    [info, index] = BasicApi.query_pool(url=url, pool_id=pool_id)
    return [info, index]


def query_job_belong_pool(url, pool_id):
    jobs = BasicApi.query_job_by_pool(url=url, pool_id=pool_id)
    return jobs


def report_job(url, hostname, thread, job_id, output_file_id, log_file_id):
    [info, index] = BasicApi.report_job(url=url, hostname=hostname, thread=thread,
                                        job_id=job_id, output_file_id=output_file_id, log_file_id=log_file_id)
    return [info, index]


def sync_file(url, file_id, folderpath, file_name=None):
    [info, index] = BasicApi.query_file(url=url, id=file_id)
    if not file_name:
        if "original_filename" in info and info["original_filename"] and info["original_filename"] != '':
            file_name = info["original_filename"]
        else:
            file_name = info["filename"]
    BasicApi.download_file(file_url=info["download_url"], path=folderpath, filename=file_name)


def sync_job_with_file(url, folderpath, new_job, original_job):
    original_job.status = new_job.status
    if not os.path.isdir(folderpath + '/' + str(new_job.job_id)):
        os.makedirs(folderpath + '/' + str(new_job.job_id))
    job_path = folderpath + '/' + str(new_job.job_id) + '/'
    if new_job.config_id and new_job.config_id != original_job.config_id:
        sync_file(url=url, file_id=new_job.config_id, folderpath=job_path)
        original_job.config_id = new_job.config_id
    if new_job.output_id and new_job.output_id != original_job.output_id:
        sync_file(url=url, file_id=new_job.output_id, folderpath=job_path, file_name="output")
        original_job.output_id = new_job.output_id
    if new_job.data_id and new_job.data_id != original_job.data_id:
        sync_file(url=url, file_id=new_job.data_id, folderpath=job_path)
        original_job.data_id = new_job.data_id
    if new_job.log_id and new_job.log_id != original_job.log_id:
        sync_file(url=url, file_id=new_job.log_id, folderpath=job_path)
        original_job.log_id = new_job.log_id
