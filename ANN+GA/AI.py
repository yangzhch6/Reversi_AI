from reversi import *
import random
import numpy as np
import math
from copy import deepcopy

invalidMove = (-1, -1)
structure = [n * n, 2 * n * n, 2 * n * n, n * n] # network structure
population = 50
bestNetworksPopulation = 5
previousGeneration = 3335
mutationAmount = 0.3

# sigmoid function
def sigmoid(x):
    return 1 / (1 + math.exp(-x))

# negative function
def negative(x):
    return -x

# RELU function  
def relu(x):
    if x > 0:
        return x
    return 0

structure = [n * n, 2 * n * n, 2 * n * n, n * n] # 网络结构
# 定义神经网络
def evaluate(board, structure, weights, turn):
    layer = np.array(sum(board, [])) # 第一层
    negativeForArray = np.vectorize(negative)
    if turn == w: # 默认为白色
        layer = negativeForArray(layer)
    for i in range(1, len(structure)):
        layer = np.append(layer, 1) # 添加偏置b
        layer = np.dot(weights[i - 1], layer) # 计算下一层
        sigmoidForArray = np.vectorize(sigmoid)
        layer = sigmoidForArray(layer) # 应用sigmoid函数
    return layer

# 选择最优落子策略
def chooseMove(board, sturcture, weights, turn):
    # 神经网络输出结果
    results = evaluate(board, structure, weights, turn)
    # 可落子列表 
    validMoves = generateValidMovesList(board, turn) 
    bestMove = invalidMove
    highestValue = -1
    for coordinates in validMoves: # choose best move
        valueOfCoordinates = results[n * coordinates[0] + coordinates[1]]
        if valueOfCoordinates > highestValue:
            highestValue = valueOfCoordinates
            bestMove = coordinates
    return bestMove

# 随机初始化神经网络
def generateNetwork(structure):    
    weights = []
    for i in range(len(structure) - 1):
        weights.append(np.array([[random.uniform(-1, 1) for _ in range(structure[i] + 1)] for _ in range(structure[i + 1])]))
    return weights

# 加载神经网络
def loadNetworks(structure, previousGeneration):
    with open("gen" + str(previousGeneration), "r") as file:
        lines = file.read().splitlines()
        for i in range(len(lines) - 1, -1, -1): # removing fitness information
            if lines[i][0] == "F":
                lines.pop(i)
           
        weightsList = sum([line.split("\t") for line in lines], [])
        weightsListIndex = 0
        networks = []

        while len(networks) < bestNetworksPopulation:
            weights = []
            for i in range(len(structure) - 1):
                layer = [[0 for _ in range(structure[i] + 1)] for _ in range(structure[i + 1])]
                for j in range(structure[i + 1]):
                    for k in range(structure[i] + 1):  
                        layer[j][k] = float(weightsList[weightsListIndex])   
                        weightsListIndex += 1   
                weights.append(np.array(layer))   
            networks.append(weights)    
        return networks      

# 存储神经网络
def saveNetworks(networks, sortedNetworksTuples, previousGeneration):
    file = open("gen" + str(previousGeneration), "w")
    for networkTuple in sortedNetworksTuples:
        fitness = networkTuple[0]
        file.write("Fitness:" + str(fitness) + "\n")
        networkIndex = networkTuple[1]
        for p in range(len(networks[networkIndex])):
            for q in range(len(networks[networkIndex][p])):
                file.write(str(networks[networkIndex][p][q][0]))
                for r in range(1, len(networks[networkIndex][p][q])):
                    file.write("\t" + str(networks[networkIndex][p][q][r]))
                file.write("\n")
    file.close()


if __name__ == "__main__":
    # create new set of neura networks if there is no previous generations
    if previousGeneration < 0:
        networks = [generateNetwork(structure) for _ in range(population)]
    
    isLoad = False
    while previousGeneration < 10000000:
        # repopulate
        if previousGeneration >= 0:
            # read networks from file if there is some previous generations
            if isLoad:
                bestnetworks = []
                bestnetworks.append(networks[sortedNetworksTuples[0][1]])
                bestnetworks.append(networks[sortedNetworksTuples[1][1]])
                bestnetworks.append(networks[sortedNetworksTuples[2][1]])
                bestnetworks.append(networks[sortedNetworksTuples[3][1]])
                bestnetworks.append(networks[sortedNetworksTuples[4][1]])
                networks = bestnetworks
            else:
                isLoad = True
                networks = loadNetworks(structure, previousGeneration)
            bestNetworkPoplation = len(networks)
            #print(bestNetworkPoplation)
            # mutation变异
            for i in range(bestNetworkPoplation):
                for _ in range(5): # do 5 mutations for each neural network
                    newNetwork = deepcopy(networks[i])
                    for p in range(len(newNetwork)):
                        for q in range(len(newNetwork[p])):
                            for r in range(len(newNetwork[p][q])):
                                newNetwork[p][q][r] += random.uniform(-mutationAmount, mutationAmount)
                    networks.append(newNetwork)
            #print("mutation: ", len(networks))
            # crossover交叉
            for i in range(bestNetworkPoplation):
                for j in range(bestNetworkPoplation):
                    if i != j:
                        for _ in range(1): # do 1 crossovers for each neural network
                            newNetwork = deepcopy(networks[i])
                            useNetworkJWeight = True if random.randrange(0, 1) < 0.5 else False
                            for p in range(len(newNetwork)):
                                for q in range(len(newNetwork[p])):
                                    for r in range(len(newNetwork[p][q])):
                                        useNetworkJWeight = not useNetworkJWeight if random.randrange(0, 1) < 30/(8*n*n*n*n) else useNetworkJWeight
                                        if useNetworkJWeight:
                                            newNetwork[p][q][r] = networks[j][p][q][r]
                            networks.append(newNetwork)
            #print("cross: ",len(networks))
        # play against each other
        previousGeneration += 1
        fitnesses = [0 for _ in range(population)]
        for i in range(population): # black
            for j in range(population): # white
                if i != j:
                    turn = b
                    board = generateBoard()
                    whiteCanPlay = True # 若白色能下子
                    blackCanPlay = True # 若黑色能下子
                    while whiteCanPlay or blackCanPlay:
                        move = chooseMove(board, structure, networks[i if turn == b else j], turn)
                        if move == invalidMove:
                            if turn == w:
                                whiteCanPlay = False
                            if turn == b:
                                blackCanPlay = False
                        else :
                            if turn == w:
                                whiteCanPlay = True
                            if turn == b:
                                blackCanPlay = True
                            board = makeMove(board, move[0], move[1], turn)
                        # 交换下棋方
                        turn = opposite(turn)
                    #print("Generation " + str(previousGeneration) + ": network " + str(i), black, str(countChess(board, b)),"vs.", \
                    #      str(countChess(board, w)), white + " network " + str(j))
                    if countChess(board, b) > countChess(board, w):
                        fitnesses[i] = fitnesses[i] + 1
                    elif countChess(board, b) < countChess(board, w):
                        fitnesses[j] = fitnesses[j] + 1

        # sort networks according to fitnesses
        networksIndices = [i for i in range(len(networks))]
        sortedNetworksTuples = [(networksIndex, fitness) for networksIndex, fitness in reversed(sorted(zip(fitnesses, networksIndices)))]

        print("Generation", previousGeneration, "Fitness: ", sortedNetworksTuples[0][0],\
        sortedNetworksTuples[1][0],\
        sortedNetworksTuples[2][0],\
        sortedNetworksTuples[3][0],\
        sortedNetworksTuples[4][0])

        print("Index  ", previousGeneration, "Fitness: ", sortedNetworksTuples[0][1],\
        sortedNetworksTuples[1][1],\
        sortedNetworksTuples[2][1],\
        sortedNetworksTuples[3][1],\
        sortedNetworksTuples[4][1])

        # eliminate
        sortedNetworksTuples = sortedNetworksTuples[ : bestNetworksPopulation]
        # save best networks in the population to file
        if previousGeneration % 5 == 0:
            saveNetworks(networks, sortedNetworksTuples, previousGeneration)