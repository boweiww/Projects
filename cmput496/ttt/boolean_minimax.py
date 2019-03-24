# Cmput 496 sample code
# Boolean Minimax
# Written by Martin Mueller

import time
from game_basics import EMPTY, BLACK, WHITE, opponent

def minimaxBooleanOR(state):
    assert state.toPlay == BLACK
    if state.endOfGame():
        return state.isWinner(BLACK)
    for m in state.legalMoves():
        state.play(m)
        isWin = minimaxBooleanAND(state)
        state.undoMove()
        if isWin:
            return True
    return False

def minimaxBooleanAND(state):
    assert state.toPlay == WHITE
    if state.endOfGame():
        return state.isWinner(BLACK)
    for m in state.legalMoves():
        state.play(m)
        isLoss = not minimaxBooleanOR(state)
        state.undoMove()
        if isLoss:
            return False
    return True

def solveForBlack(state): 
    win = False
    start = time.process_time()
    if state.toPlay == BLACK:
        win = minimaxBooleanOR(state)
    else:
        win = minimaxBooleanAND(state)
    timeUsed = time.process_time() - start
    return win, timeUsed
