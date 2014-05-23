#ifndef __sfml_raycaster__cBoard__
#define __sfml_raycaster__cBoard__

#include <vector>
#include <SFML/Graphics.hpp>
#include "global enums.h"

class cBoard {
public:
    cBoard(int, int, int, int);
    
    void        render(sf::RenderWindow&) const;
    bool        block(int, int, Terrain);
    void        randomBlocks(int);
    Terrain     status(int, int) const;
    
    // blocked() checks whether a coordinate is valid AND
    // whether it falls onto a blocked tile.
    
    bool        blocked(sf::Vector2f v) const;
    inline bool blocked(float x, float y) const
    {
        if (valid(static_cast<int>(x) / mTileWidth,
                  static_cast<int>(y) / mTileHeight) &&
            mMap[x / mTileWidth][y / mTileHeight] == Terrain::empty)
            {
                return false;
            }

        return true;
    }
    
private:
    bool        valid(sf::Vector2i) const;
    inline bool valid(int x, int y) const
    {
        return x >= 0 && y >= 0 && x < mBoardSizeX && y < mBoardSizeY;
    }
    
private:
    std::vector<std::vector<Terrain>>   mMap;
    std::vector<sf::Vertex>             mVA;    // vertexarray

    size_t      mVAsize;        // size of the vertexarray
    
    // Board size: well, the size of the board. The
    // map.
    
    int         mBoardSizeX;
    int         mBoardSizeY;
    
    // Tile size: the size of one tile in the map, in units.
    // Practically, pixels.

    int         mTileWidth;
    int         mTileHeight;
    
};

#endif /* defined(__sfml_raycaster__cBoard__) */
