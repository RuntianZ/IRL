from gameboard import env, dqn
import numpy as np

model_path = './model/model.ckpt'
model_path2 = './model2/model.ckpt'
action_list = [
    -80.0, -75.0, -70.0, -65.0, -60.0, -55.0, -50.0, -45.0, -40.0, -35.0,
    -30.0, -25.0, -20.0, -15.0, -10.0,  -5.0,   0.0,   5.0,  10.0,  15.0,
    20.0,   25.0,  30.0,  35.0,  40.0,  45.0,  50.0,  55.0,  60.0,  65.0,
    70.0,   75.0,  80.0
]


def train():
    total_steps = 0
    min_steps = 100
    num_episodes = 10000
    save_freq = 1000

    for i_episode in range(num_episodes):
        env.start_game()
        old_score = env.current_score
        old_height = env.current_max_height
        img = env.image()
        state = np.stack((img, img, img, img), axis=2)
        ep_r = 0
        while env.current_status == 1:
            action = RL.choose_action(state)
            env.move(action_list[action])

            # Computing reward
            r1 = (env.current_score - old_score) / env.current_num_of_balls   # delta_score
            r2 = 0.0 if env.current_max_height <= old_height else -3.0        # height penalty
            r3 = -8.0 if env.current_status == 2 else 0.0                     # death penalty
            reward = r1 + r2 + r3

            new_img = env.image()
            new_state = np.stack((new_img, new_img, new_img, new_img), axis=2)
            old_score = env.current_score
            old_height = env.current_max_height
            RL.store_transition(state, new_state, reward, action)
            state = new_state

            ep_r += reward
            if total_steps > min_steps:
                RL.learn()
            total_steps += 1

        print('episode: ', i_episode,
              'ep_r: ', round(ep_r, 2),
              ' epsilon: ', round(RL.epsilon, 2),
              ' total_steps: ', total_steps)

        if i_episode % save_freq == save_freq - 1:
            RL.save(model_path)


def play():
    env.display()
    while env.current_status == 1:
        img = env.image()
        s = np.stack((img, img, img, img), axis=2)
        a = RL.greedy(s)
        env.shoot(action_list[a])


if __name__ == '__main__':
    RL = dqn.DeepQNetwork(33, learning_rate=1e-6, e_greedy=0.99, e_greedy_init=0.5,
                          memory_size=20000, e_greedy_increment=1e-5, output_graph=True)
    RL.load(model_path)
    env.start_game()
    play()
