#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Chessman.h"
#include "define.h"

USING_NS_CC;

class GameScene : public Layer
{
public:
    static Scene* createScene();
    virtual bool init();
	CREATE_FUNC(GameScene);
    void menuRestartCallBack(Ref* pSender);

private:
	Size size;								// 窗口大小
	Sprite* selected[2];					// 选择图片, 0 是红色选择框，1 是蓝色选择框
	Sprite* curChessman[2];					// 提示当前下子方
	Node* board;							// 棋盘节点
	void initUI();							// 初始化界面
	void initListener();					// 初始化监听

	int map[10][9];							// 棋盘地图
	Chessman *allChessman[32];				// 所有棋子
	bool color;								// 玩家颜色，0 黑色，1 红色
	bool is_selected;						// 是否有选择棋子
	int red_king_id;						// 红方将ID
	int black_king_id;						// 黑方将ID
	ChessMove moveStep;						// 移动步骤

	void newChessman(int x, int y, int id);					// 新建棋子
	void moveChessman();									// 移动棋子
	void captureChessman();									// 吃子
	bool isRedOrBlack(int x, int type);						// 判断一个棋子是红色还是黑色, 0判断红色, 1判断黑色
	bool isValidMove(int toX, int toY);						// 判断一个棋子的走位是否合法
	bool isClickOnChessman(int g_x, int g_y, int x, int y);	// 是否点击在棋子上
	void setCurChessman(int type);							// 设置提示图片
	void selectChessman(int type);							// 选择棋子
	void unSelectChessman(int type);						// 取消选择棋子
	bool isGameOver();										// 游戏是否结束
	void gameOver();										// 游戏结束
};

#endif // __GAME_SCENE_H__
