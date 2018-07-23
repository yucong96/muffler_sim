
def vtk_decode(vtk_file_path):
    state = 'start'
    data = {'points':[], 'connections':[], 'values':[]}
    with open(vtk_file_path, 'r') as f:
        for line in f.readlines():
            if 'POINTS' in line:
                state = 'points'
                continue
            if 'POLYGONS' in line:
                state = 'polygons'
                continue
            if state != 'lookup_table' and 'LOOKUP_TABLE' in line:
                state = 'lookup_table'
                continue
            if state == 'lookup_table' and 'LOOKUP_TABLE' in line:
                state = 'end'
                continue
            if state == 'start':
                continue
            if state == 'points':
                position = line.split(' ')
                if len(position) < 4 or position[0].isalpha() == True:
                    continue
                else:
                    del position[-1] # delete \n
                    for i in range(len(position)):
                        position[i] = position[i].strip()
                    for i in range(0, len(position), 3):
                        data['points'].append(position[i:i+3])
            if state == 'polygons':
                connection = line.split(' ')
                if len(connection) < 5 or connection[0].isalpha() == True:
                    continue
                else:
                    del connection[-1] # delete \n
                    del connection[0]
                    for i in range(3):
                        connection[i] = int(connection[i].strip())
                    data['connections'].append(connection)
            if state == 'lookup_table':
                value = line.split(' ')
                if len(value) < 2 or value[0].isalpha() == True:
                    continue
                else:
                    del value[-1] # delete \n
                    for i in range(len(value)):
                        data['values'].append(value[i].strip())
            if state == 'end':
                break
    return data

def json_encode(json_file_path, data):
    points = data['points']
    connections = data['connections']
    values = data['values']
    with open(json_file_path, 'w') as f:
        f.write('{\n')
        f.write('    "metadata": {\n')
        f.write('        "version": 4,\n')
        f.write('        "type": "BufferGeometry"\n')
        f.write('    }, \n')
        f.write('    "type": "BufferGeometry", \n')
        f.write('    "data": {\n')
        f.write('        "attributes": {\n')
        f.write('            "position": {\n')
        f.write('                "itemSize": 3,\n')
        f.write('                "type": "Float32Array",\n')
        f.write('                "array": [\n')
        for conn_index in range(len(connections)):
            connection = connections[conn_index]
            for i in range(3):
                f.write('                    ')
                for j in range(3):
                    if conn_index == len(connections)-1 and i == 2 and j == 2:
                        f.write(str(points[connection[i]][j]))
                    else:
                        f.write(str(points[connection[i]][j]) + ', ')
                f.write('\n')
        f.write('                ]\n')
        f.write('            },\n')
        f.write('            "value": {\n')
        f.write('                "itemSize": 3,\n')
        f.write('                "type": "Float32Array",\n')
        f.write('                "array": [\n')
        for conn_index in range(len(connections)):
            connection = connections[conn_index]
            for i in range(3):
                f.write('                    ')
                if conn_index == len(connections)-1 and i == 2:
                    f.write(str(values[connection[i]]))
                else:
                    f.write(str(values[connection[i]]) + ', ')
                f.write('\n')
        f.write('                ]\n')
        f.write('            }\n')
        f.write('        }\n')
        f.write('    }\n')
        f.write('}\n')
 
def vtk2json(vtk_file_path, json_file_path):
    data = vtk_decode(vtk_file_path)
    json_encode(json_file_path, data)

