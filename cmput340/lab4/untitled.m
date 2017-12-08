

l = [1,1];

theta = [pi/2,pi/2]';


[~,J] = evalRobot2D(l, theta);
J1 = fdJacob2D(l, theta);

dif = J - J1