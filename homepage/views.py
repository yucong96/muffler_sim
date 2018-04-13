from django.shortcuts import render
from django.http import HttpResponse

from simulator import simulator as sim

import json

# Create your views here.
def index(request):
    return render(request, 'index.html')

def upload_model(request):
    reply = {}
    if (request.method == 'POST'):
        data = json.loads(request.body.decode())
        model_name = 'muffler'
        sim.write_model_config(data)
        sim.mesh_gen(model_name)
        output_file = sim.pre_extract_surface(model_name)
        reply['result'] = 'success'
        reply['surf-file'] = output_file
        return HttpResponse(json.dumps(reply), content_type='application/json')
    else:
        reply['result'] = 'fail'
        return HttpResponse(json.dumps(reply), content_type='application/json')
