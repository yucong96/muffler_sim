import sys
sys.path.append('..')

from tf_simulator import tf_simulator as tf_sim

chambers = []
chambers.append({'radius':'0.02', 'length':'0.05'})
#chambers.append({'radius':'0.0455', 'length':'0.0907'})
chambers.append({'radius':'0.0432', 'length':'0.1146'})
chambers.append({'radius':'0.02', 'length':'0.05'})
chambers.append({'radius':'0.0', 'length':'0.0'})
chambers.append({'radius':'0.0', 'length':'0.0'})
parameters = {'Freq_start':'50', 'Freq_end':'3000', 'Freq_step':'50', 'Density':'1.29', 'Speed':'340'}
data = {'chambers':chambers, 'parameters':parameters}

tf_sim.tensorflow_predict(data, 'muffler', test_flag=True)

