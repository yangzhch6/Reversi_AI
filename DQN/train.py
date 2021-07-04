import copy

from Reversi import Reversi
from Agent import DQNAgent


if __name__ == "__main__":

    # parameters
    n_epochs = 1000
    env = Reversi()
    # playerID
    playerID = [env.Black, env.White, env.Black]

    # player agent
    players = []
    players.append(DQNAgent(env.enable_actions, env.name, env.screen_n_rows, env.screen_n_cols))
    players.append(DQNAgent(env.enable_actions, env.name, env.screen_n_rows, env.screen_n_cols))


    for e in range(n_epochs):
        # reset
        env.reset()
        terminal = False
        while terminal == False: 
            for i in range(0, len(players)):

                state = env.screen
                targets = env.get_enables(playerID[i])

                exploration = (n_epochs - e + 20)/(n_epochs + 20)

                if len(targets) > 0:
                    for tr in targets:
                        tmp = copy.deepcopy(env)
                        tmp.update(tr, playerID[i])
                        win = tmp.winner()
                        end = tmp.isEnd()
 
                        state_X = tmp.screen
                        target_X = tmp.get_enables(playerID[i+1])
                        if len(target_X) == 0:
                            target_X = tmp.get_enables(playerID[i])

                        # 互相博弈
                        for j in range(0, len(players)):
                            reword = 0
                            if end == True:
                                if win == playerID[j]:
                                    # 获胜者奖励100点
                                    reword = 100.0
                            players[j].store_experience(state, targets, tr, \
                            reword, state_X, target_X, end)
                   
                    action = players[i].select_action(state, targets, exploration)
                    
                    env.update(action, playerID[i])
                    loss = players[i].current_loss
                    Q_max, Q_action = players[i].select_enable_action(state, targets)
                    #print("player:{:1d} | pos:{:2d} | LOSS: {:.4f} | Q_MAX: {:.4f} | Q_ACTION: {:.4f}".format(
                    #         playerID[i], action, loss, Q_max, Q_action))
                terminal = env.isEnd()

        for j in range(0, len(players)):
            if e > n_epochs*0.3:
                for k in range(25):
                    players[j].experience_replay()
            elif e > n_epochs*0.1:
                for k in range(5):
                    players[j].experience_replay()

        w = env.winner()
        print("EPOCH: {:03d}/{:03d} | WIN: player{:1d}".format(
                         e, n_epochs, w))

        if e%10 == 0:
            players[1].save_model(e)
