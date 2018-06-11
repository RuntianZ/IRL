from gameboard import env, dqn
import numpy as np

model_path = './model81/model.ckpt'
model_path2 = './model2/model.ckpt'
action_list = []
for i in range(-80, 81, 2):
    action_list.append(float(i))


def train():
    total_steps = 0
    min_steps = 1000
    num_episodes = 20000
    save_freq = 300

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
    env.start_game()
    env.display()
    while env.current_status == 1:
        img = env.image(scale=0.8)
        s = np.stack((img, img, img, img), axis=2)
        a = RL.greedy(s)
        env.shoot(action_list[a])


def trial(scale, n_trials=100):
    ans = 0.0
    for i_trial in range(n_trials):
        env.start_game()
        while env.current_status == 1:
            img = env.image(scale=scale)
            s = np.stack((img, img, img, img), axis=2)
            a = RL.greedy(s)
            env.move(action_list[a])
            print(env.current_score)
        ans += env.current_score
    return ans / n_trials


if __name__ == '__main__':
    RL = dqn.DeepQNetwork(81, learning_rate=1e-6, e_greedy=0.99, e_greedy_init=0.5,
                          memory_size=20000, e_greedy_increment=3e-6, output_graph=True)
    RL.load(model_path)
    play()

