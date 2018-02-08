
import numpy as np


from importlib import import_module
from utils import rand_in_range, rand_un




tile = import_module("tiles3")
indices = tile.IHT(4096)

z = 0
w = 0
numTilings = 8
currentState = np.zeros(2)
lastState = np.zeros(2)
lastAction = 0

alpha = 0.1/numTilings

lambdaNumber = 0.9
epsilon = 0
w_size = 1600   #
gamma = 1

#i = 0


def agent_init():
    global z,w,v,currentState,lastState,lastAction

    w = np.full(w_size,np.random.uniform(-0.001,0))    
    z = np.zeros(w_size)  

    lastAction = 0
    currentState = np.zeros(2)
    lastState = np.zeros(2)

    return

def agent_start(state):
    global z,w,v,currentState,lastState,lastAction,numTilings
    #state[1] here is always 0 (which is speed i think)
    #1.7 for 1.2+0.5
    currentState[0] = numTilings*state[0]/(1.7)
    
    currentState[1] = 0 #random to get current location


    Eaction = np.zeros(3)
    for action in range(3):
        t = tile.tiles(indices,numTilings,currentState,[action]) 
        Eaction[action] = sum(w[t])
    lastAction = np.argmax(Eaction)

    lastState = currentState

    return lastAction


def agent_step(reward, state): 
    global z,w,currentState,lastState,lastAction,numTilings




    delta = reward
    #update last state
    # Eligibility traces
    k = tile.tiles(indices,numTilings,lastState,[lastAction])
    for i in k :
        delta = delta - w[i] 
        z[i] = 1 #replacing traces

    currentState[0] = numTilings*state[0]/(1.7)
    currentState[1] = numTilings*state[1]/(0.14)  
    # choose greedy action
    # epsilon = 0, no posibility of taking random action
    Eaction = np.zeros(3)
    for m in range(3):
        t = tile.tiles(indices,numTilings,currentState,[m]) 
        Eaction[m] = sum(w[t])
    action = np.argmax(Eaction)
        
    # refresh delta
    k= tile.tiles(indices,numTilings,currentState,[action])
    for i in k :
        delta = delta + gamma*w[i]



    w = w + alpha*delta*z
    z = z * gamma*lambdaNumber
    lastAction = action
    lastState = currentState

    return action

def agent_end(reward):
    global z,w,lastState,lastAction

    delta = reward
    # Eligibility traces
    k = tile.tiles(indices,numTilings,lastState,[lastAction])
    for i in k :
        delta = delta - w[i] 
        z[i] = 1

    w = w + alpha*delta*z

    return


def agent_message(in_message): # returns string, in_message: string
    global w,v
    """
	Arguments: in_message: string
	returns: The value function as a string.
	This function is complete. You do not need to add code here.
	"""
    # should not need to modify this function. Modify at your own risk
    if (in_message == 'ValueFunction'):
        mp = 1.7/50
        mv = 0.14/50
        v = np.zeros((50,50))
        for i in range(50):
            for j in range(50):
                q = np.zeros(3)
                state = [8*(-1.2+mp*i)/(0.5+1.2),8*(-0.07+mv*j)/(0.07+0.07)]
                for k in range(3):
                    index = np.zeros(w_size)
                    index[tile.tiles(indices,8,state,[k])] = 1 
                    q[k] = np.dot(w,index)

                v[i][j] = (-1)*np.nanmax(q)

        return v
    else:
        return "I don't know what to return!!"
