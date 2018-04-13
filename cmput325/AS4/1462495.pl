% cmput325 assignment4
% bowei wang 1462495

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
nullact([]).

handarm(S0,S1,[Ahead|_],S3):- S4 is S0 +S1,S4 == Ahead,append([[S0,S1]],[[Ahead]],S3).
handarm(S0,S1,[Ahead|Atail],S3):- S4 is S0 +S1,S4 \== Ahead, handarm(S0,S1,Atail,S3).
handarm(_,_,[],S3):- nullact(S3).

handaaa(S0,S1,[Ahead|_],S3):- S4 is S0 +S1,S4 == Ahead,append([[Ahead]],[[S0,S1]],S3).
handaaa(S0,S1,[Ahead|Atail],S3):- S4 is S0 +S1,S4 \== Ahead, handaaa(S0,S1,Atail,S3).
handaaa(_,_,[],S3):- nullact(S3).

handfin([Ahead|Atail],S1,S2):- handfin(Atail,Ahead,S1,S2).
handfin([Ahead|[]],S1,S2,S3):- handarm(Ahead,S1,[S2],S3).

delete_one(_, [], []).
delete_one(Ahead, [Ahead|Atail], Atail).
delete_one(S0, [Ahead|Atail], [Ahead|Result]) :-
  delete_one(S0, Atail, Result).

armremov([_|Atail],S1,S2):- armremov(Atail,S1,S2,[]).
armremov([Ahead|_],S1,S2,_):- number(Ahead),delete_one(Ahead,S1,S2).
armremov([Ahead|_],S1,S2,_):- not(number(Ahead)),armremov(Ahead,S1,S2,_).

armremov1([Ahead|_],S1,S2):- armremov1(S1,Ahead,S2,[]).
armremov1(S1,[Ahead|_],S2,_):- number(Ahead),delete_one(Ahead,S1,S2).
armremov1(S1,[Ahead|_],S2,_):- not(number(Ahead)),armremov1(S1,Ahead,S2,_).

disarm([],[],[]):- true.
disarm(Adi, Bdi, Sol):- msort(Adi,S0),msort(Bdi,S1),disarm(S0,S1,Sol,[]).
disarm([S0head0,S0head1|S0tail],[S1head0,S1head1|S1tail],S2,S3):- S8 is S0head0 + S0head1, S9 is S1head0 + S1head1, S8 < S9,append([S1head0,S1head1],S1tail,S1),handarm(S0head0,S0head1,S1,S4),
        S4 == [],append([S0head0,S0head1],S0tail,S0),handaaa(S1head0,S1head1,S0,S5),append(S3,[S5],NS3),armremov1(S5,S0,NS0),disarm(NS0,S1tail,S2,NS3).
disarm([S0head0,S0head1|S0tail],[S1head0,S1head1|S1tail],S2,S3):- S8 is S0head0 + S0head1, S9 is S1head0 + S1head1, S8 < S9 ,append([S1head0,S1head1],S1tail,S1),handarm(S0head0,S0head1,S1,S4),
        S4 \== [],append(S3,[S4],NS3),armremov(S4,S1,NS1),disarm(S0tail,NS1,S2,NS3).
disarm([S0head0,S0head1|S0tail],[S1head0,S1head1|S1tail],S2,S3):- S8 is S0head0 + S0head1, S9 is S1head0 + S1head1, S8 >= S9,append([S0head0,S0head1],S0tail,S0),handaaa(S1head0,S1head1,S0,S5),
        S5 == [],append([S1head0,S1head1],S1tail,S1),handarm(S0head0,S0head1,S1,S4),append(S3,[S4],NS3),armremov(S4,S1,NS1),disarm(S0tail,NS1,S2,NS3).
disarm([S0head0,S0head1|S0tail],[S1head0,S1head1|S1tail],S2,S3):- S8 is S0head0 + S0head1, S9 is S1head0 + S1head1, S8 >= S9,append([S0head0,S0head1],S0tail,S0),handaaa(S1head0,S1head1,S0,S5),
        S5 \== [],append(S3,[S5],NS3),armremov1(S5,S0,NS0),disarm(NS0,S1tail,S2,NS3).
        
disarm(S0,[Ahead|[]],S2,S3):- handfin(S0,Ahead,S5),S5 \== [],append([S0],[[Ahead]],S4),append(S3,S4,S2).
disarm([Ahead|[]],S1,S2,S3):- handfin(S1,Ahead,S5),S5 \== [],append([[Ahead]],[S1],S4),append(S3,S4,S2).


%disarm([S0head0,S0head1|[]],[Ahead|[]],S2,S3):- S8 is S0head0 + S0head1,print(S8),print(Ahead),S8 == Ahead,append([[S0head0,S0head1]]],[[Ahead]],S4),append(S3,S4,S2).
%disarm([Ahead|[]],[S1head0,S1head1|[]],S2,S3):- S8 is S1head0 + S1head1,print(S8),print(Ahead),S8 == Ahead,append([[[Ahead]],[[S1head0,S1head1]],S4),append(S3,S4,S2).





/*The seating problem is to generate a sitting arrangement for a number of guests, with m tables and n chairs per table. 
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

t0 :- ['Q3tests/t0'].
/*t1 :- ['Q3tests/t1'].
t2 :- ['Q3tests/t2'].
t3 :- ['Q3tests/t3'].
t4 :- ['Q3tests/t4'].
t5 :- ['Q3tests/t5'].*/


% setUnion function from AS3
use_module(library(clpfd)).


member(A,[A|_]).
member(A,[B|L]) :- A \== B, member(A,L).  

notmember(_,[]).
notmember(A,[B|L]):- A\==B,notmember(A,L).

setUnion(S1,S2,S3):- setUnion(S1,S2,S2,S3).
setUnion([Ahead|Atail],Accum,S2,S3):- notmember(Ahead,Accum),append(Accum,[Ahead],OutList),setUnion(Atail,OutList,S2,S3).
setUnion([Ahead|Atail],Accum,S2,S3):- member(Ahead,Accum),setUnion(Atail,Accum,S2,S3).
setUnion([],Accum,_,Accum).


count([],0).
count([_|Atail], N) :- count(Atail, N1), N is N1 + 1.



gtable(L):- findall(S0,table(S0),L).
gguest(L):- findall(S0,guest(S0),L).

flike(S0,L):- findall(S1,like(S0,S1),S7),findall(S2,like(S2,S0),S8),append(S7,S8,L).

delhand([Ahead|Atail],S1,S2):- member(Ahead,S1),delete(S1,Ahead,S3),delhand(Atail,S3,S2).
delhand([Ahead|Atail],S1,S2):- notmember(Ahead,S1),delhand(Atail,S1,S2).
delhand([],S1,S1).

handseat(S0,S1):- handseat([],S1,S0).
handseat(S3,[Ahead|Atail],S6):- print(Ahead),flike(Ahead,S7),append(S7,[Ahead],S8),hand1seat(S2,S7,S8),delhand(S2,Atail,S4),append([S2],S3,S0),handseat(S0,S4,S6).       %S2 temp S7 all like, S8 all like contains self
handseat(S0,[],S0).

hand1seat(S0,[Ahead|Atail],S2):- flike(Ahead,S7),setUnion(S7,S2,S3),hand1seat(S0,Atail,S3).
hand1seat(S2,[],S2).

gcount(S0,S1):- gcount(S0,S1,[]).
gcount([Ahead|Atail],S1,S2):- print("good"),count(Ahead,S3),append(S2,[S3],S4),print(S4),gcount(Atail,S1,S4).
gcount([],S1,S1).


%handtable(S0,S1,S2,S3):- 

%S0 is the number of table 
motable(S0,S1):- motable(S0,S1,[]).
motable(S0,S1,S1):- S0 =< 0.
motable(S0,S1,S2):- print(S0),S3 is S0 - 1, append(S2,[[]],S4),motable(S3,S1,S4).

%S0 is return value,S1 is total number of seat,S2 is table set,S3 is the set of guest
arrange(S0,S1,S2,S3):- count(S3,S5),count(S2,S6),length(L,S5),print(S6),L ins 1..S6,motable(S6,S7).%arrange(S0,S1,S3,S7,L).

%S0 is return value,S1 is seat num,S2 is guest, S3 is [[]], S4 is L.
%arrange(S0,S1,[A2head|A2tail],S3,[A4head|A4tail]):- handtable(A2head,A4head,S3,S4),

q(Vs) :- Vs = [_,_,_,_],
	 global_cardinality(Vs, [1-2,3-_]), label(Vs).

%arrange(S0,S1,[A2head|A2tail],[A3head|A3tail],S4):- count(A3head,N1),N1 =< S1,append([A2head],[A3head],S5).

seating(P,S0):- seating(P,S0,[]).
seating(S0,S1,_):- gtable(S3),gguest(S4),handseat(S5,S4),arrange(S0,S1,S3,S5).