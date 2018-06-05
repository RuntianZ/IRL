import numpy as np
import tensorflow as tf

np.random.seed(1)
tf.set_random_seed(1)


class DeepQNetwork:
    def __init__(
        self,
        n_actions,
        learning_rate=0.9,
        reward_decay=0.9,
        e_greedy=0.9,
        replace_target_iter=300,
        memory_size=500,
        batch_size=32,
        e_greedy_increment=None
    ):
        self.n_actions = n_actions
        self.lr = learning_rate
        self.gamma = reward_decay
        self.epsilon_max = e_greedy
        self.replace_target_iter = replace_target_iter
        self.memory_size = memory_size
        self.batch_size = batch_size
        self.epsilon_increment = e_greedy_increment
        self.epsilon = 0 if e_greedy_increment is not None else e_greedy

        # initial some other things
        self.learn_step_counter = 0
        self.state_size = 81
        self.memory_counter = 0
        self.memory_index = 0

        # double check how to set memory
        self.memory = np.zeros((self.memory_size, self.state_size + 2))

        self._build_net()

        t_params = tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES, scope='target_net')

        e_params = tf.get_collection(tf.GraphKeys.GLOBAL_VARIABLES, scope='eval_net')

        with tf.variable_scope('soft_replacement'):
            self.target_replace_op = [tf.assign(t, e) for t, e in zip(t_params, e_params)]

        self.sess = tf.Session()

        self.sess.run(tf.global_variables_initializer())
        self.cost_his = []

    def _build_net(self):
        # ---------inputs-------------
        self.state = tf.placeholder(tf.int32, [None, self.state_size], name='state')

        w_initializer = tf.random_normal_initializer(0., 0.5)
        b_initializer = tf.constant_initializer(0.1)

        # ---------eval_net_structure------------
        with tf.variable_scope('eval_net'):
            pass
        # ---------target_net_structure----------
        with tf.variable_scope('target_net'):
            pass
        with tf.variable_scope('q_target'):
            pass
        with tf.variable_scope('q_eval'):
            pass
        with tf.variable_scope('loss'):
            self.loss = tf.losses.mean_squared_error(self.q_target, self.q_eval0)
        with tf.variable_scope('train'):
            self.train0 = tf.train.AdamOptimizer(self.lr).minimize(self.loss)

    def store_transition(self, state, reward, action):
        if self.memory_counter < self.memory_size:
            self.memory[self.memory_counter, :] = np.append(state, reward, action)
            self.memory_counter += 1
        else:
            self.memory[self.memory_index, :] = np.append(state, reward, action)
            self.memory_index = (self.memory_index + 1) % self.memory_size

    def choose_action(self, st):
        if np.random.uniform() < self.epsilon:
            val = self.sess.run(self.q_eval, feed_dict={self.state: st})
            action = np.argmax(val)
        else:
            action = np.random.randint(0, self.n_actions)
        return action

    def learn(self):
        if self.learn_step_counter % self.replace_target_iter == 0:
            self.sess.run(self.target_replace_op)
            print('\ntarget_param_replaced\n')

        if self.memory_counter > self.memory_size:
            ind = np.random.choice(self.memory_size, size=self.batch_size)
        else:
            ind = np.random.choice(self.memory_counter, size=self.batch_size)

        # ------------get memory------run loss func-----------
        batch_memory = self.memory[ind, :]

        _, cost = self.sess.run(
            [self.train0, self.loss],
            feed_dict={
                self.state: batch_memory[:],
            })
        # ----------------------------------------------------

        self.epsilon = self.epsilon + self.epsilon_increment if self.epsilon < self.epsilon_max else self.epsilon_max
        self.learn_step_counter += 1
