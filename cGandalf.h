#ifndef __sfml_raycaster__cGandalf__
#define __sfml_raycaster__cGandalf__

// Gandalf, as in, a wizard that CASTS spells... caster.... get it?! ahhahah
// so funny

#include "cBoard.h"
#include "cPlayer.h"
#include "global enums.h"
#include "cCoordinates.h"
#include "ResourcePath.hpp"

class cGandalf {
public:
    cGandalf(int, int);
    void                run();

private:
    void                processEvents();
    void                sweep(); // casts the rays;
    void                setFOV(int);
    void                adjustFOV(int);
    cCoordinates        castRay(const sf::Vector2f&, double) const;
    cCoordinates        h_intersect(const sf::Vector2f&, double, double) const;
    cCoordinates        v_intersect(const sf::Vector2f&, double, double) const;
    
    void                adjustZoom(float);
    
private:
    std::vector<sf::Vertex> VA; // vertexArray for minimap

    cBoard              board;
    cPlayer             player;
    
    int                 FOV;
    float               prDistance;         // distance to the projection screen
    
    double              step;               // the angle difference between 2 consecutive
                                            // rays
    sf::RenderWindow    window;
    sf::View            miniview;
    sf::View            mainview;
    
    float               viewsize;
    float               zoomamount;
    
    sf::Clock           clock;
    sf::Time            timeSinceLastUpdate;
    
    sf::Clock           fpsclock;
    sf::Time            timeSinceLastRender;
    int                 currentFPS;
    
    bool                playerMove[4] { false, false, false, false };   // player moving in 4 directions
    
    int                 wsizex;
    int                 wsizey;
    
    cCoordinates        cTmp;
    sf::Color           colTmp;
    sf::Vector2f        tmpos;
    
    sf::Font            mFont;
    sf::Text            tFPS;
    sf::Text            tFOV;
    sf::Text            tHints;
};

#endif /* defined(__sfml_raycaster__cGandalf__) */
