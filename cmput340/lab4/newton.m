
function theta = newton(l,theta,pos,n,def)
    
    for i = 1:n
        J = fdJacob2D(l,theta);
        
        fx = evalRobot2D(l,theta) - pos;
        s = -J\fx;
        theta = theta + s;
        
        if s < def
            break
        end
       
    end
end