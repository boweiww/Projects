import numpy as np
from scipy import stats
import re

file = open('data.txt', 'r')

file = file.read()

numbers = re.findall(r"[-+]?\d*\.\d+|\d+", file)

nn = []
for i in range(len(numbers)):
    if i%2 == 1:
        nn.append(float(numbers[i]))

logit = []
sgd=[]
mini=[]
print(nn)
print(len(nn))
for i in range(len(nn)):
    if i<50:
        logit.append(nn[i]/100)
    elif 49 < i <100:
        sgd.append(nn[i]/100)
    else:
        mini.append(nn[i]/100)

print(logit)
print(len(logit))
print(sgd)
print(len(sgd))
print(mini)
print(len(mini))

print(stats.ttest_ind(logit, sgd, equal_var=True))

print(stats.ttest_ind(sgd, mini, equal_var=True))


