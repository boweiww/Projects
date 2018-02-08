% Vectorize the following
% Note the use of the tic/toc calls to time execution
% Compare the time once you have vectorized it

tic
for i = 1:1000
    t(i) = 2*i;
    y(i) = sin (t(i));
end
toc

tic
t2=[1001:2000];
y2=sin(2*t2);
toc

%clear;
