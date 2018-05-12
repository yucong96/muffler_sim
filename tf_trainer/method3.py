import tensorflow as tf
import numpy as np
import time

INF = 1e10
parameter_num = 13
train_file = 'train'
test_file = 'test'

def normalize(data_file):
    max_parameters = []
    min_parameters = []
    for i in range(parameter_num):
        max_parameters.append(-INF)
        min_parameters.append(INF)
    max_tl = -INF
    min_tl = INF
    with open(data_file + '.txt', 'r') as f:
        for line in f.readlines():
            parameters = line.split(' ')
            for i in range(parameter_num):
                parameters[i] = float(parameters[i])
                if parameters[i] > max_parameters[i]:
                    max_parameters[i] = parameters[i]
                if parameters[i] < min_parameters[i]:
                    min_parameters[i] = parameters[i]
            parameters[parameter_num] = np.log10(float(parameters[parameter_num]))
            if parameters[parameter_num] > max_tl:
                max_tl = parameters[parameter_num]
            if parameters[parameter_num] < min_tl:
                min_tl = parameters[parameter_num]
        f.seek(0)
        with open(data_file + '_normalize.txt', 'w') as nf:
            for line in f.readlines():
                parameters = line.split(' ')
                for i in range(parameter_num):
                    parameters[i] = float(parameters[i])
                    parameters[i] = (parameters[i] - min_parameters[i]) / (max_parameters[i] - min_parameters[i])
                    nf.write(str(parameters[i]) + ' ')
                parameters[parameter_num] = float(parameters[parameter_num])
                parameters[parameter_num] = (parameters[parameter_num] - min_tl) / (max_tl - min_tl)
                nf.write(str(parameters[parameter_num]) + '\n')

def load_data(data_file):
    data = []
    with open(data_file + '_normalize.txt', 'r') as f:
        for line in f.readlines():
            parameters = line.split(' ')
            x_data = []
            for i in range(parameter_num):
                x_data.append(float(parameters[i]))
            y_data = np.log10(float(parameters[parameter_num]))
            data.append([x_data, y_data])
    return data

def get_batch_from_data(data):
    index = int(np.random.random() * len(data))
    x_data = data[index][0]
    y_data = data[index][1]
    batch = []
    batch.append([x_data])
    batch.append([[y_data]])
    return batch
    
def get_batch():
    x_data = np.array(np.float32(np.random.rand(1, 3)))
    x_data[0][2] = x_data[0][0] * x_data[0][1]
    y_data = x_data[0][0] * x_data[0][1]
    batch = []
    batch.append(x_data)
    batch.append([[y_data]])
    return batch

# r_0, l_0, ... , r_4, l_4, freq, rho, speed
input_num = parameter_num
output_num = 1
hidden_num_1 = 200
hidden_num_2 = 200
batch_num = 20000

# Load data
normalize(train_file)
normalize(test_file)
data_train = load_data(train_file)
data_test = load_data(test_file)

# Set inputs & outputs
x = tf.placeholder(tf.float32, shape=[None, input_num], name='x')
y = tf.placeholder(tf.float32, shape=[None, 1], name='y')

# Set variables & Build model
with tf.name_scope('hidden_layer_1'):
    w_1 = tf.Variable(tf.truncated_normal([input_num, hidden_num_1], stddev=0.1))
    b_1 = tf.Variable(tf.zeros([hidden_num_1]))
    h_1 = tf.nn.relu(tf.matmul(x, w_1) + b_1)
with tf.name_scope('hidden_layer_2'):
    w_2 = tf.Variable(tf.truncated_normal([hidden_num_1, hidden_num_2], stddev=0.1))
    b_2 = tf.Variable(tf.zeros([hidden_num_2]))
    h_2 = tf.nn.relu(tf.matmul(h_1, w_2) + b_2)
with tf.name_scope('output_layer'):
    w_3 = tf.Variable(tf.truncated_normal([hidden_num_2, output_num], stddev=0.1))
    b_3 = tf.Variable(tf.zeros([output_num]))
    y_ = tf.matmul(h_2, w_3) + b_3

# Loss function
regular_weight = 1e-5
tf.add_to_collection('variable_reg', tf.contrib.layers.l2_regularizer(regular_weight)(w_1))
tf.add_to_collection('variable_reg', tf.contrib.layers.l2_regularizer(regular_weight)(b_1))
tf.add_to_collection('variable_reg', tf.contrib.layers.l2_regularizer(regular_weight)(w_2))
tf.add_to_collection('variable_reg', tf.contrib.layers.l2_regularizer(regular_weight)(b_2))
tf.add_to_collection('variable_reg', tf.contrib.layers.l2_regularizer(regular_weight)(w_3))
tf.add_to_collection('variable_reg', tf.contrib.layers.l2_regularizer(regular_weight)(b_3))
loss = tf.reduce_mean(tf.square(y - y_)) + tf.reduce_sum(tf.get_collection('variable_reg'))
# + 1e-5 * tf.reduce_sum(tf.square(tf.get_collection('variable')))

# Set optimizer
train = tf.train.GradientDescentOptimizer(0.001).minimize(loss)

# Set saver
saver = tf.train.Saver(max_to_keep=20)

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    total_loss = 0
    i = 0
    while True:
        i = i + 1
        batch = get_batch_from_data(data_train)
        _, loss_value = sess.run([train, loss], feed_dict={x:batch[0], y:batch[1]})
        total_loss += loss_value
        if i % 200 == 0:
            with open('log_method3.txt', 'a') as log_f:
                log_f.write(str(i) + ': ' + str(1.0*total_loss/200) + '\n')
            total_loss = 0
        if i % 100000 == 0:
            saver.save(sess, 'model_method3/' + str(int(time.time())) + '/model.ckpt')
            # Validation
            validation_loss = 0
            for j in range(100):
                batch = get_batch_from_data(data_test)
                loss_value = sess.run(loss, feed_dict={x:batch[0], y:batch[1]})
                validation_loss += loss_value
            with open('log_method3.txt', 'a') as log_f:
                log_f.write('validation loss: ' + str(1.0*validation_loss/100) + '\n')

