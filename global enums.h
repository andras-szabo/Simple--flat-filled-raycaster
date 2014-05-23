#ifndef sfml_raycaster_global_enums_h
#define sfml_raycaster_global_enums_h

#include <SFML/Graphics.hpp>

// Terrain: a block in the grid can be empty, as well
// as occupied by some kind of a wall.

enum class Terrain { empty, wall1, wall2, wall3, wall4 };

const sf::Color wall1_color { 150, 150, 150 };
const sf::Color wall2_color { 220, 40, 40 };
const sf::Color wall3_color { 40, 220, 40 };
const sf::Color wall4_color { 40, 40, 220 };

const float     gDefaultSpeed { 200.0 };
const float     gDefaultTurnSpeed { 180.0 };
const int       gDefaultFOV { 60 };

const int       gBoardSizeX { 40 };
const int       gBoardSizeY { 40 };

const int       gTileSizeX { 64 };
const int       gTileSizeY { 64 };

const sf::Vector2f      gDefaultPosition { 100, 100 };
const float             gDefaultDir { 0 };

const double    PI  { 3.14159265 };
const double    piper180 = PI/180;
const double    piper2 = PI/2;
const double    onehalfpi = 3*PI/2;

#endif
