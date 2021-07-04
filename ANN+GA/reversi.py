#Reversi

n = 8
white = "○"
black = "●"
bg = " "

w = -1 
b = 1

def playerASCII():
    return white if turn == w else black

def countChess(board, turn): 
    return sum(board[i].count(turn) for i in range(n))

def printBoard(board): 
    print()
    print("  ", end = "")
    for i in range(n):
        print(i, end = " ")
    print()
    for i in range(n): 
        print(i, end = " ")
        for j in range(n):
            print(white if board[j][i] == w else \
                  (black if board[j][i] == b else bg), end = " ")
        print()
    print(black + ":" + strLoop(countChess(b)), end = "  ")
    print(white + ":" + str(countChess(w)), end = "")
    print()

def generateBoard(): 
    board = [[0 for _ in range(n)] for _ in range(n)]
    board[n // 2 - 1][n // 2 - 1] = w
    board[n // 2][n // 2] = w
    board[n // 2][n // 2 - 1] = b
    board[n // 2 - 1][n // 2] = b
    return board

def outOfBound(x, y): 
    return x < 0 or x >= n or y < 0 or y >= n

def neighbourIsOpposite(board, x, y, dx, dy, turn): 
    if outOfBound(x + dx, y + dy):
        return False
    return board[x + dx][y + dy] == opposite(turn)

def checkSandwich(board, x, y, dx, dy, turn):
    if outOfBound(x + dx, y + dy): # out of bound
        return False
    if board[x + dx][y + dy] == 0: # blank
        return False
    if board[x + dx][y + dy] == turn: # neighor
        return False
    while board[x + dx][y + dy] != turn:
        x = x + dx
        y = y + dy
        if outOfBound(x + dx, y + dy): # out of bound
            return False
        if board[x + dx][y + dy] == 0: # blank
            return False
    return True

def opposite(turn): # change turn
    return w if turn == b else b

def isLegal(board, x, y, turn):
    if outOfBound(x, y): # out of bound
        return False
    if board[x][y] != 0: # space is occupied
        return False
    for i in range(-1, 2):
        for j in range(-1, 2):
            if neighbourIsOpposite(board, x, y, i, j, turn) and \
               checkSandwich(board, x, y, i, j, turn):
                return True        
    return False

def replaceNeighbours(board, x, y, dx, dy, turn):
    if checkSandwich(board, x, y, dx, dy, turn):
        board[x + dx][y + dy] = turn
        replaceNeighbours(board, x + dx, y + dy, dx, dy, turn)

def generateValidMovesList(board, turn):
    return [(i,j) for i in range(n) for j in range(n) \
                      if isLegal(board, i, j, turn)]

def makeMove(board, x, y, turn):
    board[x][y] = turn
    for i in range(-1, 2):
        for j in range(-1, 2):
            if neighbourIsOpposite(board, x, y, i, j, turn):
                replaceNeighbours(board, x, y, i, j, turn)
    return board


### printing who wins
##print()
##print("Game Over!")
##printBoard(board)
##print(black + " Wins!" if countChess(b) > countChess(w) else \
##      (white + " Wins!" if countChess(b) < countChess(w) else \
##       "It's a tie!"))
