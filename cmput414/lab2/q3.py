import numpy as np
import re
from sklearn.linear_model import LinearRegression

X = []
with open("A.txt") as f:
    for line in f:
        nums = re.findall(r'[\d.]+', line)
        temp = []
        for num in nums:
            temp.append(float(num))
        X.append(temp)

Y = []
with open("B.txt") as f:
    for line in f:
        nums = re.findall(r'[\d.]+', line)
        temp = []
        for num in nums:
            temp.append(float(num))
        Y.append(temp)
reg = LinearRegression().fit(X, Y)
print(reg.coef_)
print(reg.intercept_)
