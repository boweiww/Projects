# Cmput 496 sample code
# Solve TicTacToe with Boolean Minimax
# Written by Martin Mueller

from game_basics import BLACK, WHITE, EMPTY, colorAsString, winnerAsString
from tic_tac_toe import TicTacToe
from boolean_negamax import solveForColor

def solveAndPrint(state, solver): 
    print("Board:")
    state.print()
    print(colorAsString(state.toPlay), "to play")
    result, timesUsed = solver(state)
    print("Result: {}\nTimes used (-1 = not searched): Black {:.4f}, White {:.4f}\n".format(
        winnerAsString(result), timesUsed[0], timesUsed[1]))

def solveForWinLossDraw(state):
    winBlack, timeBlack = solveForColor(state, BLACK)
    if winBlack:
        return BLACK, (timeBlack, -1)
    else:
        winner = EMPTY
        winWhite, timeWhite = solveForColor(state, WHITE)
        if winWhite:
            winner = WHITE
        return winner, (timeBlack, timeWhite)

# An example game, with some mistakes by both. 
# Call solve after every move.
def testGame(solver):
    t = TicTacToe()
    solveAndPrint(t, solver)
    t.play(0)
    solveAndPrint(t, solver)
    t.play(3)
    solveAndPrint(t, solver)
    t.play(1)
    solveAndPrint(t, solver)
    t.play(4)
    solveAndPrint(t, solver)
    t.play(5)
    solveAndPrint(t, solver)
    t.play(2)
    solveAndPrint(t, solver)
    t.play(6)
    solveAndPrint(t, solver)
    t.play(7)
    solveAndPrint(t, solver)
    t.play(8)
    solveAndPrint(t, solver)

testGame(solveForWinLossDraw)

