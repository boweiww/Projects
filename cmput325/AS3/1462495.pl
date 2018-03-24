% cmput325 assignment3
% bowei wang 1462495


/* question 1
 Define a predicate setUnion(+S1,+S2,-S3)
 where S1 and S2 are lists of numbers, and S3 represents the union of S1 and S2. 
E.g.?- setUnion([a,b,c],[b,a,c,e],S).
S=[a,b,c,e]. Assume S1 and S2 do not contain duplicates. It is required that the union does not contain repeated elements either. */


% predicate to check if A is a member of B

member(A,[A|_]).
member(A,[B|L]) :- A \== B, member(A,L).  

% predicate to check if A is not a member of B (which is not necessary, can be handeled by \+)

notmember(_,[]).
notmember(A,[B|L]):- A\==B,notmember(A,L).

%simply copy S2 and keep checking whether Ahead is a member of S2, if so ,append Ahead, if not, continue until S1 is empty

setUnion(S1,S2,S3):- setUnion(S1,S2,S2,S3).
setUnion([Ahead|Atail],Accum,S2,S3):- notmember(Ahead,Accum),append(Accum,[Ahead],OutList),setUnion(Atail,OutList,S2,S3).
setUnion([Ahead|Atail],Accum,S2,S3):- member(Ahead,Accum),setUnion(Atail,Accum,S2,S3).
setUnion([],Accum,_,Accum).


/*
question 2
Define a predicate swap(+L, -R)
where L is a list of elements and R is a list where the 1st two elements in L are swapped positions, so are the next two elements, and so on. If the number of elements in L is odd, then the last element is left as is. E.g,
?- swap([a,1,b,2], W).  W = [1,a,2,b].
?- swap([a,1,b], W).   W = [1,a,b].
Note that L may be empty, in which case R should be empty too. */

 %save the first element of L1 and handle next one and append them.
 %actureally it can be done in one sentence with [A,B|C]
 %However in this case, it will be hard to handle odd number elements cases

swap(L1,S3):- swap(L1,[],[],S3).
swap([Ahead|Atail],Accum,S2,S3):- S2 == [],swap(Atail,Accum,[Ahead],S3).
swap([Ahead|Atail],Accum,S2,S3):- S2 \== [],append([Accum,[Ahead],S2],S9),swap(Atail,S9,[],S3).
swap([],Accum,S2,S3):- S2 \== [],append(Accum,S2,S9),swap([],S9,[],S3).
swap([],Accum,[],Accum).


/*question 3
Define a predicate largest(+L,-N)
where L is a possibly nested list of numbers and N is the largest among them. We assume L is non-empty and it does not contain any empty sublists, either. E.g.
?- largest([3,4,[5,2],[1,7,2]],N). N=7. */

%this predicate will take the firest element as the base and compare it with the coming values.
%if Ahead is not a number, it will regared it as a list as find the max of the correspond list.

largest(S1,S2):- largest(S1,S2,[]).
largest([Ahead|Atail],S2,[]):- number(Ahead),largest(Atail,S2,Ahead).
largest([Ahead|Atail],S2,[]):- \+number(Ahead),largest(Ahead,S3,[]),largest(Atail,S2,S3).
largest([Ahead|Atail],S2,S3):- number(Ahead),Ahead > S3,largest(Atail,S2,Ahead).
largest([Ahead|Atail],S2,S3):- number(Ahead),Ahead =< S3,largest(Atail,S2,S3).
largest([Ahead|Atail],S2,S3):- \+number(Ahead),largest(Ahead,S4,S3),largest(Atail,S2,S4).
largest([],S2,S2).

/*question 4
Define a predicate countAll(+L,-N)
such that given a flat list L of atoms, the number of occurrences of every atom is counted. Thus, N should be a list of pairs [a,n] representing that atom a occurs in L n times. These pairs should appear in a non-increasing order. E.g.
?- countAll([a,b,e,c,c,b],N). N = [[b,2],[c 2],[a,1],[e,1]]
For atoms with the same account, the order is unimportant.
notice that the variable can not be defined as newxxxxx  
 */

 %check if A is a member of B, this version will skip the first element of B

countmember(A,[B|S1]):- nextmember(A,B,S1).
nextmember(A,[B|_],S1):- A \== B, countmember(A,S1).
nextmember(A,[A|_],_).

%check if A is not a member of B, this version will skip the first element of B

countnotmember(_,[]).
countnotmember(A,[B|L]):- nextnotmember(A,B,L).
nextnotmember(A,[B|_],L):- A\==B,countnotmember(A,L).

%countadd will handle a single character
%if not in list, append it, if in list, add count number

countadd(A,B,C):- countadd(A,[],B,C).
countadd(_,S2,[],S2).
countadd(S1,S2,[B|L],S5):- nextadd(S1,S2,B,L,S5).
nextadd(S1,S2,[B|L],S4,S5):- S1 == B,L1 is L + 1,append(S2,[[B,L1]],Ilovejk),countadd(S1,Ilovejk,S4,S5).
nextadd(S1,S2,[B|L],S4,S5):- S1 \== B,L1 is L + 0,append(S2,[[B,L1]],Ilovejk),countadd(S1,Ilovejk,S4,S5).

%define the reverse predicate to reverse number for order it.
%acturally better if directly use [number,char] structure

xreverse(L, R) :- xreverse(L, R, []).
xreverse([], R, R).
xreverse([Ahead|Atail], R, Accum) :- xreverse(Atail, R, [Ahead|Accum]).

%reverse all the sub lists of the result to get a appropriate sequence.

rev([],[]).
rev([[Number, Letter]|T], [[Letter, Number]|T1]):-rev(T,T1).
get_order_successors(L,S2):- rev(L,L1),sort(L1,L2),rev(L2,L3),xreverse(L3,S2).

%first count all chars, then sort the char.

countAll(S1,S2):- countAll(S1,[],S2).
countAll([Ahead|Atail],Accum,S2):- countmember(Ahead,Accum),countadd(Ahead,Accum,Ilovejk),countAll(Atail,Ilovejk,S2).
countAll([Ahead|Atail],Accum,S2):- countnotmember(Ahead,Accum),append(Accum,[[Ahead,1]],OutList),countAll(Atail,OutList,S2).
countAll([],Accum,S2):- get_order_successors(Accum,S2).

/*quetion 5
Define a predicate
sub(+L,+S,-L1)
where L is a possibly nested list of atoms, S is a list of pairs in the form [[x1,e1],...,[xn,en]], and L1 is the same as L except that any occurrence of xi is replaced by ei. Assume xi's are atoms and ei's are arbitrary expressions. E.g.
?- sub([a,[a,d],[e,a]],[[a,2]],L).  L= [2,[2,d],[e,2]]. */

%check each Ahead is a atom, if not an atom, regard it as a list and go deeper, if it is an atom, compare with the variable that to be handeled and replace it. 
%it somehow looks like a lisp function.

xsub(S1,[Ahead|Atail],S3):- xsub(S1,Ahead,Atail,[],S3).
xsub([],_,_,S4,S4).
xsub([Ahead|Atail],S5,S6,S0,S4):- atomic(Ahead),Ahead == S5,append(S0,S6,S2),xsub(Atail,S5,S6,S2,S4).
xsub([Ahead|Atail],S5,S6,S0,S4):- atomic(Ahead),Ahead \== S5,append(S0,[Ahead],S2),xsub(Atail,S5,S6,S2,S4).
xsub([Ahead|Atail],S5,S6,S0,S4):- \+atomic(Ahead),xsub(Ahead,S5,S6,[],S14),append(S0,[S14],S2),xsub(Atail,S5,S6,S2,S4).



sub(S1,[Ahead|Atail],S3):- xsub(S1,Ahead,S0),sub(S0,Atail,S3).
sub(S0,[],S0).



/*question 6
c325(S0, Name, S1, S2, S2, S4, S5, final)
where S0 takes the form, e.g., fall_2011, winter_2017 and so on, Name is a student name, and the rest are about the marks of the course components.
There are some facts about the setup of course components, in the form 
 setup(S0,Type,Max,Percentage)
where Type is one of {S1,S2,S2,S4,S5,final}, Max is the maximum marks for Type and percentage is the 
weight of Type in the course. 
Given such a database, you are asked to design the following queries.
 (i) query1(+S0, +Name, -Total)
Given a S0 and a student name, Total should be bound to the total mark, in terms of percentage out of 100, of the student for that S0. 
 (ii) query2(+S0, -L).
Given a S0, find all students whose final exam shows an improvement over the S5, in the sense that the percentage obtained from the final is strictly better than that of the S5. 
 (iii) query3(+S0,+Name,+Type,+NewMark)
Updates the record of Name for S0 where Type gets NewMark. If the record is not in the database, print the message "record not found". Here is an example 
 ?- query3(fall_2011,sam,final,75).
If sam's record for fall_2011 is in the database, then change his final exam mark to 75, otherwise print the message "record not found". 
*/

%it will not work without database initialed.
%database defined at here
insert :-
    assert(c325(fall_2010,john,14,13,15,10,76,87)),
    assert(c325(fall_2010,lily, 9,12,14,14,76,92)),
    assert(c325(fall_2010,peter,8,13,12,9,56,58)),
    assert(c325(fall_2010,ann,14,15,15,14,76,95)),
    assert(c325(fall_2010,ken,11,12,13,14,54,87)),
    assert(c325(fall_2010,kris,13,10,9,7,60,80)),
    assert(c325(fall_2010,audrey,10,13,15,11,70,80)),
    assert(c325(fall_2010,randy,14,13,11,9,67,76)),
    assert(c325(fall_2010,david,15,15,11,12,66,76)),
    assert(c325(fall_2010,sam,10,13,10,15,65,67)),
    assert(c325(fall_2010,kim,14,13,12,11,68,78)),
    assert(c325(ann,final,97)),
    assert(setup(fall_2010,as1,15,0.1)),
    assert(setup(fall_2010,as2,15,0.1)),
    assert(setup(fall_2010,as3,15,0.1)),
    assert(setup(fall_2010,as4,15,0.1)),
    assert(setup(fall_2010,midterm,80,0.25)),
    assert(setup(fall_2010,final,100,0.35)).


%database end
%query 1
% idea:get the information of the specified student and the percent of different exams, mult them together 

%take the first element
takelist([Ahead|_],S1):- append([],Ahead,S1).

%take the correspond marks of the student.
%S1:as1   S2: as2   S3: as3   S4: as4    S5:mid    S6: final

findque1(L,S0,Name) :-
   findall([S1, S2, S3, S4, S5, S6],c325(S0,Name, S1, S2, S3, S4, S5, S6),L).

%count marks with different percentage 

counttotal(S0,[Ahead|Atail],S2):- counteach(S0,Ahead,S3),counttotal(S0,Atail,S4),S2 is S3 + S4.
counttotal(_,[],S0):- S0 is 0.
counteach(S0,[Ahead|Atail],S2):- Ahead == as1, nth0(0,S0,S3),nth0(0,Atail,S4),nth0(1,Atail,S5),counthandle(S3,S4,S5,S2).
counteach(S0,[Ahead|Atail],S2):- Ahead == as2, nth0(1,S0,S3),nth0(0,Atail,S4),nth0(1,Atail,S5),counthandle(S3,S4,S5,S2).
counteach(S0,[Ahead|Atail],S2):- Ahead == as3, nth0(2,S0,S3),nth0(0,Atail,S4),nth0(1,Atail,S5),counthandle(S3,S4,S5,S2).
counteach(S0,[Ahead|Atail],S2):- Ahead == as4, nth0(3,S0,S3),nth0(0,Atail,S4),nth0(1,Atail,S5),counthandle(S3,S4,S5,S2).
counteach(S0,[Ahead|Atail],S2):- Ahead == midterm, nth0(4,S0,S3),nth0(0,Atail,S4),nth0(1,Atail,S5),counthandle(S3,S4,S5,S2).
counteach(S0,[Ahead|Atail],S2):- Ahead == final, nth0(5,S0,S3),nth0(0,Atail,S4),nth0(1,Atail,S5),counthandle(S3,S4,S5,S2).

%calculate the reuslt.

counthandle(S0,S1,S2,S3):- S4 is S0 / S1, S3 is S4 * S2.

asetup(Semester,S0):- findall([S1,S2,S3],setup(Semester,S1,S2,S3),S0).

query1(Semester,Name,L) :- findque1(S0,Semester,Name),takelist(S0,S1),asetup(Semester,S9),counttotal(S1,S9,S8),L is S8 * 100.


%query2
%can be better if use the previous counteach, but it may cost more calculate

%get the mid and final marks and name of all the students.

findque2(Semester,S0):-findall([Name,S5, S6],c325(Semester,Name, _, _, _, _, S5, S6),S0).

%take different fullmarks for mid and final.

cond2(Semester,Type,S1,S2):- setup(Semester,Type,S1,S2),Type == midterm.
cond2(Semester,Type,S1,S2):- setup(Semester,Type,S1,S2),Type == final.

takeset(Semester,S0):- findall([Type,S1,S2],cond2(Semester,Type,S1,S2),S0).

%count the percentage of students final and mid.
%we dont know the sequence of mid and final which go first, so handle all two cases. 

countmid(S0,S1,S2):- nth0(0,S0,S3),nth0(0,S3,S4),S4 == midterm,nth0(1,S3,S5),S2 is S1/S5.
countmid(S0,S1,S2):- nth0(1,S0,S3),nth0(0,S3,S4),S4 == midterm,nth0(1,S3,S5),S2 is S1/S5.

countfinal(S0,S1,S2):- nth0(0,S0,S3),nth0(0,S3,S4),S4 == final,nth0(1,S3,S5),S2 is S1/S5.
countfinal(S0,S1,S2):- nth0(1,S0,S3),nth0(0,S3,S4),S4 == final,nth0(1,S3,S5),S2 is S1/S5.

%compare students mid percent and final percent if final percent is larger, return student name
%if final percent is not larger than mid percent, return empty

nullact([]).

handsome([Ahead|Atail],S1,S2):- nth0(0,Atail,S3),countmid(S1,S3,S4),nth0(1,Atail,S5),countfinal(S1,S5,S6),S6 > S4,append([],[Ahead],S2).
handsome([_|Atail],S1,S2):- nth0(0,Atail,S3),countmid(S1,S3,S4),nth0(1,Atail,S5),countfinal(S1,S5,S6),S6 =< S4,nullact(S2).


improvhandle([Ahead|Atail],S1,L):- handsome(Ahead,S1,S0),improvhandle(Atail,S1,S2),append(S2,S0,L).
improvhandle([],_,[]). %:- L is [].

query2(Semester, L):- findque2(Semester,S0),takeset(Semester,S1),improvhandle(S0,S1,L).



%query3
%nreplace is defined to replace the nth element in the list.
%this should be useful fo future.

nreplace([Ahead|Atail],S1,S2,S3):- S1 > 0,S5 is S1 - 1, nreplace(Atail,S5,S2,S4),append([Ahead],S4,S3).
nreplace([_|Atail],S1,S2,S3):- S1 =< 0,append(S2,Atail,S3).

%update is done at here, it will replace the old mark with the new one and put new marks into database.

updatemark(Semester,Name,S0,NewMark,[Ahead|_]):- nreplace(Ahead,S0,[NewMark],S1),nth0(0,S1,S10),nth0(1,S1,S11),nth0(2,S1,S12),nth0(3,S1,S13),nth0(4,S1,S14),nth0(5,S1,S15),assert(c325(Semester,Name,S10,S11,S12,S13,S14,S15)),write("update success").

%handq3 will check whether the correspond is exist. If not exist, print error message.

handq3(Semester,Name,S0) :-
   findall([S1,S2,S3,S4,S5,S6],c325(Semester,Name,S1,S2,S3,S4,S5,S6),S0),S0 == [],write("record not found"),false.
handq3(Semester,Name,S0) :-
   findall([S1,S2,S3,S4,S5,S6],c325(Semester,Name,S1,S2,S3,S4,S5,S6),S0),S0 \== [].

%seperate different type of marks into different cases. 
%check whether user entered a valid type of mark.

typehandle(Type,S1):- typetonum(Type,S1).
typehandle(Type,S1):- \+typetonum(Type,S1),write("record not found in setup"),false.

typetonum(Type,S1):- Type == as1, S1 is 0.
typetonum(Type,S1):- Type == as2, S1 is 1.
typetonum(Type,S1):- Type == as3, S1 is 2.
typetonum(Type,S1):- Type == as4, S1 is 3.
typetonum(Type,S1):- Type == midterm, S1 is 4.
typetonum(Type,S1):- Type == final, S1 is 5.

query3(Semester,Name,Type,NewMark):- handq3(Semester,Name,S0),typehandle(Type,S1),retract(c325(Semester,Name,_,_,_,_,_,_)),updatemark(Semester,Name,S1,NewMark,S0). 

