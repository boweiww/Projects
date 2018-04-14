% cmput325 assignment4
% bowei wang 1462495


/*Question 1

Question 1 (a)
in mid above 3X3, we have:  A 2 
                            3   5
                            8   6
1.at this time, the domain of A is [1,9] at first, look though column, we can decrease domain to {4,5,9}. Again, look through the 3*3 block, the domain of A can be decreased to {4,9}.

in mid above 3X3, we have:  A 2 B
                            3   5
                            8   6
2.at first of this case, the domain of B is [1,9]. Look through column, we can removal the domain to {1,4,7}.

in mid below 3X3, we have:  6   9 
                            2   3
                            C 1  
3.at first of this case, the domain of C is [1,9]. Look through column, we can removal the domain to {4,5,9}. Look through the row, we can removal the domain to {4,9}. Again, look through the 3*3 block, the domain of A can be decreased to 4, so C is 4.

in mid below 3X3, we have:  6   9 
                            2   3
                            4 1  
4. Look back to A through the column we can removal the domain from {4,9} to 9, so A is 9.

in mid mid 3X3, we have:  1   2 
                          D   
                          7   8  
5..at first of this case, the domain of D is [1,9]. Look through column, we can removal the domain to 5, so D is 5. 

in mid above 3X3, we have:  9 2 B
                            3   5
                            8 E 6
6..at first of this case, the domain of E is [1,9]. Look through row, we can removal the domain to {2,3,5,7,9}. Again, look through the 3*3 block of E, the domain of E can be decreased to 7, so E is 7.

in mid above 3X3, we have:  9 2 B
                            3 F 5
                            8 7 6
7..at first of this case, the domain of F is [1,9]. Look through row, we can removal the domain to {2,4,6,7,8}. Again, look through the 3*3 block of F, the domain of F can be decreased to 4, so F is 4. We can also removal the domain of B to 1, so B is 1.

in mid above 3X3, we have:  9 2 1
                            3 4 5
                            8 7 6
result.

(a)part2:

For the left top node of the sodoku in example 2:
we have 1, 7, 2 in the row and 5,7,8,6 in the column, therefore we can remove the domain to {3,4,9}.
look at the 3X3 square, we have 1,3 ,therefore we can remove the domain to {4,9},
But no further removal can be done, we cannot know the specific number by domain.
Same cases happens in all the other nodes, the given node is not enough to removal the domain enough small. 

(b).

  path(X,Y) :- edge(X,Y).  

  path(X,Z) :- edge(X,Y), path(Y,Z). 

  edge(a,b).  edge(b,c).  edge(c,d).   edge(b,e).  edge(e,c).  edge(d,e).  edge(e,f).  edge(f,a).

  T(S) = { head(r) | body(r) is a subset of S, for some ground instance r of a clause in P} 

  head(r) = path(X,Y).
  body(r) = {a,b,c,d,e,f}.
  T(S) = {path(X,Y) | {a,b,c,d,e,f} and X \= Y}.

*/

/*Question 2   War and Peace
Two countries have signed a peace treaty and want to disarm over a period of months, but they still 
don't completely trust each other. Each month one of the countries can choose to dismantle one military division while the other can dismantle two.
Each division has a certain strength, and both sides want to make sure that the total military strength remains equal at each point during the disarmament process. 
For example, suppose the strengths of the country's divisions are:
Country A: 1, 3, 3, 4, 6, 10, 12
Country B: 3, 4, 7, 9, 16
One solution is:
Solution = [[[1,3],[4]], [[3,4],[7]], [[12],[3,9]], [[6,10],[16]]]
Write a predicate
disarm(+Adivisions, +Bdivisions,-Solution)*/

/*idea of this problem is first sort two list, then put them into two cases to handle
either take the first element of list one or take the first element of list two
analyse all the posibility of these two cases.*/

%check if list contains the value

member(A,[A|_]).
member(A,[B|L]) :- A \== B, member(A,L).  

%check if list not contains the value

notmember(_,[]).
notmember(A,[B|L]):- A\==B,notmember(A,L).

nullact([]).

%delete just one element in the list, return new list

delete_one(_, [], []).
delete_one(Ahead, [Ahead|Atail], Atail).
delete_one(S0, [Ahead|Atail], [Ahead|Result]) :-
  delete_one(S0, Atail, Result).

%remove the variable c from the c list in [[a,b],[c]] case

armremov([_|Atail],S1,S2):- armremov(Atail,S1,S2,[]).
armremov([Ahead|_],S1,S2,_):- number(Ahead),delete_one(Ahead,S1,S2).
armremov([Ahead|_],S1,S2,_):- not(number(Ahead)),armremov(Ahead,S1,S2,_).

%remove the variable c from the c list in [[c],[a,b]] case

armremov1([Ahead|_],S1,S2):- armremov1(S1,Ahead,S2,[]).
armremov1(S1,[Ahead|_],S2,_):- number(Ahead),delete_one(Ahead,S1,S2).
armremov1(S1,[Ahead|_],S2,_):- not(number(Ahead)),armremov1(S1,Ahead,S2,_).

%handle the arm in [[a,b],[c]] case, take a, list of a, list of c,
%third and fourth variable is return value which is new list of a and the [[a,b],[c]] if we can find a +b = c

handarm(S0,[Ahead|Atail],S2,Atail,[[S0,Ahead],[S8]]):- S8 is S0 + Ahead,member(S8,S2).

handarm(S0,[Ahead|Atail],S2,S3,S4):- S8 is S0 + Ahead,notmember(S8,S2),handarm(S0,Atail,S2,S6,S4),append([Ahead],S6,S3).
handarm(_,[],_,_,_):- false.

%handle the arm in [[c],[a,b]] case other is same as handarm

handarm1(S0,[Ahead|Atail],S2,Atail,[[S8],[S0,Ahead]]):- S8 is S0 + Ahead,member(S8,S2).
handarm1(S0,[Ahead|Atail],S2,S3,S4):- S8 is S0 + Ahead,notmember(S8,S2),handarm1(S0,Atail,S2,S6,S4),append([Ahead],S6,S3).
handarm1(_,[],_,_,_):- false.


chkarm([[Ahead|_]|Atail],S1):- chkarm(Atail,S2),append(Ahead,S2,S1).
chkarm([],[]).
%disarm will handle the arm into two cases.

disarm([],[],[]):- true.

disarm(Adi, Bdi, Sol):- msort(Adi,S0),msort(Bdi,S1),disarm(S0,S1,Sol,[]),chkarm(Sol,S6),msort(S6,S7),S7 == S0.
disarm([S0head0,S0head1|[]],[S1|[]],S2,_):- S8 is S0head0 + S0head1, S8 == S1, append([[[S0head0,S0head1],[S1]]],[],S2).
disarm([S0|[]],[S1head0,S1head1|[]],S2,_):- S8 is S1head0 + S1head1, S8 == S0, append([[[S0],[S1head0,S1head1]]],[],S2).
disarm([S0head|S0tail],S1,S2,_):- handarm(S0head,S0tail,S1,S4,S5),armremov(S5,S1,NS1),disarm(S4,NS1,S6,_),append([S5],S6,S2).
disarm(S0,[S1head|S1tail],S2,_):- handarm1(S1head,S1tail,S0,S4,S5),armremov1(S5,S0,NS0),disarm(NS0,S4,S6,_),append([S5],S6,S2).


/*Q3  The seating problem is to generate a sitting arrangement for a number of guests, with m tables and n chairs per table. 
Guests who like each other should sit at the same table; guests who dislike each other should not sit at the same table.
To make the issue of symmetry go away, we assume that, by "guests A and B like each other", we mean either A likes B or B likes A; similarly, 
by "guests A and B dislike each other", we mean either A dislikes B or B dislikes A.
The background information is represented by some facts. Given a specific number k, the following represents k tables.
table(1).   table(k). 
The like and dislike relationships
like(A,B).      % A likes B
dislike(A,C). % A dislikes C
Given a listing of variables P representing persons, and a number N representing the number of chairs per table, the top predicate you need to define is:
  seating(P,N) :-  
For a query, e.g., 
 ?- seating([P1,P2,P3,P4,P5,P6],3). 
If solved, each Pi should be bound to a number representing a table. By typing ";", all alternative answers should be generated.
For example, 
 ?- seating([P1,P2,P3,P4,P5,P6],3). 
is a query of whether there is a sitting arrangement for 6 guests and 3 chairs per table. 
Test cases will be provided later.*/


/*idea of this problem is first get all the table and guests, for each guests, find all the persons who like each other,
make a correspond list, then make a list of all the arrange cases by clpfd. Then take the cases out if it violate the dislike or
table seat number limit
*/
/*
t0 :- ['Q3tests/t0'].
t1 :- ['Q3tests/t1'].
t2 :- ['Q3tests/t2'].
t3 :- ['Q3tests/t3'].
t4 :- ['Q3tests/t4'].
t5 :- ['Q3tests/t5'].*/

%clpfd init

:- use_module(library(clpfd)).

% setUnion function from AS3

setUnion(S1,S2,S3):- setUnion(S1,S2,S2,S3).
setUnion([Ahead|Atail],Accum,S2,S3):- notmember(Ahead,Accum),append(Accum,[Ahead],OutList),setUnion(Atail,OutList,S2,S3).
setUnion([Ahead|Atail],Accum,S2,S3):- member(Ahead,Accum),setUnion(Atail,Accum,S2,S3).
setUnion([],Accum,_,Accum).

%count how many numbers is in the list

count([],0).
count([_|Atail], N) :- count(Atail, N1), N is N1 + 1.

%get table and guest

gtable(L):- findall(S0,table(S0),L).
gguest(L):- findall(S0,guest(S0),L).

%find all the like relationships

flike(S0,L):- findall(S1,like(S0,S1),S7),findall(S2,like(S2,S0),S8),append(S7,S8,L).

%This part do the like hand, find all the like group

delhand([Ahead|Atail],S1,S2):- member(Ahead,S1),delete(S1,Ahead,S3),delhand(Atail,S3,S2).
delhand([Ahead|Atail],S1,S2):- notmember(Ahead,S1),delhand(Atail,S1,S2).
delhand([],S1,S1).

handseat(S0,S1):- handseat([],S1,S0).
handseat(S3,[Ahead|Atail],S6):- flike(Ahead,S7),append(S7,[Ahead],S8),hand1seat(S2,S7,S8),delhand(S2,Atail,S4),append([S2],S3,S0),handseat(S0,S4,S6).       %S2 temp S7 all like, S8 all like contains self
handseat(S0,[],S0).

hand1seat(S0,[Ahead|Atail],S2):- flike(Ahead,S7),setUnion(S7,S2,S3),hand1seat(S0,Atail,S3).
hand1seat(S2,[],S2).

gcount(S0,S1):- gcount(S0,S1,[]).
gcount([Ahead|Atail],S1,S2):- count(Ahead,S3),append(S2,[S3],S4),gcount(Atail,S1,S4).
gcount([],S1,S1).

%S0 is the first element of name, S1 is number, S2 is list, S3 is return.

handtable(S0,S1,[Ahead|Atail],S3):- nth0(0,Ahead,S4),S4 == S1,handtable(Ahead,S0,S5),append([S5],Atail,S3).
handtable(S0,S1,[Ahead|Atail],S3):- nth0(0,Ahead,S4),S4 \== S1,handtable(S0,S1,Atail,S5),append([Ahead],S5,S3).

handtable([Ahead|[]],S1,S2):- append(Ahead,S1,S2).
handtable([Ahead|Atail],S1,S2):- handtable(Atail,S1,S4),append([Ahead],[S4],S2).

%S0 is the number of table 

motable(S0,S1):- motable(S0,S1,[]).

%motable(S0,S1,S1):- S0 =< 0.

motable(S0,S1,S2):- S3 is S0 - 1, S3 > 0,append([[S0,[]]],S2,S4),motable(S3,S1,S4).
motable(S0,S1,S2):- S3 is S0 - 1, S3 =< 0,append([[S0,[]]],S2,S1).


%S0 is return value,S1 is total number of seat,S2 is table set,S3 is the set of guest

arrange(S0,S1,S2,S3):- count(S3,S5),count(S2,S6),length(L,S5),L ins 1..S6,label(L),motable(S6,S7),arrange(S0,S1,S3,S7,L).

%S0 is return value,S1 is seat num,S2 is guest, S3 is [A,[]], S4 is L.

arrange(S0,S1,[A2head|A2tail],S3,[A4head|A4tail]):- handtable(A2head,A4head,S3,S5),arrange(S0,S1,A2tail,S5,A4tail).
arrange(S0,_,[],S0,[]).

gsize([Ahead|[]],S0):- count(Ahead,S2),S2 =< S0.
gsize([_|Atail],S0):- gsize(Atail,S0).

%S0 is list,S1 is seat number,S2 is return.
%tablesize check whether the list is under the limit of table size.

tablesize([Ahead|Atail],S1):- gsize(Ahead,S1),tablesize(Atail,S1).
tablesize([],_).

%gdislike check whether the list violate dislike.

gdislike([Ahead|Atail],S0):- findall(S1,dislike(Ahead,S1),S7),findall(S2,dislike(S2,Ahead),S8),append(S7,S8,L),intersection(L,S0,S6),S6 == [],gdislike(Atail,S0).
gdislike([],_).

handdislike([Ahead|[]]):- gdislike(Ahead,Ahead).
handdislike([_|Atail]):- handdislike(Atail).

tdislike([Ahead|Atail]):- handdislike(Ahead),tdislike(Atail).
tdislike([]).


seating(P,S0):- seating(P,S0,[]).
seating(S0,S1,_):- gtable(S3),gguest(S4),handseat(S5,S4),arrange(S0,S1,S3,S5),tablesize(S0,S1),tdislike(S0).   %print(S0),print(S1),print(S3),print(S5).  %   





/*Question 4 - Subset Sum Problem (SSP)
An important problem in cryptography is the so-called Subset Sum Problem. Given a list of integers L (more formally, called a multiset - elements in it may be repeated) and an integer N, 
determine whether there exists a subset S of L, such that the sum of the integers in S equals N. Also see this description. 
In this question, you will be using CLP(FD) to solve the problem.  That is, you state what constraints must be satisfied and let the CLP(FD) solver solve it. 
Test your solution with example queries in this file, and report which ones can be solved within 2 minutes (if not the lab machine, describe your machine). */

%get tests

:- include("tests-SSS.pl").

%algorithm is simple, get all the subset and check the sum.

handsub([], []).
handsub([Ahead|S0tail], [Ahead|S1tail]) :-handsub(S0tail, S1tail).
handsub([_|Atail], S1) :-handsub(Atail, S1).

subsetsum(S0, S1):- handsub(S0, S2),sum_list(S2, S1),comp_statistics,print(S2).


%given to check time.

comp_statistics :-
statistics(runtime,[_,X]),
T is X/1000,
nl,                                         % write to screen
write('run time: '),
write(T), write(' sec.'), nl.

%q1,q2,q3,q4,q5,q6 can be solved in 2mins. q5 and q6 will return false.

/*
Question 5 (4 marks):  Graph Coloring
The problem is: Given a graph in terms of nodes and arcs and a number of colors, determine whether there exists a coloring such that each node is colored with exactly one color 
and any two nodes that are adjacent to each other must be colored with different colors. 
We assume that nodes are named as 0,1,2,..., and arc/2 is a predicate for arcs, colors are given as red, blue, yellow, green, etc. The following is an input instance of the graph coloring problem.  Note that nodes are implicitly given in arcs. 
arc(0,1). arc(0,2). arc(0,3). arc(1,2). arc(1,3). arc(1,4). arc(2,3). arc(2,4). arc(3,4). 
color(red). color(blue). color(yellow). 
This graph coloring problem does not have a solution. 
Later, TA will provide some hints and possibly a sketch of a solution.  */


t0 :- ['graphs/p5'].

t1 :- ['graphs/p8'].

t2 :- ['graphs/p10'].

t3 :- ['graphs/p12'].

t4 :- ['graphs/p13'].

t5 :- ['graphs/p14'].

t6 :- ['graphs/p15'].

t7 :- ['graphs/p17'].

t8 :- ['graphs/p20'].

t9 :- ['graphs/p25'].

t10 :- ['graphs/p30'].

t11 :- ['graphs/p100'].

t12 :- ['graphs/p150'].

t13 :- ['graphs/p150_2'].

t14 :- ['graphs/p300'].


%follows the algorithm given on the slide


c(L) :-findall(S0, vertex(S0), S1),handcolor(S1, L),comp_statistics.

handcolor([], []).
handcolor([Ahead | S0tail], [[Ahead,S1head] | S1tail]) :-handcolor(S0tail, S1tail),color(S1head),forall(member([Node,Color], S1tail),
           ((arc(Node, Ahead) -> Color \= S1head; true),
           ( arc(Ahead, Node) -> Color \= S1head; true))).


