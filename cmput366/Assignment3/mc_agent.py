#!/usr/bin/env python

"""
  Author: Adam White, Matthew Schlegel, Mohammad M. Ajallooeian, Sina Ghiassian
  Purpose: Skeleton code for Monte Carlo Exploring Starts Control Agent
           for use on A3 of Reinforcement learning course University of Alberta Fall 2017
 
"""


from utils import rand_in_range, rand_un
import numpy as np
import pickle

epsilon=0.5
def agent_init():
    """
    Hint: Initialize the variables that need to be reset before each run begins
    Returns: nothing
    """
    global Numbers,Returns,last_action,last_state,policy,Q
    
    
    Q = np.zeros((99,99))
    last_state = None
    last_action = None
    Returns = np.zeros((99,99))
    Numbers = np.zeros((99,99))    
    

    policy = np.zeros((99,99))
    for s in range(1,100):
        a = min(s,100-s)
        for i in range(1,a):
            policy[s-1][i-1] = 0.5
        
        
        # under s choose a
        
    #initialize the policy array in a smart way
def getmax(array):
    global policy
    max=np.zeros(2)
    for i in range (1,99):
        if max[0]<array[i]:
            max[1]=i 
    return int(max[1])
def agent_start(state):
    """
    Hint: Initialize the variavbles that you want to reset before starting a new episode
    Arguments: state: numpy array
    Returns: action: integer
    """
    global Numbers,Returns,last_action,last_state,policy
    
    action =  getmax(policy[state[0]-1])+1
    last_action = action 
    last_state = state[0] 
    Numbers[state[0]-1][action-1]+=1    
    # pick the first action, don't forget about exploring starts 
    return action


def agent_step(reward, state): # returns NumPy array, reward: floating point, this_observation: NumPy array
    """
    Arguments: reward: floting point, state: integer
    Returns: action: integer
    """
    # select an action, based on Q
    global Numbers,Returns,last_action,last_state,policy,Q
    
    array=policy[state[0]-1]*Q[state[0]-1]
    action =   getmax(array)+1
    if action == 1 and Q[state[0]-1][action-1] == 0:
        action = rand_in_range(min(state[0],100-state[0]))+1


    Returns[last_state-1][last_action-1] += reward+(Q[state[0]-1][action-1])
    last_action = action 
    last_state = state[0] 
    Numbers[last_state-1][last_action-1]+=1 
    return action
    
    

def agent_end(reward):
    """
    Arguments: reward: floating point
    Returns: Nothing
    """
    global Numbers,Returns,last_action,last_state,policy,Q
    
    # do learning and update pi
    #np.seterr(divide='ignore',invalid='ignore')
    Returns[last_state-1][last_action-1] += reward
    for a in range (0,99):
        for b in range (0,99):
            if Numbers[a][b]!=0:
                Q[a][b]=Returns[a][b]/Numbers[a][b]
    for i in range(1,100):
        best_action = getmax(Q[i-1])+1
        if best_action == 1 and Q[i-1][best_action-1] == 0 :
            best_action =rand_in_range(min(i,100-i))+1
        pi = epsilon/(min(i,100-i))
        for j in range(min(i,100-i)):
            policy[i-1][j] = pi
        #if a = A_best
        policy[i-1][best_action-1] = 1 - epsilon+pi
    return    

    

def agent_cleanup():
    """
    This function is not used
    """
    # clean up
    return

def agent_message(in_message): # returns string, in_message: string
    global Q
    """
    Arguments: in_message: string
    returns: The value function as a string.
    This function is complete. You do not need to add code here.
    """
    # should not need to modify this function. Modify at your own risk
    if (in_message == 'ValueFunction'):
        return pickle.dumps(np.max(Q, axis=1), protocol=0)
    else:
        return "I don't know what to return!!"

