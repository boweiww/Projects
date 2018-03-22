import matplotlib.pyplot as plt
import numpy as np
import random


def getoptimal(rates):
     p= 0.25
     best = [0] * 100
     count=0
     vs=0
     plotV=np.zeros((99,6))
     
     pmax=0
     V= [0] * 101
     while True:
          delta=0 
          
          emax=0
          for money in range(1,100):
               m=money
     
               maxValue = 0
               best_action = 0
               v=V[m]
               for action in range(1,min(m,100-m)+1): #0-min(s,100-s)
                    if m+action >=100:
                         vs = rates*(1+ V[action+m])+(1- rates)*V[m-action]
                    else:
                         vs = rates*(0+ V[action+m])+(1- rates)*V[m-action]
                    if vs>maxValue:
                         maxValue = vs
                         best_action = action
               if maxValue < V[m]:
                    maxValue = V[m]
                    action =0
               best[m] = best_action
               V[m]=maxValue
     
               delta = max(abs(V[m]-v),delta)
               if count<6:
                    plotV[m-1][count] = V[m]
          
          print(count)
          if delta < 0.0000000000000000000000000000000000001:
               break
          count+=1
     for a in range(0,99):
          plotV[a][5] = V[a]     
     
     #plt.plot(best)
     
     plt.figure(1)                # the first figure
          
     plt.plot(plotV)
     
     #plt.plot(plotV)
     plt.ylabel('Value estimates')
     plt.figure(2)
     plt.plot(best)
     
     plt.show()         
     return   
                    
def main():
     getoptimal(0.25)
     getoptimal(0.55)
     
main()