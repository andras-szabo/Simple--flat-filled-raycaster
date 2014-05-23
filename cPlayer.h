#ifndef __sfml_raycaster__cPlayer__
#define __sfml_raycaster__cPlayer__

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "global enums.h"
#include "cBoard.h"

class cPlayer {
public:
    cPlayer(sf::Vector2f, float, const cBoard& m);
    
    void        setSpeed(float);
    void        setTurnSpeed(float);
    void        setPosition(sf::Vector2f);
    void        update(sf::Time);    
    void        move(bool*);
    void        turn(bool*);
    
    float           getDirection() const;
    sf::Vector2f    getPosition() const;
    
    // This only for diagnostics
    void        render(sf::RenderWindow&) const;
    
    
private:
    sf::Vector2f        mPos;       // pos, where are we on the map?
    sf::Vector2f        mVel;       // velocity
    float               mDir;       // angle of direction in which we're facing
    float               mTurning;   // angle that we're turning in a given update
    
    float               mSpeed;     // maximum speed in pixel/sec
    float               mTurnSpeed; // maximum turning rate in degrees/sec
    
    const cBoard&       mMap;
};

#endif /* defined(__sfml_raycaster__cPlayer__) */
