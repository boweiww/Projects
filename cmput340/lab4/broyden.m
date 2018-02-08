
function theta = broyden(l,theta,pos,n,def)
    [~, B] = evalRobot2D(l,theta);
   
    for i = 1:n
        fx = evalRobot2D(l,theta) - pos;
        s = -B\fx;
        thetaNext = theta + s;
        y = evalRobot2D(l,thetaNext) - evalRobot2D(l,theta);
        theta = thetaNext;
        B = B + ((y - B*s)*s')/(s'*s);
        
        if s < def
            break
        end
    end

end