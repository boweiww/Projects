

color(blue).
color(red).
% the edges
edge(1,2).
edge(1,3).
edge(2,4).
edge(5,2).

coloring(L) :-
    setof(Node,X^Y^(edge(Node, X); edge(Y,Node)), LN),
    set_color(LN, L).


set_color([], []).
set_color([H | T], [H/C | TC]) :-
    set_color(T, TC),
    color(C),
    forall(member(Node/Color, TC),
           (   (edge(Node, H) -> Color \= C; true),
           ( edge(H, Node) -> Color \= C; true))).