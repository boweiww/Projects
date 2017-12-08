function J=fdJacob2D(l,theta,alpha)


if ~exist('alpha','var')
     % alpha does not exist, so default it to something
      alpha = 0.00000001;
end

J1 = (evalRobot2D(l,theta+[alpha;0])-evalRobot2D(l,theta-[alpha;0]))/(2*alpha);
J2 = (evalRobot2D(l,theta+[0;alpha])-evalRobot2D(l,theta-[0;alpha]))/(2*alpha);
J = [J1 J2];
end