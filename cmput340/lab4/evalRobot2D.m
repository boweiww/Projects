function [pos,J]= evalRobot2D(l,theta)
    
x = l(1)*cos(theta(1)) + l(2)*cos(theta(1)+theta(2));
y = l(1)*sin(theta(1)) + l(2)*sin(theta(1)+theta(2));
pos = [x;y];

%Compute Jacobian base on theta
J11 = -l(1)*sin(theta(1)) - l(2)*sin(theta(1)+theta(2));
J12 = -l(2)*sin(theta(1)+theta(2));
J21 = l(1)*cos(theta(1)) + l(2)*cos(theta(1)+theta(2));
J22 = l(2)*cos(theta(1)+theta(2));
J = [J11 J12; J21 J22];
end