
%% Exercise 5
Select = input('Type 1 and press enter for obj_pca, enter 2 for light_pca\n');

if Select == 1
    load obj_pca
end;
if Select ==2
    load light_pca
end;


X = 0:size(Y,2)-1;
X_new = 0:1/2:size(Y,2)-1; 

M=size(Y);
disp(M);
Y_new = zeros(size(Y,1),length(X_new));
for i = 1:size(Y,1)
    Y_new(i,:) = polynomial_interp(X',Y(i,:)',X_new',5);
end;

Y_new_l = zeros(size(Y,1),length(X_new));
for i = 1:size(Y,1)
    Y_new_l(i,:) = interp1(X,Y(i,:),X_new);
end;

figure(1);
plot(X,Y(1:6,:));
figure(2);

plot(X_new,Y_new(1:6,:));
figure(3);

plot(X_new,Y_new_l(1:6,:));

pause;
figure(1);
for c = 1:size(Y,2)
  Ic = renderim(Y(:,c),B,imsize);
  imshow(Ic)
  drawnow
end;

figure(2);
for c = 1:size(Y_new,2)
  Ic = renderim(Y_new(:,c),B,imsize);
  imshow(Ic)
  drawnow
end;

figure(3);
for c = 1:size(Y_new_l,2)
  Ic = renderim(Y_new_l(:,c),B,imsize);
  imshow(Ic)
  drawnow
end;
X_new = 0:1/4:size(Y,2)-1; 

Y_new_l = zeros(size(Y,1),length(X_new));
for i = 1:size(Y,1)
    Y_new_l(i,:) = interp1(X,Y(i,:),X_new);
end;

figure(3);
for c = 1:size(Y_new_l,2)
  Ic = renderim(Y_new_l(:,c),B,imsize);
  imshow(Ic)
  drawnow
end;