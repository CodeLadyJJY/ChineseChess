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
	cocos2d::Size size;								// ���ڴ�С
	cocos2d::Sprite* selected[2];					// ѡ��ͼƬ, 0 �Ǻ�ɫѡ���1 ����ɫѡ���
	cocos2d::EventListenerTouchOneByOne* listener;	// ��������

	int map[10][9];							// ���̵�ͼ
	Chessman *allChessman[32];				// ��������
	bool is_red;							// �Ƿ��Ǻ췽�߲�
	bool is_selected;						// �Ƿ���ѡ������
	int selected_id;						// ��ѡ����id
	int eaten_id;							// ��������id
	int red_king_id;						// �췽��ID
	int black_king_id;						// �ڷ���ID
	ChessMove moveStep;						// �ƶ�����

	void newChessman(int x, int y, int id);					// �½�����
	void moveChessman();									// �ƶ�����
	void captureChessman();									// ����
	bool isRedOrBlack(int x, int type);						// �ж�һ�������Ǻ�ɫ���Ǻ�ɫ, 0�жϺ�ɫ, 1�жϺ�ɫ
	bool isValidMove(int toX, int toY);						// �ж�һ�����ӵ���λ�Ƿ�Ϸ�
	bool isClickOnChessman(int p_x, int p_y, int x, int y);	// �Ƿ�����������
	void selectChessman(int type);							// ѡ������
	void unSelectChessman(int type);						// ȡ��ѡ������
	bool isGameOver();										// ��Ϸ�Ƿ����
	void gameOver();										// ��Ϸ����
};

#endif // __GAME_SCENE_H__
