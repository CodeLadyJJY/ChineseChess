#include "GameScene.h"

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
    
	initUI();
	initListener();
	
    return true;
}

// 初始化界面
void GameScene::initUI()
{
	size = Director::getInstance()->getWinSize();

	// 棋盘
	board = Node::create();
	board->setPosition(Vec2(size.width / 2, size.height / 2));
	this->addChild(board);

	auto boardSprite = Sprite::createWithSpriteFrameName("background.png");
	boardSprite->setPosition(Vec2(size.width / 2, size.height / 2));
	boardSprite->setScale(0.5);
	this->addChild(boardSprite, -1);

	// 初始化棋子
	int id = 0;
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 9; ++j)
			if (InitChessBoard[i][j] != NOCHESS)
			{
				newChessman(i, j, id++);
			}

	// 初始化数据
	color = 1;
	is_selected = false;
	red_king_id = 27;
	black_king_id = 4;
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 9; j++)
		{
			map[i][j] = InitChessBoard[i][j];
		}

	// 提示下子图片1
	curChessman[0] = Sprite::createWithSpriteFrameName("1.png");
	curChessman[0]->setPosition(Vec2(size.width / 2, size.height * 9 / 10));
	curChessman[0]->setVisible(false);
	this->addChild(curChessman[0]);
	// 提示下子图片2
	curChessman[1] = Sprite::createWithSpriteFrameName("8.png");
	curChessman[1]->setPosition(Vec2(size.width / 2, size.height * 9 / 10));
	curChessman[1]->setVisible(false);
	this->addChild(curChessman[1]);

	// 当前下子方图片
	curChessman[1]->setVisible(true);

	// 选择图片1
	selected[0] = Sprite::createWithSpriteFrameName("selected1.png");
	selected[0]->setVisible(false);
	board->addChild(selected[0]);

	// 选择图片2
	selected[1] = Sprite::createWithSpriteFrameName("selected2.png");
	selected[1]->setVisible(false);
	board->addChild(selected[1]);

	// 按钮
	auto menuItemReStart = MenuItemFont::create("Restart", CC_CALLBACK_1(GameScene::menuRestartCallBack, this));
	menuItemReStart->setColor(Color3B::WHITE);

	auto menu = Menu::create(menuItemReStart, NULL);
	menu->setPosition(Vec2::ZERO);
	menuItemReStart->setPosition(size.width / 2, size.height / 8);
	this->addChild(menu);
}

// 初始化监听
void GameScene::initListener()
{
	// 触摸侦听
	auto listener = EventListenerTouchOneByOne::create();

	listener->onTouchBegan = [&](Touch * t, Event * e) {
		auto p = board->convertTouchToNodeSpace(t);
		int x = (p.x + BOARD_WIDTH / 2 + GRID_WIDTH / 2) / GRID_WIDTH;
		int y = (p.y + BOARD_HEIGHT / 2 + GRID_WIDTH / 2) / GRID_WIDTH;

		// 点击棋子
		if (isClickOnChessman(p.x, p.y, x, y))
		{
			// 如果已选择第一个棋子，并且点击的是对方棋子，并且是合法走步，则吃子
			if (is_selected && isRedOrBlack(map[9 - y][x], color) && isValidMove(9 - y, x))
			{
				captureChessman();
				unSelectChessman(0);
				selectChessman(1);
				color = !color;
				setCurChessman(color);
			}
			// 如果点击的是己方棋子，则选择棋子
			else if (isRedOrBlack(map[9 - y][x], !color))
			{
				moveStep.from.x = allChessman[moveStep.id]->m_x;
				moveStep.from.y = allChessman[moveStep.id]->m_y;
				unSelectChessman(1);
				selectChessman(0);
			}
		}
		// 点击在空白处
		else if (is_selected && isValidMove(9 - y, x))
		{
			moveChessman();
			unSelectChessman(0);
			selectChessman(1);
			color = !color;
			setCurChessman(color);
		}

		if (isGameOver())
			gameOver();

		return true;
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

// 创建新子
void GameScene::newChessman(int x, int y, int id)
{
	auto pSprite = Chessman::create(InitChessBoard[x][y]);	
	pSprite->id = id;
	pSprite->setPositionRC(x, y);
	board->addChild(pSprite);
	allChessman[id] = pSprite;
}

// 移动棋子
void GameScene::moveChessman()
{
	float d = sqrt(pow(moveStep.from.x - moveStep.to.x, 2) + pow(moveStep.from.y - moveStep.to.y, 2));

	allChessman[moveStep.id]->runAction(MoveBy::create(d / 8.0, GRID_WIDTH * Vec2(moveStep.to.y - moveStep.from.y, moveStep.from.x - moveStep.to.x)));
	allChessman[moveStep.id]->setPos(moveStep.to.x, moveStep.to.y);

	map[moveStep.from.x][moveStep.from.y] = NOCHESS;
	map[moveStep.to.x][moveStep.to.y] = moveStep.type;
}

// 吃子
void GameScene::captureChessman()
{
	float d = sqrt(pow(moveStep.from.x - moveStep.to.x, 2) + pow(moveStep.from.y - moveStep.to.y, 2));

	allChessman[moveStep.id]->runAction(Sequence::create(MoveBy::create(d / 8.0, GRID_WIDTH * Vec2(moveStep.to.y - moveStep.from.y, moveStep.from.x - moveStep.to.x)), CCCallFunc::create([&] {allChessman[moveStep.eaten_id]->removeFromParent();}), NULL));
	allChessman[moveStep.id]->setPos(moveStep.to.x, moveStep.to.y);

	allChessman[moveStep.eaten_id]->isLive = false;
	map[moveStep.to.x][moveStep.to.y] = moveStep.type;
	map[moveStep.from.x][moveStep.from.y] = NOCHESS;
}

// 判断一个棋子是红色还是黑色, 0判断红色, 1判断黑色
bool GameScene::isRedOrBlack(int x, int type)
{
	if (type && IsBlack(x))
		return true;
	else if (!type && IsRed(x))
		return true;

	return false;
}

// 判断一个棋子的走位是否合法
bool GameScene::isValidMove(int toX, int toY)
{
	int fromX = allChessman[moveStep.id]->m_x;
	int fromY = allChessman[moveStep.id]->m_y;
	int moveChessID = map[fromX][fromY];
	int targetID = map[toX][toY];

	// 同一位置，非法
	if (fromX == toX && fromY == toY)
		return false;
	// 同色，非法
	if (IsSameSide(moveChessID, targetID))
		return false;

	switch (moveChessID)
	{
	case B_KING:
		// 在九宫格外，非法
		if (toX < 0 || toX > 2 || toY < 3 || toY > 5)
			return false;
		// 将走一步直线
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case R_KING:
		// 在九宫格外，非法
		if (toX < 7 || toX > 9 || toY < 3 || toY > 5)
			return false;
		// 将走一步直线
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case B_BISHOP:
		// 在九宫格外，非法
		if (toX < 0 || toX > 2 || toY < 3 || toY > 5)
			return false;
		// 仕走一步斜线
		if (abs(fromX - toX) != 1 || abs(fromY - toY) != 1)
			return false;
		break;
	case R_BISHOP:
		// 在九宫格外，非法
		if (toX < 7 || toX > 9 || toY < 3 || toY > 5)
			return false;
		// 仕走一步斜线
		if (abs(fromX - toX) != 1 || abs(fromY - toY) != 1)
			return false;
		break;
	case B_ELEPHANT:
		// 象不能过河
		if (toX > 4)
			return false;
		// 象走田字
		if (abs(fromX - toX) != 2 || abs(fromY - toY) != 2)
			return false;
		// 象眼被塞住
		if (map[(fromX + toX) / 2][(fromY + toY) / 2] != NOCHESS)
			return false;
		break;
	case R_ELEPHANT:
		// 象不能过河
		if (toX < 5)
			return false;
		// 象走田字
		if (abs(fromX - toX) != 2 || abs(fromY - toY) != 2)
			return false;
		// 象眼被塞住
		if (map[(fromX + toX) / 2][(fromY + toY) / 2] != NOCHESS)
			return false;
		break;
	case B_PAWN:
		// 兵不回头
		if (toX < fromX)
			return false;
		// 兵过河前只能直走
		if (fromX < 5 && fromX == toX)
			return false;
		// 兵走一步直线
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case R_PAWN:
		// 兵不回头
		if (toX > fromX)
			return false;
		// 兵过河前只能直走
		if (fromX > 4 && fromX == toX)
			return false;
		// 兵走一步直线
		if (abs(fromX - toX) + abs(fromY - toY) > 1)
			return false;
		break;
	case B_CAR:
	case R_CAR:
		// 车走直线
		if (fromX != toX && fromY != toY)
			return false;
		// 直线中间无子阻拦
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
		// 马走日
		if (!((abs(fromX - toX) == 1 && abs(fromY - toY) == 2) || (abs(fromX - toX) == 2 && abs(fromY - toY) == 1)))
			return false;
		// 蹩马腿
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
		// 炮走直线
		if (abs(fromX - toX) != 0 && abs(fromY - toY) != 0)
			return false;
		// 炮不吃子时，中间无子
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
		// 炮吃子时，中间有一子
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

	// 将军本在不同列，移动将军后同列且中间无子，见面，非法
	if ((moveChessID == B_KING && toY == allChessman[red_king_id]->m_y) || (moveChessID == R_KING && toY == allChessman[black_king_id]->m_y))
	{
		int count = 0;
		for (int i = allChessman[black_king_id]->m_x + 1; i < allChessman[red_king_id]->m_x; ++i)
		{
			if (map[i][toY] != NOCHESS)
				count++;
		}

		if (count == 0)
			return false;
	}
	// 将军本在同列，中间只剩一子，并且该子将要移开，造成将军见面
	if (allChessman[black_king_id]->m_y == allChessman[red_king_id]->m_y)
	{
		int count = 0;
		for (int i = allChessman[black_king_id]->m_x + 1; i < allChessman[red_king_id]->m_x; ++i)
		{
			if (map[i][allChessman[black_king_id]->m_y] != NOCHESS)
				count++;
		}

		// 中间只有 1 子相隔,并且这 1 子将要移开，则非法
		if (count == 1 && fromY == allChessman[black_king_id]->m_y && toY != allChessman[black_king_id]->m_y)
			return false;
	}

	moveStep.type = moveChessID;
	moveStep.to.x = toX;
	moveStep.to.y = toY;

	return true;
}

// 是否点击在棋子上
bool GameScene::isClickOnChessman(int p_x, int p_y, int x, int y)
{
	for (int i = 0; i < 32; ++i)
	{
		//判断点击位置是否在圆内
		int r = sqrt(pow(abs(p_x - allChessman[i]->p_x), 2) + pow(abs(p_y - allChessman[i]->p_y), 2));
		if (allChessman[i]->isLive && r <= GRID_WIDTH / 2)
		{
			if (!is_selected || isRedOrBlack(map[9 - y][x], !color))
				moveStep.id = allChessman[i]->id;
			else
				moveStep.eaten_id = allChessman[i]->id;
			return true;
		}
	}

	return false;
}

// 设置提示图片
void GameScene::setCurChessman(int type)
{
	curChessman[type]->setVisible(true);
	curChessman[!type]->setVisible(false);
}

// 选择棋子
void GameScene::selectChessman(int type)
{
	selected[type]->setPosition(Vec2(allChessman[moveStep.id]->p_x, allChessman[moveStep.id]->p_y));
	selected[type]->setVisible(true);
	if(type == 0)
		is_selected = true;
}

// 取消选择棋子
void GameScene::unSelectChessman(int type)
{
	selected[type]->setVisible(false);
	if (type == 0)
		is_selected = false;
}

// 游戏是否结束
bool GameScene::isGameOver()
{
	if (!allChessman[red_king_id]->isLive || !allChessman[black_king_id]->isLive)
		return true;

	return false;
}

// 游戏结束
void GameScene::gameOver()
{
	Label *gameover;
	if (allChessman[red_king_id]->isLive)
		gameover = Label::createWithSystemFont("Red Win", "Arial", 130);
	else
		gameover = Label::createWithSystemFont("Black Win", "Arial", 130);
	gameover->setPosition(size / 2);
	this->addChild(gameover);

	// 删除触摸监听
	_eventDispatcher->removeAllEventListeners();
}

void GameScene::menuRestartCallBack(Ref* pSender)
{
	auto scene = GameScene::createScene();
	TransitionScene *transition = TransitionFade::create(2, scene);
	Director::getInstance()->replaceScene(transition);
}
