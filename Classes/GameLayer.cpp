#include "GameLayer.h"
#include "SimpleAudioEngine.h"

#define GOAL_LEFT 0.26f
#define GOAL_RIGHT 0.74f

GameLayer::GameLayer()
{
    _player1 = nullptr;
    _player2 = nullptr;
    _ball = nullptr;
    _players = Vector<GameSprite*>();
    _player1ScoreLabel = nullptr;
    _player2ScoreLabel = nullptr;
    _player1Score = 0;
    _player2Score = 0;
}

GameLayer::~GameLayer()
{
}

bool GameLayer::init()
{
    if (!Layer::init())
    {
        return false;
    }
    _players = Vector<GameSprite*>(2);
    _player1Score = 0;
    _player2Score = 0;
    _screenSize = Director::getInstance()->getWinSize();
    this->addBackgroud();
    this->addPlayers();
    this->addBall();
    this->addScoreLabels();
    this->addEventListener();
    this->scheduleUpdate();
    return true;
}

GameLayer* GameLayer::create()
{
    auto layer = new(std::nothrow) GameLayer();
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    delete layer;
    layer = nullptr;
    return nullptr;
}

void GameLayer::onTouchesBegan(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch : touches)
    {
        if (touch)
        {
            const auto tap = touch->getLocation();
            for (auto player : _players)
            {
                if (player && player->getBoundingBox().containsPoint(tap))
                {
                    log("touch on player");
                    player->setTouch(touch);
                }
            }
        }
    }
}

void GameLayer::onTouchesMoved(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch : touches)
    {
        if (touch)
        {
            const auto tap = touch->getLocation();
            for (auto player : _players)
            {
                // if this touch belongs to the player who is holding touch
                if (player && player->getTouch() == touch)
                {
                    // then, calculate the next position of the player
                    // nextPosition will be used in update() to move the player
                    auto nextPosition = tap;
                    
                    // keep nextPosition.x in range (minX, maxX)
                    auto minX = player->getRadius();
                    auto maxX = _screenSize.width - player->getRadius();
                    nextPosition.x = nextPosition.x < minX ? minX : nextPosition.x;
                    nextPosition.x = nextPosition.x > maxX ? maxX : nextPosition.x;

                    // keep nextPosition.y in range (minY, maxY)
                    const auto midY = _screenSize.height* 0.5f;
                    auto minY = player->getPositionY() < midY ? player->getRadius() : midY + player->getRadius();
                    auto maxY = player->getPositionY() < midY ? midY - player->getRadius() : _screenSize.height - player->getRadius();
                    nextPosition.y = nextPosition.y < minY ? minY : nextPosition.y;
                    nextPosition.y = nextPosition.y > maxY ? maxY : nextPosition.y;

                    // update next position and moving vector of player
                    player->setNextPosition(nextPosition);
                    player->setMovingVector(Vec2(tap.x - player->getPositionX(), tap.y - player->getPositionY()));
                }
            }
        }
    }
}

void GameLayer::onTouchesEnded(const std::vector<Touch*>& touches, Event* event)
{
    for (auto touch : touches)
    {
        if (touch)
        {
            const auto tap = touch->getLocation();
            for (auto player : _players)
            {
                // clear touch belongs to player
                if (player && player->getTouch() == touch)
                {
                    player->setTouch(nullptr);
                    player->setMovingVector(Vec2(0, 0));
                    // no need to set nextPosition, it will be updated when we call setPosition()
                }
            }
        }
    }
}

void GameLayer::update(float dt)
{
    // Range of position, keep the ball in this range
    const auto minX = _ball->getRadius();
    const auto maxX = _screenSize.width - _ball->getRadius();
    const auto minY = _ball->getRadius();
    const auto maxY = _screenSize.height - _ball->getRadius();

    // if the ball is in goal 1
    if (this->isGoal())
    {
        auto whichCourt = this->whichCourt();
        if (whichCourt == 1)
        {
            CCLOG("GOAL for player 2");
            this->increaseScore(2);
        }
        else
        {
            CCLOG("GOAL for player 1");
            this->increaseScore(1);
        }
        this->resetGame();
        return;
    }

    auto ballNextPosition = _ball->getNextPosition();
    auto ballMovingVector = _ball->getMovingVector();
    //CCLOG("ballMovingVector: %f %f",ballMovingVector.x, ballMovingVector.y);
    ballMovingVector *= 1.0f;
    ballNextPosition.x += ballMovingVector.x;
    ballNextPosition.y += ballMovingVector.y;

    // check collision between ball and players
    const auto sumSquareRadius = std::pow(_player1->getRadius() + _ball->getRadius(), 2);
    for (auto player : _players)
    {
        auto playerNextPosition = player->getNextPosition();
        auto playerMovingVector = player->getMovingVector();
        auto dx = ballNextPosition.x - playerNextPosition.x;
        auto dy = ballNextPosition.y - playerNextPosition.y;
        auto dist = std::pow(dx, 2) + std::pow(dy, 2);
        
        if (dist <= sumSquareRadius)
        {
            CCLOG("Collision: Player and Ball");
            auto magBall = std::pow(ballMovingVector.x, 2) + std::pow(ballMovingVector.y, 2);
            auto magPlayer = std::pow(playerMovingVector.x, 2) + std::pow(playerMovingVector.y, 2);
            auto force = std::sqrt(magBall + magPlayer);
            auto angle = std::atan2(dy, dx);
            ballMovingVector.x = force * std::cos(angle);
            ballMovingVector.y = force * std::sin(angle);
            ballNextPosition.x = playerNextPosition.x + (player->getRadius() + _ball->getRadius() + force) * std::cos(angle);
            ballNextPosition.y = playerNextPosition.y + (player->getRadius() + _ball->getRadius() + force) * std::sin(angle);
            //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
        }
    }

    // check collision between ball and walls
    if (ballNextPosition.x < minX)
    {
        CCLOG("Collision: Ball and Left wall");
        ballNextPosition.x = minX;
        ballMovingVector.x *= -0.75f; // move back
        //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
    }
    else if (ballNextPosition.x > maxX)
    {
        CCLOG("Collision: Ball and Right wall");
        ballNextPosition.x = maxX;
        ballMovingVector.x *= -0.75f; // move back
        //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
    }
    else if (ballNextPosition.y < minY)
    {
        CCLOG("Collision: Ball and Bot wall");
        ballNextPosition.y = minY;
        ballMovingVector.y *= -0.75f; // move back
        //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
    }
    else if (ballNextPosition.y > maxY)
    {
        CCLOG("Collision: Ball and Top wall");
        ballNextPosition.y = maxY;
        ballMovingVector.y *= -0.75f; // move back
        //CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
    }

    _ball->setMovingVector(ballMovingVector);
    _ball->setNextPosition(ballNextPosition);

    // move players
    _player1->setPosition(_player1->getNextPosition());
    _player2->setPosition(_player2->getNextPosition());
    _ball->setPosition(_ball->getNextPosition());
}

void GameLayer::addBackgroud()
{
    auto background = Sprite::create("court.png");
    background->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    background->setScaleX(_screenSize.width / background->getContentSize().width);
    background->setScaleY(_screenSize.height / background->getContentSize().height);
    this->addChild(background, 0, "Background");
}

void GameLayer::addPlayers()
{
    _player1 = GameSprite::createWithFile("mallet.png");
    _player1->setPosition(Vec2(_screenSize.width * 0.5f, _player1->getRadius()));
    _player1->setAnchorPoint(Vec2(0.5f, 0.5f));
    _player1->setIgnoreAnchorPointForPosition(false);
    _players.pushBack(_player1);
    this->addChild(_player1, 0, "Player1");

    _player2 = GameSprite::createWithFile("mallet.png");
    _player2->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height -_player2->getRadius()));
    _player2->setAnchorPoint(Vec2(0.5f, 0.5f));
    _player2->setIgnoreAnchorPointForPosition(false);
    _players.pushBack(_player2);
    this->addChild(_player2, 0, "Player1");
}

void GameLayer::addBall()
{
    _ball = GameSprite::createWithFile("puck.png");
    _ball->setPosition(Vec2(_screenSize.width * 0.5, _screenSize.height * 0.5));
    this->addChild(_ball, 0, "Ball");
}

void GameLayer::addScoreLabels()
{
    _player1ScoreLabel = Label::createWithTTF("0", "fonts/Arial.ttf", 60);
    _player1ScoreLabel->setPosition(Vec2(_screenSize.width - 60, _screenSize.height * 0.5f - 80));
    _player1ScoreLabel->setRotation(90);
    this->addChild(_player1ScoreLabel, 0, "Player1ScoreLabel");

    _player2ScoreLabel = Label::createWithTTF("0", "fonts/Arial.ttf", 60);
    _player2ScoreLabel->setPosition(Vec2(_screenSize.width - 60, _screenSize.height * 0.5f + 80));
    _player2ScoreLabel->setRotation(90);
    this->addChild(_player2ScoreLabel, 0, "Player2ScoreLabel");
}

void GameLayer::addEventListener()
{
    auto listener = EventListenerTouchAllAtOnce::create();
    listener->onTouchesBegan = CC_CALLBACK_2(GameLayer::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(GameLayer::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(GameLayer::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool GameLayer::isGoal() const
{
    const auto minX = _screenSize.width * GOAL_LEFT;
    const auto maxX = _screenSize.width * GOAL_RIGHT;
    const auto minY = _ball->getRadius();
    const auto maxY = _screenSize.height - _ball->getRadius();
    const auto x = _ball->getPositionX();
    const auto y = _ball->getPositionY();
    const auto inLine = y <= minY || y >= maxY;
    const auto inRangeX = x >= minX && x <= maxX;
    return inLine && inRangeX;
}


int GameLayer::whichCourt() const
{
    return _ball->getPositionY() <= _screenSize.height * 0.5 ? 1 : 2;
}

void GameLayer::resetGame()
{
    _player1->setPosition(Vec2(_screenSize.width * 0.5f, 0));
    _player1->setTouch(nullptr);
    _player1->setMovingVector(Vec2());

    _player2->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height));
    _player2->setTouch(nullptr);
    _player2->setMovingVector(Vec2());

    _ball->setPosition(Vec2(_screenSize.width * 0.5f, _screenSize.height * 0.5f));
    _ball->setMovingVector(Vec2());
}

void GameLayer::increaseScore(int whichplayer)
{
    if (whichplayer == 1)
    {
        _player1Score++;
        _player1ScoreLabel->setString(std::to_string(_player1Score));
    }
    else
    {
        _player2Score++;
        _player2ScoreLabel->setString(std::to_string(_player2Score));
    }
}
