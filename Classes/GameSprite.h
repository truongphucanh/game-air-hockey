#pragma once
#include "cocos2d.h"

using namespace cocos2d;

class GameSprite : public Sprite
{
public:
    // this sprite will move to nextPosition in update())
    CC_SYNTHESIZE(Vec2, _nextPosition, NextPosition);

    // this sprite will move by this vector
    CC_SYNTHESIZE(Vec2, _movingVector, MovingVector);

    // holds the touch on this sprite
    CC_SYNTHESIZE(Touch*, _touch, Touch); 

    GameSprite();
    virtual ~GameSprite();
    static GameSprite* createWithFile(const char* fileName);
    void setPosition(const Vec2& position) override;

    /**
     * \brief Get radius of this sprite
     * \return Radius = 1/2 width
     */
    float getRadius() const;
};
