#include "GameLayer.h"

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

                    // keep player inside scene
                    if (nextPosition.x < player->getRadius())
                        nextPosition.x = player->getRadius();
                    if (nextPosition.x > _screenSize.width - player->getRadius())
                        nextPosition.x = _screenSize.width - player->getRadius();
                    if (nextPosition.y < player->getRadius())
                        nextPosition.y = player->getRadius();
                    if (nextPosition.y > _screenSize.height - player->getRadius())
                        nextPosition.y = _screenSize.height - player->getRadius();

                    // keep player inside its court
                    if (player->getPositionY() < _screenSize.height* 0.5f) 
                    {
                        if (nextPosition.y > _screenSize.height* 0.5 - player->getRadius()) 
                        {
                            nextPosition.y = _screenSize.height* 0.5 - player->getRadius();
                        }
                    }
                    else 
                    {
                        if (nextPosition.y < _screenSize.height* 0.5 + player->getRadius()) 
                        {
                            nextPosition.y = _screenSize.height* 0.5 + player->getRadius();
                        }
                    }

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
    // move players
    _player1->setPosition(_player1->getNextPosition());
    _player2->setPosition(_player2->getNextPosition());
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
    _player1ScoreLabel = Label::createWithTTF("1", "fonts/Arial.ttf", 60);
    _player1ScoreLabel->setPosition(Vec2(_screenSize.width - 60, _screenSize.height * 0.5f - 80));
    _player1ScoreLabel->setRotation(90);
    this->addChild(_player1ScoreLabel, 0, "Player1ScoreLabel");

    _player2ScoreLabel = Label::createWithTTF("2", "fonts/Arial.ttf", 60);
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
