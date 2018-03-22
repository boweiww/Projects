% cmput325 assignment3
% bowei wang 1462495
% question 1
% Define a predicate setUnion(+S1,+S2,-S3)
% where S1 and S2 are lists of numbers, and S3 represents the union of S1 and S2.






member(A,[A|_]).
member(A,[B|L]) :- A \== B, member(A,L).

notmember(_,[]).
notmember(A,[B|L]):- A\==B,notmember(A,L).



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
xappend([], L, L).
% xappend([H|T],L,[H|R]):- xappend(T,L,R).


swap(L1,S3):- write(L1),swap(L1,[],[],S3).
swap([Ahead|Atail],Accum,S2,S3):- S2 == [],swap(Atail,Accum,[Ahead],S3).
swap([Ahead|Atail],Accum,S2,S3):- write(S2),S2 \== [],write("good"),append([Ahead],S2,A1),write(A1),write(Accum),xappend(Accum,A1,newAccum),write(newAccum),swap(Atail,newAccum,[],S3).
swap([],Accum,S2,S3):- S2 \== [],append(Accum,S2,newAccum),swap([],newAccum,[],S3).
swap([],Accum,[],Accum).


/*question 3
Define a predicate largest(+L,-N)
where L is a possibly nested list of numbers and N is the largest among them. We assume L is non-empty and it does not contain any empty sublists, either. E.g.
?- largest([3,4,[5,2],[1,7,2]],N). N=7. */

largest(S1,S2):- largest(S1,S2,[]).
largest([Ahead|Atail],S2,[]):- number(Ahead),largest(Atail,S2,Ahead).
largest([Ahead|Atail],S2,[]):- \+number(Ahead),largest(Ahead,S3,[]),largest(Atail,S2,S3).
largest([Ahead|Atail],S2,S3):- number(Ahead),Ahead > S3,largest(Atail,S2,Ahead).
largest([Ahead|Atail],S2,S3):- number(Ahead),Ahead =< S3,largest(Atail,S2,S3).
largest([Ahead|Atail],S2,S3):- \+number(Ahead),largest(Ahead,S4,S3),largest(Atail,S2,S4).
largest([],S2,S2).

/*Define a predicate countAll(+L,-N)
such that given a flat list L of atoms, the number of occurrences of every atom is counted. Thus, N should be a list of pairs [a,n] representing that atom a occurs in L n times. These pairs should appear in a non-increasing order. E.g.
?- countAll([a,b,e,c,c,b],N). N = [[b,2],[c 2],[a,1],[e,1]]
For atoms with the same account, the order is unimportant. */

countmember(A,[B|S1]):- nextmember(A,B,S1).
nextmember(A,[B|_],S1):- A \== B, countmember(A,S1).
nextmember(A,[A|_],_).

countnotmember(_,[]).
countnotmember(A,[B|L]):- nextnotmember(A,B,L).
nextnotmember(A,[B|_],L):- A\==B,countnotmember(A,L).

countadd(A,B):- countadd(A,[],B).
countadd(A,[C|L1],[B|L]):- A == C, L1

countAll(S1,S2):- countAll(S1,[],S2).
countAll([Ahead|Atail],Accum,S2):- countnotmember(Ahead,Accum),append(Accum,[Ahead,1],OutList),countAll(Atail,OutList,S2).
countAll([Ahead|Atail],Accum,S2):- countmember(Ahead,Accum),countAll(Atail,Accum,S2).
countAll([],Accum,_,Accum).