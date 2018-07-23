
import math

def get_data(model_file, flag):
    max_value = 1
    min_value = -1
    point_data = []
    geometry_data = []
    value_data = []
    lookup_table_data = []
    with open(model_file, 'r') as f:
        state = 'start'
        for line in f.readlines():
            if 'POINTS' in line:
                state = 'points'
                continue
            if 'CELLS' in line:
                state = 'cells'
                continue
            if 'CELL_TYPES' in line:
                state = 'cell_types'
                continue
            if 'LOOKUP_TABLE' in line and state == 'cell_types':
                state = 'point_data'
                continue
            if 'LOOKUP_TABLE' in line and state == 'point_data':
                state = 'lookup_table'
                continue
            if state == 'points':
                if flag == 'REAL':
                    pieces = line.split(' ')
                    point = []
                    for i in range(3):
                        point.append(float(pieces[i]))
                    point_data.append(point)
                continue
            if state == 'cells':
                if flag == 'REAL':
                    pieces = line.split(' ')
                    geometry = []
                    for i in range(1, 5):
                        geometry.append(int(pieces[i]))
                    geometry_data.append(geometry)
                continue
            if state == 'point_data':
                value = float(line.strip())
                value = (max_value-min_value) * value + min_value
                value_data.append(value)
                continue
            if state == 'lookup_table':
                if flag == 'REAL':
                    pieces = line.split(' ')
                    lookup_table = []
                    for i in range(4):
                        lookup_table.append(float(pieces[i]))
                    lookup_table_data.append(lookup_table)
                continue
    return point_data, geometry_data, value_data, lookup_table_data

def compute_amp(real_data, imag_data):
    amplitude_data = []
    for i in range(len(real_data)):
        amplitude_data.append(math.sqrt(real_data[i]*real_data[i] + imag_data[i]*imag_data[i]))
    return amplitude_data

def compute_phase(real_data, imag_data, amplitude_data):
    phase_data = []
    for i in range(len(real_data)):
        phase_data.append(math.acos(real_data[i] / amplitude_data[i]))
        if imag_data[i] < 0:
            phase_data[i] = 2*math.pi - phase_data[i]
    return phase_data

def gen_step(model_result_file, point_data, geometry_data, lookup_table_data, amplitude_data, phase_data, step, index, flag):
    max_value = 1
    min_value = -1
    with open(model_result_file, 'w') as f:
        f.write('# vtk DataFile Version 2.0\n')
        f.write('TET\n')
        f.write('ASCII\n\n')
        f.write('DATASET UNSTRUCTURED_GRID\n')
        f.write('POINTS ' + str(len(point_data)) + ' float\n')
        for i in range(len(point_data)):
            for j in range(3):
                f.write(str(point_data[i][j]) + ' ')
            f.write('\n')
        f.write('CELLS ' + str(len(geometry_data)) + ' ' + str(5*len(geometry_data)) + '\n')
        for i in range(len(geometry_data)):
            f.write('4 ')
            for j in range(4):
                f.write(str(geometry_data[i][j]) + ' ')
            f.write('\n')
        f.write('CELL_TYPES ' + str(len(geometry_data)) + '\n')
        for i in range(len(geometry_data)):
            f.write('10\n')
        f.write('POINT_DATA ' + str(len(point_data)) + '\n')
        f.write('SCALARS point_scalar_data float 1\n')
        f.write('LOOKUP_TABLE data_color\n')
        for i in range(len(point_data)):
            if flag == 'REAL':
                value = amplitude_data[i] * math.cos(phase_data[i] + 2 * math.pi * index / step)
                value = (value - min_value) / (max_value - min_value)
            if flag == 'IMAG':
                value = amplitude_data[i] * math.sin(phase_data[i] + 2 * math.pi * index / step)
                value = (value - min_value) / (max_value - min_value)
            f.write(str(value) + '\n')
        f.write('LOOKUP_TABLE data_color ' + str(len(lookup_table_data)) + '\n')
        for i in range(len(lookup_table_data)):
            for j in range(4):
                f.write(str(lookup_table_data[i][j]) + ' ')
            f.write('\n')

def gen_video(freq, step, model_real_file, model_imag_file, model_name, result_dir):
    point_data, geometry_data, real_data, lookup_table_data = get_data(model_real_file, 'REAL')
    _, _, imag_data, _ = get_data(model_imag_file, 'IMAG')
    amplitude_data = compute_amp(real_data, imag_data)
    phase_data = compute_phase(real_data, imag_data, amplitude_data)
    for index in range(0, step):
        gen_step(result_dir + model_name + '_' + str(freq) + '_real_' + str(index) + '.vtk', point_data, geometry_data, lookup_table_data, amplitude_data, phase_data, step, index, 'REAL')
        gen_step(result_dir + model_name + '_' + str(freq) + '_imag_' + str(index) + '.vtk', point_data, geometry_data, lookup_table_data, amplitude_data, phase_data, step, index, 'IMAG')

def main():
    origin_dir = 'model_video_origin/'
    result_dir = 'model_video_result/'
    model_name = 'muffler_std_2'
    freq = 2000
    gen_video(freq, 50, origin_dir + model_name + '_real_' + str(freq) + '.vtk', origin_dir + model_name + '_imag_' + str(freq) + '.vtk', model_name, result_dir)

main()
