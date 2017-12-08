
%% Exercise 2

load sincos_basis.mat
figure (1);
for i = 1:size(Y,2)
    Ii = renderim(Y(:,i),B,imsize); 
    imshow(Ii,[]); 
    drawnow; 
    pause(0.1);
end;


M=size(B);
disp(M);


% Generating a new basis which has twice the frequency of the previous
% basis
B1=zeros(M(1),2);
for i =1:M(1)
    B1(i,2)=2*B(i,1)*B(i,2);
    B1(i,1)=B(i,1).^2-B(i,2).^2;
    
end;


figure (2);
for i = 1:size(Y,2)
    Ii = renderim(Y(:,i),B1,imsize); 
    imshow(Ii,[]); 
    drawnow; 
    pause(0.1);
end;
