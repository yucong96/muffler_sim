import tensorflow as tf
import numpy as np
import time
import sys

INF = 1e10

max_radius = 0.06
min_radius = 0.04
max_length = 0.15
min_length = 0.06
max_freq = 2950
min_freq = 50

def normalize(data_file):
    with open(data_file + '.txt', 'r') as f:
        with open(data_file + '_normalize.txt', 'w') as nf:
            for line in f.readlines():
                parameters = line.split(' ')
                parameters[1] = str((float(parameters[1])-min_radius)/(max_radius-min_radius))
                parameters[6] = str((float(parameters[6])-min_length)/(max_length-min_length))
                parameters[12] = str((float(parameters[12])-min_freq)/(max_freq-min_freq))
                for i in range(len(parameters)-1):
                    nf.write(str(parameters[i]) + ' ')
                nf.write('\n')

def load_data(data_file, normalize_flag):
    if normalize_flag == True:
        data_file = data_file + '_normalize.txt'
    else:
        data_file = data_file + '.txt'
    data = []
    with open(data_file, 'r') as f:
        for line in f.readlines():
            if len(line) < 10:
                break
            parameters = line.split(' ')
            x_data = []
            x_data.append(float(parameters[1]))
            x_data.append(float(parameters[6]))
            x_data.append(float(parameters[12]))
            y_data = np.log10(float(parameters[13]))
            data.append([x_data, y_data])
    return data

def normalize_data(data):
    for index in range(len(data)):
        data[index][0][0] = (data[index][0][0] - min_radius) / (max_radius - min_radius)
        data[index][0][1] = (data[index][0][1] - min_length) / (max_length - min_length)
        data[index][0][2] = (data[index][0][2] - min_freq) / (max_freq - min_freq)
    return data

def get_batch_from_data(data, index=-1):
    if index == -1:
        index = int(np.random.random() * len(data))
    x_data = data[index][0]
    y_data = data[index][1]
    batch = []
    batch.append([x_data])
    batch.append([[y_data]])
    return batch

def log_write(log_file, string):
    with open(log_file, 'a') as f:
        f.write(string + '\n')
        
def nn(mode,
       train_data=None, validate_data=None, nn_log_file=None, nn_model_file=None,
       predict_data=None, model_log_file=None):
    
    if mode == 'train':
        if train_data == None or validate_data == None or nn_log_file == None or nn_model_file == None:
            return 'error'
    if mode == 'predict':
        if predict_data == None or model_log_file == None or nn_model_file == None:
            return 'error'

    input_num = 3
    output_num = 1
    h1_num = 100
    h2_num = 50

    # Set inputs & outputs
    x = tf.placeholder(tf.float32, shape=[None, input_num], name='x')
    y = tf.placeholder(tf.float32, shape=[None, 1], name='y')

    # Set variables
    w_1 = tf.Variable(tf.truncated_normal([input_num, h1_num], stddev=0.1))
    b_1 = tf.Variable(tf.zeros([h1_num]))
    w_2 = tf.Variable(tf.truncated_normal([h1_num, h2_num], stddev=0.1))
    b_2 = tf.Variable(tf.zeros([h2_num]))
    w_3 = tf.Variable(tf.truncated_normal([h2_num, output_num], stddev=0.1))
    b_3 = tf.Variable(tf.zeros([output_num]))

    # Build model
    h1 = tf.nn.relu(tf.matmul(x, w_1) + b_1)
    h2 = tf.nn.relu(tf.matmul(h1, w_2) + b_2)
    y_ = tf.matmul(h2, w_3) + b_3

    # Loss function
    loss = tf.reduce_mean(tf.square(y - y_))

    # Set optimizer
    global_step = tf.Variable(0, trainable=False)
    learning_rate = tf.train.exponential_decay(1e-2, global_step, 1e5, 0.99, staircase=True)
    train = tf.train.GradientDescentOptimizer(learning_rate).minimize(loss, global_step=global_step)

    # Set saver
    saver = tf.train.Saver()

    config = tf.ConfigProto()
    config.gpu_options.allow_growth = True
    with tf.Session(config=config) as sess:

        if mode == 'train':
            sess.run(tf.global_variables_initializer())
            total_loss = 0
            while True:
                batch = get_batch_from_data(train_data)
                _, lossV, learning_rateV, step = sess.run([train, loss, learning_rate, global_step], feed_dict={x:batch[0], y:batch[1]})
                total_loss += lossV
                if step % 1e3 == 0:
                    log_write(nn_log_file, str(step) + ': ' + str(total_loss/1e3) + '\tlearning_rate: ' + str(learning_rateV))
                    total_loss = 0
                if step % 1e4 == 0:
                    # Save model
                    saver.save(sess, nn_model_file, global_step=global_step)
                    # Validate
                    validate_loss = 0
                    for validate_index in range(len(validate_data)):
                        batch = get_batch_from_data(validate_data, validate_index)
                        xV, yV, y_V, lossV = sess.run([x, y, y_, loss], feed_dict={x:batch[0], y:batch[1]})
                        validate_loss += lossV
                    log_write(nn_log_file, 'validate_loss: ' + str(validate_loss/len(validate_data)))

        if mode == 'predict':
            saver.restore(sess, nn_model_file)
            freq_tl = {}
            for index in range(len(predict_data)):
                batch = get_batch_from_data(predict_data, index)
                y_predict = sess.run(y_, feed_dict={x:batch[0], y:batch[1]})
                freq = batch[0][0][2]
                freq = int(freq * (max_freq - min_freq) + min_freq)
                tl = y_predict[0][0]
                freq_tl[str(freq)] = str(tl)
                #with open(model_log_file, 'a') as f:
                #    freq = batch[0][0][2]
                #    freq = int(freq * (max_freq - min_freq) + min_freq)
                #    f.write(str(freq) + ' ' + str(y_predict[0][0]) + '\n')
            return freq_tl



        
def train(train_data_file="tf_simulator/train", validate_data_file="tf_simulator/validate",
          nn_log_file="tf_simulator/log/nn_log.txt", nn_model_file="tf_simulator/nn_model/model"):

    normalize(train_data_file)
    train_data = load_data(train_data_file, normalize_flag=True)
    normalize(validate_data_file)
    validate_data = load_data(validate_data_file, normalize_flag=True)
    with open(nn_log_file, 'w') as f:
        f.write('')
    nn('train', train_data=train_data, validate_data=validate_data, nn_log_file=nn_log_file, nn_model_file=nn_model_file)



def predict(predict_data, model_log_file, nn_model_file="tf_simulator/nn_model/model"):

    predict_data = normalize_data(predict_data)
    print(sys.stderr, nn_model_file)
    freq_tl = nn('predict', predict_data=predict_data, model_log_file=model_log_file, nn_model_file=nn_model_file)
    return freq_tl
