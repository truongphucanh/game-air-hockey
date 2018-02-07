#pragma once
#include "cocos2d.h"
#include "GameSprite.h"

using namespace cocos2d;

class GameLayer : public cocos2d::Layer
{
private:
    GameSprite* _player1;
    GameSprite* _player2;
    GameSprite* _ball;
    cocos2d::Vector<GameSprite*> _players;
    int _player1Score;
    int _player2Score;
    Label* _player1ScoreLabel;
    Label* _player2ScoreLabel;
    Size _screenSize;

public:
    GameLayer();
    virtual ~GameLayer();
    bool init() override;
    static GameLayer* create();
    void onTouchesBegan(const std::vector<Touch*>& touches, Event* event) override;
    void onTouchesMoved(const std::vector<Touch*>& touches, Event* event) override;
    void onTouchesEnded(const std::vector<Touch*>& touches, Event* event) override;
    void update(float dt) override;
    void addBackgroud();
    void addPlayers();
    void addBall();
    void addScoreLabels();
    void addEventListener();
    bool isGoal() const;
    int whichCourt() const;
    void resetGame();
    void increaseScore(int whichplayer);
};
