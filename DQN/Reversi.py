import os
import numpy as np
class Reversi:

    def __init__(self):
        # parameters
        self.name = os.path.splitext(os.path.basename(__file__))[0]
        self.Blank = 0
        self.Black = 1
        self.White = 2
        self.screen_n_rows = 8
        self.screen_n_cols = 8
        self.enable_actions = np.arange(self.screen_n_rows*self.screen_n_cols)
        # variables
        self.reset()

    # 棋盘初始化
    def reset(self):
        # reset ball position
        self.screen = np.zeros((self.screen_n_rows, self.screen_n_cols))
        self.set_cells(27, self.White)
        self.set_cells(28, self.Black)
        self.set_cells(35, self.Black)
        self.set_cells(36, self.White)

    def get_cells(self, i):
        r = int(i / self.screen_n_cols)
        c = int(i - ( r * self.screen_n_cols))
        return self.screen[r][c]


    def set_cells(self, i, value):
        r = int(i / self.screen_n_cols)
        c = int(i - ( r * self.screen_n_cols))
        self.screen[r][c] = value


    def print_screen(self):
        i = 0
        for r in range(self.screen_n_rows):
            s1 = ''
            for c in range(self.screen_n_cols):
                s2 = ''
                if self.screen[r][c] == self.Blank:
                    s2 = '{0:2d}'.format(self.enable_actions[i])
                elif self.screen[r][c] == self.Black:
                    s2 = ' ●'
                elif self.screen[r][c] == self.White:
                    s2 = ' ○'
                s1 = s1 + ' ' + s2
                i += 1
            print(s1)
    
    # 在action位置着子color，返回着子后可翻转棋子的数量，puton控制是否更新棋盘
    def put_piece(self, action, color, puton=True): 
        if self.get_cells(action) != self.Blank:
            return -1

        """ ---------------------------------------------------------
           纵横斜的8条，因为是一维数据，所以
           从现在的位置[ - 9，- 8，- 7，- 1，1，7，8，9 ]
           看偏离的方向。
           这是用[ - 1，0，1 ]和[ - 8，0，8 ]的组合进行调查
          （0和0的对除外）。
        """
        t, x, y, l = 0, action%8, action//8, []
        for di, fi in zip([-1, 0, 1], [x, 7, 7-x]):
            for dj, fj in zip([-8, 0, 8], [y, 7, 7-y]):
                if not di == dj == 0:
                    b, j, k, m, n =[], 0, 0, [], 0
                    a = self.enable_actions[action+di+dj::di+dj][:min(fi, fj)]
                    for i in a:
                        b.append(self.get_cells(i))
                    for i in b:
                        if i == 0: #空白
                            break
                        elif i == color:
                            n = k
                            l += m
                            break
                        else:
                            k += 1
                            m.insert(0, a[j])
                        j += 1
                    #print("n={:}".format(n))
                    t += n

        if t == 0: # 着子后可翻棋子数量为0
            return 0

        if puton: 
            # 将对手的棋子翻转
            for i in l: # l保存了可翻转的对手的棋子
                self.set_cells(i, color)
            # 添加这一步放置的棋子
            self.set_cells(action, color)
        
        return t # 返回着子后可翻转棋子数量

    # 判断胜者
    def winner(self):
        # 清点棋子数量，判断胜利的一方
        Black_score = self.get_score(self.Black)
        White_score = self.get_score(self.White)

        if Black_score == White_score:
            return 0 
        elif Black_score > White_score:
            return self.Black 
        elif Black_score < White_score:
            return self.White 

    # 获取分数，即当前颜色棋子数量
    def get_score(self, color):
        # 返回指定颜色的当前分数(棋子数量)
        score = 0
        for i in self.enable_actions:
            if self.get_cells(i) == color:
                score += 1
        return score

    # 获取当前color可着子点(列表list)
    def get_enables(self, color): # 获得可着子点
        result = []
        for action in self.enable_actions:
            if self.get_cells(action) == self.Blank: # 如果是空白位置
                if self.put_piece(action, color, False) > 0: # 测试是否可以着子
                    result.insert(0, action) 
        return result


    # 更新棋盘
    def update(self, action, color): # 
        # 若action位置可以着子color，则着子，同时返回翻转棋子的个数
        n = self.put_piece(action, color, False)
        if n  > 0:
            self.put_piece(action, color)
        return n

    # 判断棋局是否结束
    def isEnd(self):
        e1 = self.get_enables(self.Black)
        e2 = self.get_enables(self.White)
        if len(e1) == 0 and len(e2) == 0:
            return True
        for action in self.enable_actions:
            if self.get_cells(action) == self.Blank:
                return False
        return True