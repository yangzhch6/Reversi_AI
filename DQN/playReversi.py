
import argparse

from Reversi import Reversi
from Agent import DQNAgent


if __name__ == "__main__":
    # args
    parser = argparse.ArgumentParser()
    parser.add_argument("-m", "--model_path")
    parser.add_argument("-s", "--save", dest="save", action="store_true")
    parser.set_defaults(save=False)
    args = parser.parse_args()

    # environmet, agent
    env = Reversi()
    agent = DQNAgent(env.enable_actions, env.name, env.screen_n_rows, env.screen_n_cols)
    
    # game
    print("------------- GAME START ---------------")
    print("请玩家选择棋子：白棋先手-1，黑棋后手-2：")
    choose = 0
    while(choose != 1 and choose != 2): 
        tmp = input('>>>')
        choose = int(tmp)
        if(choose != 1 and choose != 2):
            print("输入错误，请重新输入")

    if choose == 1:# 如果玩家选择了 白旗-1 先手 
        while not env.isEnd():
            print("***玩家下棋***")
            env.print_screen()
            enables = env.get_enables(1) # 获取可着子点列表
            if len(enables) > 0:
                flg = False
                while not flg:
                    print("输入着子点")
                    print(enables)
                    inp = ''
                    while inp == '':
                        inp = input('>>>  ')
                    action_t = int(inp)
                    for i in enables: 
                        if action_t == i: # 若action_t可以着子
                            flg = True
                            break

                env.update(action_t, 1)
            else:
                print("无子科下，转换下棋方")

            if env.isEnd() == True:break

            print("***AI下棋***")
            env.print_screen()
            enables = env.get_enables(2)
            if len(enables) > 0:
                qvalue, action_t = agent.select_enable_action(env.screen, enables)
                print('>>>  {:}'.format(action_t))
                env.update(action_t, 2)
            else:
                print("无子科下，转换下棋方")

    else:
        while not env.isEnd():
            print("***AI下棋***")
            env.print_screen()
            enables = env.get_enables(1)
            if len(enables) > 0:
                qvalue, action_t = agent.select_enable_action(env.screen, enables)
                print('>>>  {:}'.format(action_t))
                env.update(action_t, 1)
            else:
                print("无子科下，转换下棋方")

            if env.isEnd() == True:break

            print("***玩家下棋***")
            env.print_screen()
            enables = env.get_enables(2) # 获取可着子点列表
            if len(enables) > 0:
                flg = False
                while not flg:
                    print("输入着子点")
                    print(enables)
                    inp = ''
                    while inp == '':
                        inp = input('>>>  ')
                    action_t = int(inp)
                    for i in enables: 
                        if action_t == i: # 若action_t可以着子
                            flg = True
                            break
                env.update(action_t, 2)
            else:
                print("无子可下，转换下棋方")

    print("*** 游戏结束 ***")
    if env.winner() == 1:
        print("你赢了-- {:}".format(env.get_score(1)))
    else:
        print("你输了-- {:}".format(env.get_score(2)))
