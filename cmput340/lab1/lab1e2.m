close all;
clear all;

% example 2.13(3*3)

A=[1 2 2; 4 4 2; 4 6 4];
b=[3;6;10];

%1.Factorization: L U = A
[L,U]=myLU(A);

%fwdSubst to solve for y in: Ly=b
y = fwdSubst(L,b);

%backSubst to solve for x in: Ux = y
x = backSubst(U,y);

disp(x);


% my own example(4*4)
clear all;

A=[7 1 9 2;4 0 1 3;4 10 2 13; 1 2 9 4];
b=[8;2;15;11];

%1.Factorization: L U = A
[L,U]=myLU(A);

%fwdSubst to solve for y in: Ly=b
y = fwdSubst(L,b);

%backSubst to solve for x in: Ux = y
x = backSubst(U,y);

disp(x);