from __future__ import print_function
import requests
import json
import hashlib
import os
import sys


def generate_file_md5(filename, blocksize=2 ** 20):
    m = hashlib.md5()
    with open(filename, "rb") as f:
        while True:
            buf = f.read(blocksize)
            if not buf:
                break
            m.update(buf)
    return m.hexdigest()


def create_pool(url, pool_name, docker_id, app_id):
    upload_value = {"pool_name": pool_name, "docker_image": docker_id, "app": app_id}
    try:
        r = requests.post(url + 'pool/create/', data=upload_value)
    except requests.exceptions.RequestException as e:
        sys.exit(1)

    result_json = json.loads(json.loads(r.content.decode('utf-8'))["value"])[0]
    index = result_json["pk"]
    info = result_json["fields"]

    return [info, index]


def query_job_by_pool(url, pool_id):
    try:
        r = requests.get(url + 'pool/job/?belong_pool=' + str(pool_id))
    except requests.exceptions.RequestException as e:
        print (e)
        sys.exit(1)

    result_json = json.loads(json.loads(r.content.decode('utf-8'))["value"])
    jobs = {}
    for job in result_json:
        jobs[str(job["pk"])] = job["fields"]
    return jobs


def query_pool(url, pool_id):
    try:
        r = requests.get(url + 'pool/?id=' + str(pool_id))
    except requests.exceptions.RequestException as e:
        print (e)
        sys.exit(1)

    if r.status_code == 200:
        result_json = json.loads(json.loads(r.content.decode('utf-8'))["value"])[0]
        index = result_json["pk"]
        info = result_json["fields"]
        return [info, index]
    else:
        return [None, None]


def delete_pool(url, pool_id, access_token=None):
    upload_value = {"pool_id": pool_id, "access_token": access_token}
    try:
        r = requests.post(url + 'pool/delete/', data=upload_value)
    except requests.exceptions.RequestException as e:
        print (e)
        sys.exit(1)

    print (r.content)
    return r.content


def stop_pool(url, pool_id, access_token=None):
    upload_value = {"pool_id": pool_id, "access_token": access_token}
    try:
        r = requests.post(url + 'pool/stop/', data=upload_value)
    except requests.exceptions.RequestException as e:
        print (e)
        sys.exit(1)

    return r.content


def resume_pool(url, pool_id, access_token=None):
    upload_value = {"pool_id": pool_id, "access_token": access_token}
    try:
        r = requests.post(url + 'pool/resume/', data=upload_value)
    except requests.exceptions.RequestException as e:
        print (e)
        sys.exit(1)

    return r.content


def create_job(url, pool_id, data_id, config_id=None, access_token=None):
    upload_value = {"pool_id": pool_id, "data_id": data_id, "access_token": access_token}
    if config_id is not None:
        upload_value["config_id"] = config_id

    try:
        r = requests.post(url + 'pool/job/create/', data=upload_value)
    except requests.exceptions.RequestException as e:
        print (e)
        sys.exit(1)

    if r.status_code != 200:
        print (r.content)
        return [None, None]
    else:
        result_json = json.loads(json.loads(r.content.decode('utf-8'))["value"])[0]
        index = result_json["pk"]
        info = result_json["fields"]
        return [info, index]


def upload_file(url, upload_filename, filename=None, file_origin=None, file_type=None, md5=None, original_filename=None):
    upload_value = {'type': file_type, 'filename': filename, 'origin': file_origin, 'md5': md5,
                    'original_filename': original_filename}
    upload_file = open(upload_filename, 'rb')
    upload_files = {'file': upload_file}

    try:
        r = requests.post(url + 'file_manager/upload/', data=upload_value, files=upload_files)
    except requests.exceptions.RequestException as e:
        return [{}, None]

    upload_file.close()
    json_obj = r.json()
    if "value" in json_obj:
        json_obj = json.loads(json_obj["value"])[0]
        return [json_obj['fields'], json_obj['pk']]
    else:
        return [{}, None]


def query_file(url, md5='', id=''):
    first_parameter = True
    id = str(id)
    get_url = url + 'file_manager/'
    if md5 != '':
        if first_parameter:
            get_url += '?'
            first_parameter = False
        else:
            get_url += '&'
        get_url += 'md5=' + md5
    if id != '':
        if first_parameter:
            get_url += '?'
            first_parameter = False
        else:
            get_url += '&'
        get_url += 'id=' + id

    try:
        r = requests.get(get_url)
    except requests.exceptions.RequestException as e:
        print (e)
        sys.exit(1)

    if r.status_code == 200:
        json_obj = json.loads(r.content.decode('utf-8'))
        if "value" in json_obj:
            json_obj = json.loads(json_obj["value"])[0]
            return [json_obj['fields'], json_obj['pk']]
        else:
            return [{}, None]
    else:
        return [{}, None]


def download_file(file_url, path, filename):
    file_path = os.path.join(path, filename)

    try:
        r = requests.get(file_url)
    except requests.exceptions.RequestException as e:
        return None

    with open(file_path, 'wb') as f:
        for chunk in r.iter_content(chunk_size=1024):
            if chunk:
                f.write(chunk)


def report_job(url, hostname, thread, job_id, output_file_id, log_file_id):
    upload_value = {"hostname": hostname, "thread": thread, "job_id": job_id,
                    "output_file_id": output_file_id, "log_file_id": log_file_id}

    try:
        r = requests.post(url + "pool/job/report/", data=upload_value)
    except requests.exceptions.RequestException as e:
        return [{}, None]

    json_obj = r.json()
    if "value" in json_obj:
        json_obj = json.loads(json_obj["value"])[0]
        return [json_obj['fields'], json_obj['pk']]
    else:
        return [{}, None]
