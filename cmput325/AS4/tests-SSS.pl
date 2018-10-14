p([],Q,Q).
p([A|L],S,SL) :-
A > 5,

p(L,[A|S],SL).
p([A|L],S,SL) :- p(L,S,SL).