function [pos,J]=evalRobot3D(M,theta)
%M1 hip, M2 knee, M3 foot
%position sould be 3X1 matrix
%we can regard function as f(theta 1- 4) =...
pos = M{1}*Rz(theta(3))*Ry(theta(2))*Rx(theta(1))*M{2}*Rx(theta(4))*M{3}*[0;0;0;1];
diff1 = diff(Rx(theta(1)));
diff2 = diff(Ry(theta(2)));
diff3 = diff(Rz(theta(3)));
diff4 = diff(Rx(theta(4)));
J1 = M{1}*Rz(theta(3))*Ry(theta(2))*diff1*M{2}*Rx(theta(4))*M{3}*[0;0;0;1];
J2 = M{1}*Rz(theta(3))*diff2*Rx(theta(1))*M{2}*Rx(theta(4))*M{3}*[0;0;0;1];
J3 = M{1}*diff3*Ry(theta(2))*Rx(theta(1))*M{2}*Rx(theta(4))*M{3}*[0;0;0;1];
J4 = M{1}*Rz(theta(3))*Ry(theta(2))*Rx(theta(1))*M{2}*diff4*M{3}*[0;0;0;1];
J=[J1;J2;J3;J4];
