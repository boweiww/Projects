function [M,L] = elimMat(A,k) 
    [r,~]=size(A);
    a=A(:,k);
    I=eye(r);
    m=a/(a(k));  
    m=[zeros(k,1);m(k+1:end,:)];
    e=I(:,k);
    M=I-m*(e');
    L=I+m*(e');
end