function x = backSubst(U,y,k)

  [m,n]=size(U);
  if ~exist('k')  % If first call no k param given, but k=1
    k=m;
  end
  
  x=y(k)/U(k,k);
  if k > 1 % Recursion step
    l = [U(1:k-1,k);zeros(m-k+1,1)];
    x = [backSubst(U,y-x*l,k-1);x];
  end