function [L,U] = myLU(A,k) 
    [~,c]=size(A);
    if ~exist('k')  % If first call no k param given, but k=1
        k=1;
    end
    [M,~]=elimMat(A,k);
    U=M*A;
    if k< c
        [L,U] = myLU(U,k+1);       
    end
    L=A/U;
end