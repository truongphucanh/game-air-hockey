#include "GameSprite.h"

GameSprite::GameSprite()
{
    _nextPosition = Vec2(0.0f, 0.0f);
    _movingVector = Vec2(0.0f, 0.0f);
    _touch = nullptr;
}

GameSprite::~GameSprite()
{
}

GameSprite* GameSprite::createWithFile(const char* fileName)
{
    auto sprite = new(std::nothrow) GameSprite();
    if (sprite && sprite->initWithFile(fileName))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return sprite = nullptr;
}

void GameSprite::setPosition(const Vec2& position)
{
    Sprite::setPosition(position);
    if(!_nextPosition.equals(position))
    {
        _nextPosition = position;
    }
}

float GameSprite::getRadius() const
{
    return this->getTexture()->getContentSize().width * 0.5f;
}

