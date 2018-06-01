# from gameboard import env            # Import the library
#                                      # Make sure that zqtytdll.dll is in your workspace
# env.start_game()                     # Start a new game
# while env.current_status == 1:       # current_status == 2 if game over, 1 if not
#     env.move(0.0)                    # shoot the balls at angle = 0.0
#     print(env.current_score)         # Show the score
#     print(env.current_num_of_balls)  # Show number of balls
#     print(env.current_blocks)        # Show the blocks, each item means [type, life, centerx, centery, angle]
#                                      # Type: Triangle(0), Circle(1), Square(2)
# print("Game Over! Your final score is: %d" % env.current_score)
# env.start_game()                     # Start another game

import tensorflow as tf
hello = tf.constant('Hello')
sess = tf.Session()
print(sess.run(hello))
