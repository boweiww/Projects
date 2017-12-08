#Bowei Wang assignment 5
#cmput366
import random
import matplotlib.patches as mpatches
import matplotlib.pyplot as plt
import numpy as np
#initital environment
# obstacles:all points that is obstacle
# target: target point
# start: start point
# gamma: gamma value that will be used in estimate calculation
obstacles = [[3,3],[3,4],[3,5],[6,2],[8,4],[8,5],[8,6]]
target = [9,6]
start=[1,4]
gamma=0.95


#basic flows containing agent
def run(n,m):
    # initial episode
    # episode: list of steps in each episode
    # ep: current episode
    # Q: list that save all state estimate (used in Q-learning)
    # S: current state position
    # failcount: variable to count failtimes
    
    episode=np.zeros(51)
    ep=0
    #initial model
    init_model()
    Q=np.zeros((11,8))
    S=start
    failcount=0
    while ep <50 :
        #if cannot reach target in over 1000 steps, reset the star to run again
        if    episode[ep]>=1000:
            episode[ep]  =0       
            S=start
            failcount+=1
            if failcount>=10:
                #used to check bug if stuck is always happen
                print modellist
                print lmodel[9][5]
                print lmodel[9][4]
                print lmodel[9][3]
                print lmodel[1][4]
        # a: random number to be checked to find random action posibility
        # b: action in list 
        # nS: next state position 
        # reward: reward value
        # action: action, 0 means up, 1 means right, 2 means down, 3 means left
        # temp: temporary variable to save reward+gamma*Q[nS[0]][nS[1]]-Q[S[0]][S[1]]
        
        a=random.uniform(0, 10)
        episode[ep]+=1
        # check if we can just follow the model
        # if in modellist, follow the model
        # if S is not in model list, do Q-learning 
        if S  in modellist:
            # follow model
            action=model_work(S)
            b=action_to_b(action)
            nS=[S[0]+b[0],S[1]+b[1]]

            if nS==target:
                reward =1     
            else:
                reward=0                 
            #Q-learning update value
            temp=reward+gamma*Q[nS[0]][nS[1]]-Q[S[0]][S[1]]
            Q[S[0]][S[1]]=Q[S[0]][S[1]]+alpha*temp                
        elif a <1:
            
                # do random
                # get random action 
                ran=random.uniform(0, 1)
                if ran< 0.25:
                    b=[1,0]
                    action=1
                elif 0.25<ran<0.5:
                    b=[-1,0]
                    action=3
                elif 0.5<ran<0.75:
                    b=[0,1]
                    action=0
                elif 0.75<ran:
                    b=[0,-1]
                    action=2
                nS=[S[0]+b[0],S[1]+b[1]]
                
                if nS==target:
                    reward =1
                 
                else:
                    reward=0
                
                #Q-learning update value
               
                temp=reward+gamma*Q[nS[0]][nS[1]]-Q[S[0]][S[1]]
                Q[S[0]][S[1]]=Q[S[0]][S[1]]+alpha*temp
                 
                   
                
        else:
            # greedy action
            # selection action that have largest Q value
                maxvalue=max(Q[S[0]+1][S[1]],Q[S[0]][S[1]+1],Q[S[0]-1][S[1]],Q[S[0]][S[1]-1]) 
                if maxvalue==Q[S[0]+1][S[1]]:
                    nS=[S[0]+1,S[1]]
                    action=1
                elif maxvalue==Q[S[0]][S[1]+1]:
                    nS=[S[0],S[1]+1]
                    action=0
                elif maxvalue==Q[S[0]-1][S[1]]:
                    nS=[S[0]-1,S[1]]
                    action=3
                elif maxvalue==Q[S[0]][S[1]-1]:
                    nS=[S[0],S[1]-1]
                    action=2
                if nS==[9,6]:
                    reward =1
              
                else:
                    reward=0

                temp=reward+gamma*Q[nS[0]][nS[1]]-Q[S[0]][S[1]]
                Q[S[0]][S[1]]=Q[S[0]][S[1]]+alpha*temp  
                
        #update model in every step
        model(n)
        #check if next state will be blocked by obstacles and edges
        if environment(nS):
            nS=S
        #check if next state is target
        elif nS==target:
            # print run information
            if ep%25==0:
                print "run: "+str(m+1)+" n="+str(n)+"  episode: "+str(ep)  
            # If it is the first time reach target, set model up, model learn begins
            if lmodel[0][0][0]==100:
                modellearn(S,action)
                modellist.append(nS)
                
                lmodel[S[0]][S[1]][1]=1
                lmodel[nS[0]][nS[1]][1]=0
                lmodel[0][0][0]=0
            # Reset S, goe to next episode
            S=start
            Q[nS[0]][nS[1]]+=0.2
            ep+=1              
        # udpdate S
        else:
            S=nS            

    
    return episode

# This function is used to model learn
def model(n):
    # aS: state we are current handeling
    # bS: next state we in our assume
    # dis: distant from this state to target
    for j in range(n) :
        # check if modellist is empty if it is empty directly break
        if modellist!=[]:
            #randomly select a model from the list
            k=random.randint(0,len(modellist)-1)
        else:
            break
        aS=modellist[k]
        dis = lmodel[aS[0]][aS[1]][1]
        dis+=1
        while True:
            # take a random action on this state
            # check if this action is a obstacle action in model, if so random to generate another action
            ran=random.uniform(0, 1)
            if ran< 0.25:
                action=0
                b=[0,1]
            elif 0.25<ran<0.5:
                action=1
                b=[1,0]
            elif 0.5<ran<0.75:
                action=2
                b=[0,-1]
            elif 0.75<ran:
                action=3
                b=[-1,0]  
            if omodel[aS[0]][aS[1]][action]!=0:
                break
        bS=[aS[0]+b[0],aS[1]+b[1]]
        # check if next action will be obstacled 
        # if so, remember this action in model and never take this action again
        if environment(bS):
            bS=aS
            omodel[aS[0]][aS[1]][action]=0   
        # if bS is new to modellist, add it
        if bS not in modellist:
            if action==0:
                action=2
            elif action ==1:
                action=3
            elif action ==2:
                action=0
            elif action ==3:
                action =1
            modellearn(bS,action)
            lmodel[bS[0]][bS[1]][1]=dis
        # if bS has a better way to go, udate it
        elif (bS in modellist and lmodel[bS[0]][bS[1]][1] > dis):
            lmodel[bS[0]][bS[1]][0] = action
            lmodel[bS[0]][bS[1]][1] = dis
    
    
# simple function to change action to b
def action_to_b(action):
    if action ==0:
        b=[0,1]
    if action ==1:
        b=[1,0]
    if action ==2:
        b=[0,-1]
    if action ==3:
        b=[-1,0]
    return b
# set best action in state
def modellearn(S,action):
    
    lmodel[S[0]][S[1]][0] =action
    
    modellist.append(S)
# initial model 
# omodel: save all the obstacle that already know 
# modellist: save all states that in model
# lmodel: save best action and distant to cooresponding state
def init_model():
    global modellist,lmodel,omodel
    omodel=np.ones((11,8,4))
    modellist=[]
    lmodel=np.empty(( 11,8,2))  
    lmodel.fill(100)
    
    omodel=np.ones((11,8,4))
    #0=up 1=right 2=down 3=left   
    return
# environment check whether next action will be blocked
def environment(nS):
    blocked=False
    for i in range(7):
        if obstacles[i]==nS:
            blocked=True
    if (nS[0]<1 or nS[0] > 9):
        blocked=True
    elif (nS[1]<1 or nS[1] > 6):
        blocked=True
    return blocked
        
# find the best action in model
def model_work(S):
    if lmodel[S[0]+1][S[1]][1] <lmodel[S[0]][S[1]][1]:
        action=1
    elif lmodel[S[0]-1][S[1]][1] <lmodel[S[0]][S[1]][1]:
        action=3
    elif lmodel[S[0]][S[1]+1][1] <lmodel[S[0]][S[1]][1]:
        action=0
    elif lmodel[S[0]][S[1]-1][1] <lmodel[S[0]][S[1]][1]:
        action=2
    
    try:
        return action
    except:
        print S
        
        print lmodel[S[0]][S[1]+1][1]
        
    
            
    
    
def main ():
    global alpha
    alpha=0.1
    
    i=0
    e1=[[0]]*10
    e2=[[0]]*10
    e3=[[0]]*10
    #e1[0]=run(0)
    #e2[0]=run(5)
    #e3[0]=run(50)    
    #plt.plot(e1[0],'r',e2[0],'b',e3[0],'g')
    #plt.ylabel('steps per episode')
    #plt.show()     
    
    for i in range (10):
            e1[i]=run(0,i)
            e2[i]=run(5,i)
            e3[i]=run(50,i)
        
            print str(i+1) +" run is finished"
    r1=np.zeros(50)
    r2=np.zeros(50)
    r3=np.zeros(50)
    for i in range(10):
        for j in range(50):
            r1[j]+=e1[i][j]
            r2[j]+=e2[i][j]
            r3[j]+=e3[i][j]
    for i in range(50):
        r1[i]=r1[i]/10
        r2[i]=r2[i]/10
        r3[i]=r3[i]/10
    
    
    plt.figure(0)

    plt.plot(r1, label="n=0")
    plt.plot(r2, label="n=5")
    plt.plot(r3, label="n=50")
    plt.legend()
    
    
    plt.ylabel('steps per episode')
    plt.show()  
def part2():
    global alpha
    
    x=[0.03125,0.0625,0.125,0.25,0.5,1]
    e2=[[0]]*10
    r=np.zeros(6)
    for i in range(len(x)):
        print "This is i: "+str(i)
        alpha=x[i]
        for j in range (10):
            e2[j]=run(5,j)        
            
        for k in range(10):
            for l in range(50):
                r[i]+=e2[k][l]
        r[i]=r[i]/500
    plt.figure(1)
    
    plt.plot(x, r)
    plt.show()
    
main()
part2()
