import numpy as np
import tensorflow as tf
from collections import deque
import random
from . import env

np.random.seed(1)
tf.set_random_seed(1)


def weight_variable(shape, name):
    initial = tf.truncated_normal(shape, stddev=0.01)
    return tf.Variable(initial, name=name)


def bias_variable(shape, name):
    initial = tf.constant(0.01, shape=shape)
    return tf.Variable(initial, name=name)


def conv2d(x, W, stride):
    return tf.nn.conv2d(x, W, strides=[1, stride, stride, 1], padding="SAME")


def max_pool_2x2(x, name):
    return tf.nn.max_pool(x, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding="SAME", name=name)


class DeepQNetwork:
    def __init__(
        self,
        n_actions,
        learning_rate=0.9,
        reward_decay=0.9,
        e_greedy=0.9,
        memory_size=500,
        batch_size=32,
        e_greedy_increment=None,
        e_greedy_init=0.0,
        output_graph=False,
    ):
        self.n_actions = n_actions
        self.lr = learning_rate
        self.gamma = reward_decay
        self.epsilon_max = e_greedy
        self.memory_size = memory_size
        self.batch_size = batch_size
        self.epsilon_increment = e_greedy_increment
        self.epsilon = e_greedy_init

        # Memory arrangement: (s, s_, r, a)
        self.memory = deque()

        self._build_net()

        self.sess = tf.Session()
        if output_graph:
            tf.summary.FileWriter("logs/", self.sess.graph)

        self.sess.run(tf.global_variables_initializer())
        self.cost_his = []
        self.saver = tf.train.Saver()

    def _build_net(self):
        # ---------inputs-------------
        self.s = tf.placeholder(tf.float32, [None, env.max_x, env.max_y, 4], name='s')
        self.r = tf.placeholder(tf.float32, [None], name='r')
        self.a = tf.placeholder(tf.float32, [None, self.n_actions], name='a')

        # ------------------ build evaluate_net ------------------
        with tf.variable_scope('eval_net'):
            W_conv1 = weight_variable([5, 5, 4, 32], "W_conv1")
            b_conv1 = bias_variable([32], "b_conv1")
            h_conv1 = tf.nn.relu(conv2d(self.s, W_conv1, 2) + b_conv1, name="conv1")
            h_pool1 = max_pool_2x2(h_conv1, "pool1")

            W_conv2 = weight_variable([3, 3, 32, 64], "W_conv2")
            b_conv2 = bias_variable([64], "b_conv2")
            h_conv2 = tf.nn.relu(conv2d(h_pool1, W_conv2, 2) + b_conv2, name="conv2")
            h_pool2 = max_pool_2x2(h_conv2, "pool2")
            h_flat3 = tf.reshape(h_pool2, [-1, 2560])
            #
            # W_conv3 = weight_variable([3, 3, 64, 64], "W_conv3")
            # b_conv3 = bias_variable([64], "b_conv3")
            # h_conv3 = tf.nn.relu(conv2d(h_pool2, W_conv3, 1) + b_conv3, name="conv3")
            # h_flat3 = tf.reshape(h_conv3, [-1, 2560])

            W_fc1 = weight_variable([2560, 512], "W_fc1")
            b_fc1 = bias_variable([512], "b_fc1")
            h_fc1 = tf.nn.relu(tf.matmul(h_flat3, W_fc1) + b_fc1, name="fc1")

            W_fc2 = weight_variable([512, self.n_actions], "W_fc2")
            b_fc2 = bias_variable([self.n_actions], "b_fc2")
            self.q_eval = tf.matmul(h_fc1, W_fc2) + b_fc2

        with tf.variable_scope('loss'):
            self.q0 = tf.reduce_sum(tf.multiply(self.q_eval, self.a), reduction_indices=1)
            self.loss = tf.losses.mean_squared_error(self.q0, self.r)
        with tf.variable_scope('train'):
            self.train0 = tf.train.AdamOptimizer(self.lr).minimize(self.loss)

    def store_transition(self, state, state_, reward, action):
        self.memory.append((state, state_, reward, action))
        if len(self.memory) > self.memory_size:
            self.memory.popleft()

    def choose_action(self, st):
        # print(st)
        val = self.sess.run(self.q_eval, feed_dict={self.s: np.reshape(st, newshape=(1, env.max_x, env.max_y, 4))})
        # print(val)
        if np.random.uniform() < self.epsilon:
            action = np.argmax(val)
        else:
            action = np.random.randint(0, self.n_actions)
        return action

    def greedy(self, st):
        # print(st)
        val = self.sess.run(self.q_eval, feed_dict={self.s: np.reshape(st, newshape=(1, env.max_x, env.max_y, 4))})
        action = np.argmax(val)
        # print(val)
        return action

    def learn(self):
        # Experience Replay
        batch_memory = random.sample(self.memory, self.batch_size)
        batch_s = [d[0] for d in batch_memory]
        batch_s_ = [d[1] for d in batch_memory]
        batch_r = [d[2] for d in batch_memory]
        batch_a = [d[3] for d in batch_memory]
        batch_target = []
        batch_action = []
        s_eval = self.sess.run(self.q_eval,
                               feed_dict={self.s: batch_s_})

        # Compute q-target
        for i in range(0, len(batch_memory)):
            batch_target.append(batch_r[i] + self.gamma * np.max(s_eval[i]))
            a_ = np.zeros(self.n_actions)
            a_[batch_a[i]] = 1
            batch_action.append(a_)

        _, cost = self.sess.run(
            [self.train0, self.loss],
            feed_dict={
                self.s: batch_s,
                self.r: batch_target,
                self.a: batch_action,
            })
        # ----------------------------------------------------
        self.cost_his.append(cost)
        self.epsilon = self.epsilon + self.epsilon_increment if self.epsilon < self.epsilon_max else self.epsilon_max

    def save(self, path):
        try:
            save_path = self.saver.save(self.sess, path)
            print("Model saved in path: %s" % save_path)
        except IOError:
            print("IOError: Model is not saved.")

    def load(self, path):
        self.saver.restore(self.sess, path)
        print("Successfully loaded.")

    def plot_cost(self):
        import matplotlib.pyplot as plt
        plt.plot(np.arange(len(self.cost_his)), self.cost_his)
        plt.ylabel('Cost')
        plt.xlabel('training steps')
        plt.show()
