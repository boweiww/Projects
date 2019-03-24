#!/usr/bin/python3
# /usr/local/bin/python3
# Set the path to your python3 above
import random

from gtp_connection import GtpConnection
from board_util import GoBoardUtil, BLACK, WHITE, EMPTY,BORDER, PASS,where1d
from simple_board import SimpleGoBoard
numSimulations = 10
BLOCK = 1
PLAY = 0
class Gomoku():
    def __init__(self):
        """
        Gomoku player that selects moves randomly
        from the set of legal moves.
        Passe/resigns only at the end of game.

        """
        self.name = "GomokuAssignment3"
        self.version = 1.0



    def check(self,state, point, shift,type):

        color = state.board[point]
        count = 1
        d = shift
        p = point
        while True:
            p = p + d
            if state.board[p] == color:
                count = count + 1
                if count == 4 and state.board[p - (4 * d)] == EMPTY and state.board[p + d] == EMPTY:
                    try:
                        if type == BLOCK and (state.board[p - (5 * d)] != EMPTY or state.board[p + 2*d] != EMPTY):
                            if (state.board[point-2*d] != EMPTY):
                                return [point]
                            return [point, p - (4 * d), p + d]
                        else:
                            return [point]
                    except:
                        return [point]


            else:
                break
        d = -d
        p = point
        while True:
            p = p + d
            if state.board[p] == color:
                count = count + 1
                if count == 4 and state.board[p - (4 * d)] == EMPTY and state.board[p + d] == EMPTY:
                    try:
                        if type == BLOCK and (state.board[p - (5 * d)] != EMPTY or state.board[p + 2*d] != EMPTY):
                            if (state.board[point-2*d] != EMPTY):
                                return [point]
                            return [point, p - (4 * d), p + d]
                        else:
                            return [point]
                    except:
                        return [point]

            else:
                break

        return False

    def check3_4(self,state, point,player,type):
        """
            Check if the point causes the game end for the game of Gomoko.
            """
        # check horizontal
        state.play_move_gomoku(point,player)
        c = self.check(state, point, 1,type)
        if c:
            state.undoMove()
            return c

        # check vertical
        c = self.check(state, point, state.NS,type)
        if c:
            state.undoMove()
            return c

        # check y=x
        c = self.check(state, point, state.NS + 1,type)
        if c:
            state.undoMove()
            return c

        # check y=-x
        c = self.check(state, point, state.NS - 1,type)
        if c:
            state.undoMove()
            return c
        state.undoMove()
        return False

    def OpenFour(self,state, current_player,type):
        points = where1d(state.board == EMPTY)
        moves = []
        for point in points:
            move = self.check3_4(state, point,current_player,type)
            if move:
                for mov in move:
                    moves.append(mov)
        if moves == []:
            return False
        else:
            legal = state.get_empty_points()
            moves = list(set(legal) & set(moves))
            return moves

    def RuleBased(self,board):
        moves = board.get_empty_points()
        numMoves = len(moves)
        if numMoves == 0:
            return 'Random',PASS

        # check win
        nextmove = []
        for i in range(numMoves):
            board.play_move_gomoku(moves[i],board.current_player)
            game_end, winner = board.check_game_end_gomoku()
            board.undoMove()
            if game_end and winner == board.current_player:
                nextmove.append(moves[i].tolist())
        if len(nextmove)!= 0:
            return 'Win', nextmove

        # check block
        nextmove = []
        current = GoBoardUtil.opponent(board.current_player)
        for i in range(numMoves):
            board.play_move_gomoku(moves[i],current)
            game_end, winner = board.check_game_end_gomoku()
            board.undoMove()
            if game_end and winner == current:
                nextmove.append(moves[i].tolist())
        if len(nextmove) != 0:
            return 'BlockWin', nextmove
        board.current_player = GoBoardUtil.opponent(current)


        # check OpenFour

        mov = self.OpenFour(board, board.current_player,PLAY)
        if mov:
            return 'OpenFour',mov
        # check BlockOpenFour
        mov = self.OpenFour(board, GoBoardUtil.opponent(board.current_player),BLOCK)
        if mov:
            return 'BlockOpenFour',mov
        # random
        return 'Random',self.FlatMC(board)

    def random(self,board):
        return 'Random',self.FlatMC(board)

    def get_move(self, board, color):
        return GoBoardUtil.generate_random_move_gomoku(board)

    def FlatMC(self,board):
        moves = board.get_empty_points()
        numMoves = len(moves)

        if numMoves == 0:
            return PASS
        score = [0] * numMoves

        for i in range(numMoves):
            move = moves[i]
            score[i] = self.simulate(board, move)

        #print(score)
        # bestIndex = score.index(max(score))

        # highest = max(score)
        # best = []
        # for i in range(len(score)):
        #     if score[i] == highest:
        #         best.append(moves[i])
        # # best = moves[bestIndex]
        # # #print("Best move:", best, "score", score[best])
        # # assert best in board.get_empty_points()
        # print(best)
        return moves.tolist()

    def simulate(self, board, move):
        stats = [0] * 3
        board.play_move_gomoku(move,board.current_player)
        # moveNr = board.moveNumber()
        for _ in range(numSimulations):
            b = board.copy()
            winner, _ = self.sim(b)
            stats[winner] += 1
            # board.resetToMoveNumber(moveNr)
        # assert sum(stats) == numSimulations
        # assert moveNr == board.moveNumber()
        board.undoMove()
        eval = (stats[BLACK] + 0.5 * stats[EMPTY]) / numSimulations
        if board.current_player == WHITE:
            eval = 1 - eval
        return eval


    def sim(self,board):
        i = 0
        allMoves = board.get_empty_points()
        random.shuffle(allMoves)
        game_end, winner = board.check_game_end_gomoku()
        # print(game_end)
        while not game_end:
            board.play_move_gomoku(allMoves[i],board.current_player)
            i += 1
            game_end, winner = board.check_game_end_gomoku()

        return winner, i

def run():
    """
    start the gtp connection and wait for commands.
    """
    board = SimpleGoBoard(7)
    con = GtpConnection(Gomoku(), board)
    con.start_connection()


if __name__ == '__main__':
    run()
