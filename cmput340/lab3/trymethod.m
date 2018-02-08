close all;
clear all;

load immotion_basis

interval=5:10;
largeY=[ones(1,19);0 interval zeros(1,6) interval;0 zeros(1,6) interval interval];
M=size(Y);
disp(M);
for i=1:13
    Ii = renderim(Y(:,i),B,imsize); 
    imshow(Ii,[]);
    drawnow; 
    pause(0.1);
end

F=imread('flower.jpg');
F=double(rgb2gray(F));
[Fx,Fy]=gradient(F);
Bf=[F(:)  Fx(:) Fy(:)];


for i=1:13
    Ii = renderim(Y(:,i),Bf,size(F)); 
    imshow(Ii,[]);
    drawnow; 
    pause(0.1);
end

pause();

interval=5:10;
largeY=[ones(1,19);0 interval zeros(1,6) interval;0 zeros(1,6) interval interval];
disp(Y);
for i=1:13
    Ii = renderim(largeY(:,i),Bf,size(F)); 
    imshow(Ii,[]);
    drawnow; 
    pause(0.1);
end