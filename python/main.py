from gameboard import env, dqn
RL = dqn.DeepQNetwork(33, learning_rate=0.01, e_greedy=0.9,
                      replace_target_iter=100, memory_size=2000, e_greedy_increment=0.001)
action_list = [
    -80.0, -75.0, -70.0, -65.0, -60.0, -55.0, -50.0, -45.0, -40.0, -35.0,
    -30.0, -25.0, -20.0, -15.0, -10.0,  -5.0,   0.0,   5.0,  10.0,  15.0,
    20.0,   25.0,  30.0,  35.0,  40.0,  45.0,  50.0,  55.0,  60.0,  65.0,
    70.0,   75.0,  80.0
]
total_steps = 0
min_steps = 1000
num_episodes = 100


for i_episode in range(num_episodes):
    env.start_game()
    old_score = env.current_score
    old_height = env.current_max_height
    state = env.vector()
    ep_r = 0
    while env.current_status == 1:
        action = RL.choose_action(state)
        env.move(action_list[action])

        # Computing reward
        r1 = env.current_score - old_score                      # delta_score
        r2 = 10 if env.current_max_height <= old_height else 0  # height reward
        reward = r1 + r2

        state = env.vector()
        old_score = env.current_score
        old_height = env.current_max_height
        RL.store_transition(state, reward, action)

        ep_r += reward
        if total_steps > min_steps:
            RL.learn()
        total_steps += 1

    print('episode: ', i_episode,
          'ep_r: ', round(ep_r, 2),
          ' epsilon: ', round(RL.epsilon, 2))
