function theta=invKin2D(l,theta0,pos,n,mode) 
def = 0.000001;
if mode == 0    
    theta = newton(l,theta0,pos,n,def);
else
    theta = broyden(l,theta0,pos,n,def);
end
end

