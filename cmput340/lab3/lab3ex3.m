
load immotion_basis.mat

figure;
for i = 1:size(Y,2)
    Ii = renderim(Y(:,i),B,imsize); 
    imshow(Ii,[]); 
    drawnow; 
    pause(0.1);
end;

% Generating new set of coefficient for finer translations
[X,Y] = meshgrid(1:5,-3:0.2:3);  
%[du, dv] = meshgrid(-3:0.2:3,-3:0.2:3);  
Y=zeros(3,93);
c=-3;
for i = 1:93
    Y(1,i)=1;
    if i<32
        Y(2,i)=c;
    end;
    if 32<=i<63
        Y(3,i)=c;
    end
    if i>=63
        Y(3,i)=c;
        Y(2,i)=c;
    end
    c=c+0.2;
    if c>=3.2
        c=-3;
    end
    
end;
    
%Y = [ones(1,length(du(:)));du(:)';dv(:)']; 
figure;
for i = 1:size(Y,2)
    Ii = renderim(Y(:,i),B,imsize); 
    imshow(Ii,[]); 
    drawnow; 
end;

 P = imread('pic.png');
 P = double(rgb2gray(P))/255; % Get Gray image
 imsize = size(P);
 [Ix, Iy] = gradient(P); % Get derivatives
 P = P(:);
 Ix = Ix(:);
 Iy = Iy(:);
 
 for i = 1:13
    du = Y(2,i);
    dv = Y(3,i);

    for t = -3:0.2:3
        imshow(reshape(P + t*[Ix Iy]*[du dv]',imsize))
    end
 end
 close
