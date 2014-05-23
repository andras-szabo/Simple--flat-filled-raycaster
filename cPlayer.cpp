#include "cPlayer.h"

#include <iostream>

cPlayer::cPlayer(sf::Vector2f v, float dir, const cBoard& m):
mPos { v },
mVel { sf::Vector2f(0, 0) },
mDir { dir },
mSpeed { gDefaultSpeed },
mTurnSpeed { gDefaultTurnSpeed },
mMap { m }
{
    
}

sf::Vector2f cPlayer::getPosition() const
{
    return mPos;
}

void cPlayer::setSpeed(float s)
{
    mSpeed = s;
}

void cPlayer::setTurnSpeed(float t)
{
    mTurnSpeed = t;
}

void cPlayer::setPosition(sf::Vector2f p)
{
    mPos = p;
}

void cPlayer::update(sf::Time dt)
{

    // First update rotation.
    
    mDir += mTurning * dt.asSeconds();
    
    if ( mDir > 360 ) mDir -= 360;
    else if ( mDir < 0 ) mDir += 360;
    
    // Creates a rotation matrix with mDir (the direction
    // we're facing), rotates the velocity vector with it,
    // (or rather, with its inverse),
    // so that velocity becomes relative to the direction
    
    sf::Transform tmp;
    tmp.rotate(-mDir);
    mVel = tmp.transformPoint(mVel.x, mVel.y);
    
    auto new_Pos = mPos + mVel * dt.asSeconds();
    
    // Only apply new position if it falls on an empty
    // tile on the map; no clipping I'm afraid.
    
    if (mMap.blocked(new_Pos))
        return;
    else
        mPos = new_Pos;
}

void cPlayer::move(bool* m)
{
    // In the passed array, the order is:
    // forward - backward - left - right
    // 0         1          2      3

    mVel.x = 0; mVel.y = 0;
    if ( m[0] && !m[1] ) mVel.y = -mSpeed;
    if ( m[1] && !m[0] ) mVel.y = mSpeed;
    if ( m[2] && !m[3] ) mVel.x = -mSpeed;
    if ( m[3] && !m[2] ) mVel.x = mSpeed;
    
    // Cutting diagonal speeds
    
    if ( mVel.x != 0 && mVel.y != 0 )
    {
        mVel.x *= 0.7;
        mVel.y *= 0.7;
    }
}

void cPlayer::turn(bool* m)
{
    // In the passed array of bools,
    // the order is: 0: left, 1: right

    mTurning = m[0] * mTurnSpeed - m[1] * mTurnSpeed;
}

float cPlayer::getDirection() const
{
    return mDir;
}

// Placeholder only
void cPlayer::render(sf::RenderWindow& w) const
{
    sf::CircleShape c;
    c.setFillColor(sf::Color::Red);
    c.setRadius(5);
    c.setOrigin(5, 5);
    c.setPosition(mPos.x, mPos.y);

    w.draw(c);
} 
// Placeholder only