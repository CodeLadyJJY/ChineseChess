#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Chessman.h"
#include "define.h"

class GameScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
	CREATE_FUNC(GameScene);
    void menuRestartCallBack(cocos2d::Ref* pSender);

private:
	cocos2d::Size size;								// 窗口大小
	cocos2d::Sprite* selected[2];					// 选择图片, 0 是红色选择框，1 是蓝色选择框
	cocos2d::EventListenerTouchOneByOne* listener;	// 触摸监听

	int map[10][9];							// 棋盘地图
	Chessman *allChessman[32];				// 所有棋子
	bool is_red;							// 是否是红方走步
	bool is_selected;						// 是否有选择棋子
	int selected_id;						// 被选棋子id
	int eaten_id;							// 被吃棋子id
	int red_king_id;						// 红方将ID
	int black_king_id;						// 黑方将ID
	ChessMove moveStep;						// 移动步骤

	void newChessman(int x, int y, int id);					// 新建棋子
	void moveChessman();									// 移动棋子
	void captureChessman();									// 吃子
	bool isRedOrBlack(int x, int type);						// 判断一个棋子是红色还是黑色, 0判断红色, 1判断黑色
	bool isValidMove(int toX, int toY);						// 判断一个棋子的走位是否合法
	bool isClickOnChessman(int p_x, int p_y, int x, int y);	// 是否点击在棋子上
	void selectChessman(int type);							// 选择棋子
	void unSelectChessman(int type);						// 取消选择棋子
	bool isGameOver();										// 游戏是否结束
	void gameOver();										// 游戏结束
};

#endif // __GAME_SCENE_H__
