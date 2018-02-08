
from rl_glue import *  # Required for RL-Glue
RLGlue("mountaincar", "sarsa_lambda_agent")

import numpy as np

if __name__ == "__main__":
    num_episodes = 1000
    num_runs = 1

    value = np.zeros((50,50,3))

    for r in range(num_runs):
        RL_init()
        for e in range(num_episodes):
            print "episode number : ", e
            
            # print '\tepisode {}'.format(e+1)
            RL_episode(0)
    value = RL_agent_message("ValueFunction")



    np.save('value',value)