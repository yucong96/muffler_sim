from django.shortcuts import render
from django.http import HttpResponse
from django.utils.encoding import smart_str
from wsgiref.util import FileWrapper

import os
import random
import time
import json

from simulator import simulator as sim
from tf_simulator import tf_simulator as tf_sim
from docker_simulator import docker_simulator as d_sim

# Create your views here.
def index(request):
    return render(request, 'index.html')

def preview_model(request):
    reply = {}
    if (request.method == 'POST'):
        data = json.loads(request.body.decode())
        model_name = 'muffler'
        output_file = sim.preview_model(data, model_name)
        reply['result'] = 'success'
        reply['surf-file'] = output_file
        return HttpResponse(json.dumps(reply), content_type='application/json')
    else:
        reply['result'] = 'fail'
        return HttpResponse(json.dumps(reply), content_type='application/json')

def quick_sim_model(request):
    reply = {}
    if (request.method == 'POST'):
        data = json.loads(request.body.decode())
        model_name = 'muffler'
        freq_tl = tf_sim.tensorflow_predict(data, model_name)
        reply['result'] = 'success'
        reply['freq_tl'] = freq_tl
        return HttpResponse(json.dumps(reply), content_type='application/json')
    else:
        reply['result'] = 'fail'
        return HttpResponse(json.dumps(reply), content_type='application/json')
    
def single_sim(request):
    reply = {}
    if (request.method == 'POST'):
        data = json.loads(request.body.decode())
        model_name = 'muffler'
        freq_tl, model_json_path = sim.single_sim(data, model_name)
        reply['result'] = 'success'
        reply['freq_tl'] = freq_tl
        reply['json-file'] = model_json_path
        return HttpResponse(json.dumps(reply), content_type='application/json')
    else:
        reply['result'] = 'fail'
        return HttpResponse(json.dumps(reply), content_type='application/json')
    
def simulate_model(request):
    reply = {}
    if (request.method == 'POST'):
        data = json.loads(request.body.decode())
        model_name = 'muffler-' + str(time.time())
        url = d_sim.simulate(data, model_name)
        reply['result'] = 'success'
        reply['url'] = url
        return HttpResponse(json.dumps(reply), content_type='application/json')
    else:
        reply['result'] = 'fail'
        return HttpResponse(json.dumps(reply), content_type='application/json')
    

def result(request):
    reply = {}
    model_id = request.GET.get('id', None)
    if model_id == None:
        reply['result'] = 'fail'
        return HttpResponse(json.dumps(reply), content_type='application/json')
    [err, result_path] = d_sim.get_result(model_id)
    if err == 1 or err == 3:
        reply['result'] = 'fail'
        return HttpResponse(json.dumps(reply), content_type='application/json')
    else:
        reply['result'] = 'success'
        if err == 2:
            reply['status'] = 'Waiting'
            return render(request, 'waiting.html')
            # return HttpResponse(json.dumps(reply), content_type='application/json')
        else:
            response = HttpResponse(FileWrapper(open(result_path, 'rb')), content_type='application/zip')
            response['Content-Disposition'] = 'attachment; filename=' + os.path.basename(smart_str(result_path)) + '.zip'
            return response
