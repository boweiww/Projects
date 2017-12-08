%% Exercise 1

load EllipsePoints.mat

% Estimating translation offsets for each cluster
t1 = mean(Y1,2);
t2 = mean(Y2,2);
t3 = mean(Y3,2);

mY1 = Y1 - t1*ones(1,size(Y1,2));
mY2 = Y2 - t2*ones(1,size(Y2,2));
mY3 = Y3 - t3*ones(1,size(Y3,2));

% Computing covariances
C1 = mY1 * mY1' / size(Y1,2);
C2 = mY2 * mY2' / size(Y2,2);
C3 = mY3 * mY3' / size(Y3,2);
disp(C1);


[X1,D1] = eig(C1);
[X2,D2] = eig(C2);
[X3,D3] = eig(C3);

figure;
plot([Y1(1,:),Y2(1,:),Y3(1,:)]',[Y1(2,:),Y2(2,:),Y3(2,:)]','b.');
axis equal

hold on

plot(t1(1)+2*sqrt(D1(1,1))*[0 X1(1,1)],t1(2)+2*sqrt(D1(1,1))*[0 X1(2,1)],'g');
plot(t1(1)+2*sqrt(D1(2,2))*[0 X1(1,2)],t1(2)+2*sqrt(D1(2,2))*[0 X1(2,2)],'r');
plot(t2(1)+2*sqrt(D2(1,1))*[0 X2(1,1)],t2(2)+2*sqrt(D2(1,1))*[0 X2(2,1)],'g');
plot(t2(1)+2*sqrt(D2(2,2))*[0 X2(1,2)],t2(2)+2*sqrt(D2(2,2))*[0 X2(2,2)],'r');
plot(t3(1)+2*sqrt(D3(1,1))*[0 X3(1,1)],t3(2)+2*sqrt(D3(1,1))*[0 X3(2,1)],'g');
plot(t3(1)+2*sqrt(D3(2,2))*[0 X3(1,2)],t3(2)+2*sqrt(D3(2,2))*[0 X3(2,2)],'r');

hold off

