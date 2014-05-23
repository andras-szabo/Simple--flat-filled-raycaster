#include "cBoard.h"

#include <iostream>

cBoard::cBoard(int boardx, int boardy,
               int gwidth, int gheight):
mBoardSizeX { boardx },
mBoardSizeY { boardy },
mTileWidth { gwidth },
mTileHeight { gheight }
{
    // Setting up a single column, initializing as empty
    std::vector<Terrain>    column(mBoardSizeY, Terrain::empty);
    
    // Filling up the entire map
    for (auto i = 0; i < mBoardSizeX; ++i)
    {
        mMap.push_back(column);
    }
    
    // Setting up bordering walls
    for (auto i = 0; i < mBoardSizeX; ++i)
    {
        mMap[i][0] = Terrain::wall1;
        mMap[i][mBoardSizeY-1] = Terrain::wall1;
    }
    
    for (auto i = 0; i < mBoardSizeY; ++i)
    {
        mMap[0][i] = Terrain::wall1;
        mMap[mBoardSizeX-1][i] = Terrain::wall1;
    }
    
    // Preparing the vertex array
    
    mVAsize = 4 * mBoardSizeY * mBoardSizeX;
    
    for (auto i = 0; i < mBoardSizeX; ++i)
        for (auto j = 0; j < mBoardSizeY; ++j)
        {
            float left = j * mTileWidth;
            float top = i * mTileHeight;
            mVA.push_back(sf::Vertex(sf::Vector2f(left, top)));
            mVA.push_back(sf::Vertex(sf::Vector2f(left + mTileWidth, top)));
            mVA.push_back(sf::Vertex(sf::Vector2f(left + mTileWidth, top + mTileHeight)));
            mVA.push_back(sf::Vertex(sf::Vector2f(left, top + mTileHeight)));
        }
    
    // Setting up colors
    
    sf::Color tmpCol;
    
    for (auto i = 0; i < mBoardSizeX; ++i)
        for (auto j = 0; j < mBoardSizeY; ++j)
        {
            switch (mMap[i][j]) {
                case Terrain::empty: tmpCol = sf::Color::Black; break;
                case Terrain::wall1: tmpCol = wall1_color; break;
                case Terrain::wall2: tmpCol = wall2_color; break;
                case Terrain::wall3: tmpCol = wall3_color; break;
                case Terrain::wall4: tmpCol = wall4_color; break;
            }
            auto id = ( 4 * mBoardSizeX ) * j + (4 * i);

            mVA[id].color = tmpCol;
            mVA[id+1].color = tmpCol;
            mVA[id+2].color = tmpCol;
            mVA[id+3].color = tmpCol;
        }
}

bool cBoard::valid(sf::Vector2i v) const
{
    return valid(v.x, v.y);
}

Terrain cBoard::status(int x, int y) const
{
    if (valid(x / gTileSizeX, y / gTileSizeY)) return mMap[x / gTileSizeX][y / gTileSizeY];
    else return Terrain::empty;
}

void cBoard::randomBlocks(int count)
{
    for (auto i = 0; i < count; ++i)
    {
        int x, y, col;
        col = rand() % 4;
        do
        {
            x = rand() % mBoardSizeX + 1;
            y = rand() % mBoardSizeY + 1;
        } while ( !valid(x, y) || mMap[x][y] != Terrain::empty );

        switch (col) {
            case 0: block(x, y, Terrain::wall1); break;
            case 1: block(x, y, Terrain::wall2); break;
            case 2: block(x, y, Terrain::wall3); break;
            case 3: block(x, y, Terrain::wall4); break;
        }
    }
}

bool cBoard::block(int x, int y, Terrain t)
{
    if ( valid(x,y) && mMap[x][y] != t)
    {
        // Updating map
        
        mMap[x][y] = t;
        
        // Updating vertexarray w/ appropriate colour
        
        sf::Color tmpCol;
        switch (t) {
            case Terrain::empty: tmpCol = sf::Color::Black; break;
            case Terrain::wall1: tmpCol = wall1_color; break;
            case Terrain::wall2: tmpCol = wall2_color; break;
            case Terrain::wall3: tmpCol = wall3_color; break;
            case Terrain::wall4: tmpCol = wall4_color; break;
        }

        auto id = (4 * mBoardSizeX * y) + 4 * x;
        for ( auto i = 0; i < 4; ++i)
            mVA[id+i].color = tmpCol;

        // Update successful
        
        return true;
    }

    // Update unsuccessful (out of bounds or grid already held the same value)
    
    return false;
}


bool cBoard::blocked(sf::Vector2f v) const
{
    if (valid(v.x / mTileWidth, v.y / mTileHeight) &&
        mMap[v.x / mTileWidth][v.y / mTileHeight] == Terrain::empty) return false;
    return true;
}

void cBoard::render(sf::RenderWindow& w) const
{
    // Simply put out the whole vertexarray onto the supplied renderwindow
    w.draw(&mVA[0], mVAsize, sf::Quads);
}