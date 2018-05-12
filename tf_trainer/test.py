import tensorflow as tf
import numpy as np

#def load_data(statistic_file):
#    data = []
#    with open(statistic_file, 'r') as f:
#        for line in f.readlines():
#            parameters = line.split(' ')
#            x_data = []
#            for i in range(5):
#                x_data.append((float(parameters[i])-0.01)*(1/0.06)) # radius [0.02, 0.08]
#            for i in range(5, 10):
#                x_data.append((float(parameters[i])-0.015)*(1/0.12)) # length [0.03, 0.15]
#            x_data.append((float(parameters[i])-200)*(1/800)) # speed [200, 1000]
#            x_data.append((float(parameters[i])-0.5)*(1/3.5)) # rho [0.5, 4]
#            x_data.append((float(parameters[i])-50)*(1/2950)) # freq [50, 3000]
#            y_data = np.log10(float(parameters[len(parameters)-2]))
#            if x_data[12] > 2500.0 / 3000:
#                continue
#            data.append([x_data, y_data])
#    return data
#
#def get_batch_from_data(data):
#    index = int(np.random.random() * len(data))
#    x_data = data[index][0]
#    y_data = data[index][1]
#    batch = []
#    batch.append([x_data])
#    batch.append([[y_data]])
#    return batch
    
def get_batch():
    x_data = np.array(np.float32(np.random.rand(1, 2)))
    y_data = np.float32(0)
    #x_data[0][2] = x_data[0][0] * x_data[0][1]
    y_data = x_data[0][0] * x_data[0][1] * 10
    batch = []
    batch.append(x_data)
    batch.append([[y_data]])
    return batch

# r_0, l_0, ... , r_4, l_4, freq, rho, speed
input_num = 2
output_num = 1
hidden_num_1 = 10
hidden_num_2 = 10
batch_num = 20000

# Load data
#data = load_data('statistic.txt')

# Set inputs & outputs
x = tf.placeholder(tf.float32, shape=[1, input_num], name='x')
y = tf.placeholder(tf.float32, shape=[1, 1], name='y')

# Set variables
w_1 = tf.Variable(tf.zeros([input_num, hidden_num_1]))
b_1 = tf.Variable(tf.zeros([hidden_num_1]))
#w_2 = tf.Variable(tf.zeros([hidden_num_1, hidden_num_2]))
#b_2 = tf.Variable(tf.zeros([hidden_num_2]))
w_3 = tf.Variable(tf.zeros([hidden_num_2, output_num]))
b_3 = tf.Variable(tf.zeros([output_num]))

# Build model
hidden_layer_1 = tf.nn.sigmoid(tf.matmul(x, w_1) + b_1)
#hidden_layer_2 = tf.nn.sigmoid(tf.matmul(hidden_layer_1, w_2) + b_2)
y_ = tf.matmul(hidden_layer_1, w_3) + b_3

# Loss function
loss = tf.reduce_mean(tf.square(y - y_))

# Set optimizer
train = tf.train.GradientDescentOptimizer(0.1).minimize(loss)

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    total_loss = 0
    for i in range(batch_num):
        batch = get_batch()
        _, loss_value = sess.run([train, loss], feed_dict={x:batch[0], y:batch[1]})
        total_loss += loss_value
        if i%100 == 0:
            print(1.0*total_loss/100)
            total_loss = 0



