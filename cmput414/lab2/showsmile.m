clear all
close all
clc

T1 =     [0.3333    0.3333    0.3333    4.0000;
    0.3333    0.3333    0.3333   -5.0000;
    0.3333    0.3333    0.3333    6.0000;
         0         0         0             1.0000];
     
T2 =     [0.3333    0.3333    0.3333   -0.0000;
    0.6667    0.6667    0.6667   -0.0000;
    1.0000    1.0000    1.0000   -0.0000;
    0.0000    0.0000    0.0000    1.0000];

T3 =     [0.3714   -0.6857    0.2857    0.2857;
    0.6857    0.6571    0.1429    0.1429;
    0.2857   -0.1429    0.1429    0.1429;
    0.2857   -0.1429    0.1429    0.1429];
im = double(imread('./smile.png'));

[row_im column_im] = size(im);
figure(1)
set (gcf,'Color',[1 1 1])
for x = 1:column_im
    for y = 1:row_im
         v = T1*[x y 1 1].';
        if im(y,x) == 255
            plot3(v(1),v(2),v(3),'w.')
            grid on
        else
            plot3(v(1),v(2),v(3),'k.')
        end
        hold on
        drawnow
    end
end
figure(2)
set (gcf,'Color',[1 1 1])

for x = 1:column_im
    for y = 1:row_im
         v = T2*[x y 1 1].';
        if im(y,x) == 255
            plot3(v(1),v(2),v(3),'w.')
            grid on
        else
            plot3(v(1),v(2),v(3),'k.')
        end
        hold on
        drawnow
    end
end
figure(3)
set (gcf,'Color',[1 1 1])

for x = 1:column_im
    for y = 1:row_im
         v = T3*[x y 1 1].';
        if im(y,x) == 255
            plot3(v(1),v(2),v(3),'w.')
            grid on
        else
            plot3(v(1),v(2),v(3),'k.')
        end
        hold on
        drawnow
    end
end
