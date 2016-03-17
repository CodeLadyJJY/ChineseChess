#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    size = Director::getInstance()->getWinSize();

	// ����
	auto board = Sprite::createWithSpriteFrameName("background.png");
	board->setPosition(Vec2(size.width / 2, size.height / 2));
	board->setScale(0.5);
	this->addChild(board);

	// ��ʼ������
	int id = 0;
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 9; ++j)
			if (InitChessBoard[i][j] != NOCHESS)
			{
				newChessman(i, j, id++);
			}

	// ��ʼ������
	is_red = true;
	is_selected = false;
	red_king_id = 27;
	black_king_id = 4;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 9; j++)
		{
			map[i][j] = InitChessBoard[i][j];
		}

	// ѡ��ͼƬ1
	selected[0] = Sprite::createWithSpriteFrameName("selected1.png");
	selected[0]->setVisible(false);
	this->addChild(selected[0]);

	// ѡ��ͼƬ2
	selected[1] = Sprite::createWithSpriteFrameName("selected2.png");
	selected[1]->setVisible(false);
	this->addChild(selected[1]);

	// ��ť
	auto menuItemReStart = MenuItemFont::create("Restart", CC_CALLBACK_1(GameScene::menuRestartCallBack, this));
	menuItemReStart->setColor(Color3B::WHITE);

	auto menu = Menu::create(menuItemReStart, NULL);
	menu->setPosition(Vec2::ZERO);
	menuItemReStart->setPosition(size.width / 2, size.height / 8);
	this->addChild(menu);
    
	// ��������
	listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = [&](Touch * t, Event * e) {
		int x = (t->getLocation().x - BOARD_WIDTH + GRID_WIDTH / 2) / GRID_WIDTH;
		int y = (t->getLocation().y - BOARD_HEIGHT + GRID_WIDTH / 2) / GRID_WIDTH;

		// �������
		if (isClickOnChessman(t->getLocation().x, t->getLocation().y, x, y))
		{
			// �����ѡ���һ�����ӣ����ҵ�����ǶԷ����ӣ������ǺϷ��߲��������
			if (is_selected && isRedOrBlack(map[9 - y][x], is_red) && isValidMove(9 - y, x))
			{
				captureChessman();
				unSelectChessman(0);
				selectChessman(1);
				is_red = !is_red;
			}
			// ���������Ǽ������ӣ���ѡ������
			else if(isRedOrBlack(map[9 - y][x], !is_red))
			{
				moveStep.from.x = allChessman[selected_id]->m_x;
				moveStep.from.y = allChessman[selected_id]->m_y;
				unSelectChessman(1);
				selectChessman(0);
			}
		}
		// ����ڿհ״�
		else if (is_selected && isValidMove(9 - y, x))
		{
			moveChessman();
			unSelectChessman(0);
			selectChessman(1);
			is_red = !is_red;
		}

		if (isGameOver())
			gameOver();

		return true;
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

// ��������
void GameScene::newChessman(int x, int y, int id)
{
	auto pSprite = Chessman::create(InitChessBoard[x][y]);	
	pSprite->id = id;
	pSprite->setPositionRC(x, y);
	this->addChild(pSprite);
	allChessman[id] = pSprite;
}

// �ƶ�����
void GameScene::moveChessman()
{
	float d = sqrt(pow(moveStep.from.x - moveStep.to.x, 2) + pow(moveStep.from.y - moveStep.to.y, 2));

	allChessman[selected_id]->runAction(MoveTo::create(d / 8.0, Vec2(BOARD_WIDTH + GRID_WIDTH * moveStep.to.y, BOARD_HEIGHT + GRID_WIDTH * (9 - moveStep.to.x))));
	allChessman[selected_id]->setPos(moveStep.to.x, moveStep.to.y);

	map[moveStep.from.x][moveStep.from.y] = NOCHESS;
	map[moveStep.to.x][moveStep.to.y] = moveStep.id;
}

// ����
void GameScene::captureChessman()
{
	float d = sqrt(pow(moveStep.from.x - moveStep.to.x, 2) + pow(moveStep.from.y - moveStep.to.y, 2));

	allChessman[selected_id]->runAction(Sequence::create(MoveTo::create(d / 8.0, Vec2(allChessman[eaten_id]->p_x, allChessman[eaten_id]->p_y)), CCCallFunc::create([&] {allChessman[eaten_id]->removeFromParent();}), NULL));
	allChessman[selected_id]->setPos(moveStep.to.x, moveStep.to.y);
	allChessman[eaten_id]->isLive = false;
	map[moveStep.to.x][moveStep.to.y] = moveStep.id;
	map[moveStep.from.x][moveStep.from.y] = NOCHESS;
}

// �ж�һ�������Ǻ�ɫ���Ǻ�ɫ, 0�жϺ�ɫ, 1�жϺ�ɫ
bool GameScene::isRedOrBlack(int x, int type)
{
	if (type && IsBlack(x))
		return true;
	else if (!type && IsRed(x))
		return true;

	return false;
}

// �ж�һ�����ӵ���λ�Ƿ�Ϸ�
bool GameScene::isValidMove(int toX, int toY)
{
	int fromX = allChessman[selected_id]->m_x;
	int fromY = allChessman[selected_id]->m_y;
	int moveChessID = map[fromX][fromY];
	int targetID = map[toX][toY];

	// ͬһλ�ã��Ƿ�
	if (fromX == toX && fromY == toY)
		return false;
	// ͬɫ���Ƿ�
	if (IsSameSide(moveChessID, targetID))
		return false;

	switch (moveChessID)
	{
	case B_KING:
		// �ھŹ����⣬�Ƿ�
		if (toX < 0 || toX > 2 || toY < 3 || toY > 5)
			return false;
		// ����һ��ֱ��
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case R_KING:
		// �ھŹ����⣬�Ƿ�
		if (toX < 7 || toX > 9 || toY < 3 || toY > 5)
			return false;
		// ����һ��ֱ��
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case B_BISHOP:
		// �ھŹ����⣬�Ƿ�
		if (toX < 0 || toX > 2 || toY < 3 || toY > 5)
			return false;
		// ����һ��б��
		if (abs(fromX - toX) != 1 || abs(fromY - toY) != 1)
			return false;
		break;
	case R_BISHOP:
		// �ھŹ����⣬�Ƿ�
		if (toX < 7 || toX > 9 || toY < 3 || toY > 5)
			return false;
		// ����һ��б��
		if (abs(fromX - toX) != 1 || abs(fromY - toY) != 1)
			return false;
		break;
	case B_ELEPHANT:
		// ���ܹ���
		if (toX > 4)
			return false;
		// ��������
		if (abs(fromX - toX) != 2 || abs(fromY - toY) != 2)
			return false;
		// ���۱���ס
		if (map[(fromX + toX) / 2][(fromY + toY) / 2] != NOCHESS)
			return false;
		break;
	case R_ELEPHANT:
		// ���ܹ���
		if (toX < 5)
			return false;
		// ��������
		if (abs(fromX - toX) != 2 || abs(fromY - toY) != 2)
			return false;
		// ���۱���ס
		if (map[(fromX + toX) / 2][(fromY + toY) / 2] != NOCHESS)
			return false;
		break;
	case B_PAWN:
		// ������ͷ
		if (toX < fromX)
			return false;
		// ������ǰֻ��ֱ��
		if (fromX < 5 && fromX == toX)
			return false;
		// ����һ��ֱ��
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case R_PAWN:
		// ������ͷ
		if (toX > fromX)
			return false;
		// ������ǰֻ��ֱ��
		if (fromX > 4 && fromX == toX)
			return false;
		// ����һ��ֱ��
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case B_CAR:
	case R_CAR:
		// ����ֱ��
		if (fromX != toX && fromY != toY)
			return false;
		// ֱ���м���������
		if (fromX == toX)
		{
			int t1 = fromY < toY ? fromY : toY;
			int t2 = fromY > toY ? fromY : toY;
			for (int i = t1 + 1; i < t2; ++i)
				if (map[fromX][i] != NOCHESS)
					return false;
		}
		else
		{
			int t1 = fromX < toX ? fromX : toX;
			int t2 = fromX > toX ? fromX : toX;
			for (int i = t1 + 1; i < t2; ++i)
				if (map[i][fromY] != NOCHESS)
					return false;
		}
		break;
	case B_HORSE:
	case R_HORSE:
		// ������
		if (!((abs(fromX - toX) == 1 && abs(fromY - toY) == 2) || (abs(fromX - toX) == 2 && abs(fromY - toY) == 1)))
			return false;
		// ������
		int i, j;
		if (fromX - toX == 2)
		{
			i = fromX - 1;
			j = fromY;
		}
		else if(fromX - toX == -2)
		{
			i = fromX + 1;
			j = fromY;
		}
		else if (fromY - toY == 2)
		{
			i = fromX;
			j = fromY - 1;
		}
		else if (fromY - toY == -2)
		{
			i = fromX;
			j = fromY + 1;
		}
		if (map[i][j] != NOCHESS)
			return false;
		break;
	case B_CANON:
	case R_CANON:
		// ����ֱ��
		if (abs(fromX - toX) != 0 && abs(fromY - toY) != 0)
			return false;
		// �ڲ�����ʱ���м�����
		if (map[toX][toY] == NOCHESS)
		{
			if (fromX == toX)
			{
				int t1 = fromY < toY ? fromY : toY;
				int t2 = fromY > toY ? fromY : toY;
				for (int i = t1 + 1; i < t2; ++i)
					if (map[fromX][i] != NOCHESS)
						return false;
			}
			else
			{
				int t1 = fromX < toX ? fromX : toX;
				int t2 = fromX > toX ? fromX : toX;
				for (int i = t1 + 1; i < t2; ++i)
					if (map[i][fromY] != NOCHESS)
						return false;
			}
		}
		// �ڳ���ʱ���м���һ��
		else
		{
			int count = 0;
			if (fromX == toX)
			{
				int t1 = fromY < toY ? fromY : toY;
				int t2 = fromY > toY ? fromY : toY;
				for (int i = t1 + 1; i < t2; ++i)
					if (map[fromX][i] != NOCHESS)
						count++;
			}
			else
			{
				int t1 = fromX < toX ? fromX : toX;
				int t2 = fromX > toX ? fromX : toX;
				for (int i = t1 + 1; i < t2; ++i)
					if (map[i][fromY] != NOCHESS)
						count++;
			}
			if (count != 1)
				return false;
		}
		break;
	default:
		return false;
	}

	// �������棬�Ƿ�
	// ������ͬ��
	if (allChessman[black_king_id]->m_y == allChessman[red_king_id]->m_y)
	{
		int count = 0;
		for (int i = allChessman[black_king_id]->m_x + 1; i < allChessman[red_king_id]->m_x; ++i)
			if (map[i][allChessman[black_king_id]->m_y] != NOCHESS)
				count++;

		// �м�ֻ�� 1 �����,������ 1 �ӽ�Ҫ�ƿ�����Ƿ�
		if (count == 1 && fromY == allChessman[black_king_id]->m_y && toY != allChessman[black_king_id]->m_y)
			return false;
	}

	moveStep.id = moveChessID;
	moveStep.to.x = toX;
	moveStep.to.y = toY;

	return true;
}

// �Ƿ�����������
bool GameScene::isClickOnChessman(int p_x, int p_y, int x, int y)
{
	for (int i = 0; i < 32; ++i)
	{
		//�жϵ��λ���Ƿ���Բ��
		int r = sqrt(pow(abs(p_x - allChessman[i]->p_x), 2) + pow(abs(p_y - allChessman[i]->p_y), 2));
		if (allChessman[i]->isLive && r <= GRID_WIDTH / 2)
		{
			if (!is_selected || isRedOrBlack(map[9 - y][x], !is_red))
				selected_id = allChessman[i]->id;
			else
				eaten_id = allChessman[i]->id;
			return true;
		}
	}

	return false;
}

// �Ƿ����ڿհ״�
//bool GameScene::isClickOnEmpty(int x, int y)
//{
//	int xx = (x - BOARD_WIDTH + GRID_WIDTH / 2) / GRID_WIDTH;
//	int yy = (y - BOARD_HEIGHT + GRID_WIDTH / 2) / GRID_WIDTH;
//
//	if (map[9 - yy][xx] == NOCHESS)
//	{
//		moveStep.id = selected_id;
//		moveStep.to.x = 9 - yy;
//		moveStep.to.y = xx;
//		return true;
//	}
//
//	return false;
//}

// ѡ������
void GameScene::selectChessman(int type)
{
	selected[type]->setPosition(Vec2(allChessman[selected_id]->p_x, allChessman[selected_id]->p_y));
	selected[type]->setVisible(true);
	if(type == 0)
		is_selected = true;
}

// ȡ��ѡ������
void GameScene::unSelectChessman(int type)
{
	selected[type]->setVisible(false);
	if (type == 0)
		is_selected = false;
}

// ��Ϸ�Ƿ����
bool GameScene::isGameOver()
{
	if (!allChessman[red_king_id]->isLive || !allChessman[black_king_id]->isLive)
		return true;

	return false;
}

// ��Ϸ����
void GameScene::gameOver()
{
	Label *gameover;
	if (allChessman[red_king_id]->isLive)
		gameover = Label::createWithSystemFont("Red Win", "Arial", 130);
	else
		gameover = Label::createWithSystemFont("Black Win", "Arial", 130);
	gameover->setPosition(size / 2);
	this->addChild(gameover);

	// ɾ����������
	_eventDispatcher->removeEventListener(listener);
}

void GameScene::menuRestartCallBack(Ref* pSender)
{
	auto scene = GameScene::createScene();
	TransitionScene *transition = TransitionFade::create(2, scene);
	Director::getInstance()->replaceScene(transition);
}