#include "cGandalf.h"
#include <cmath>

#include <iostream>

std::string i2s(int i)
{

    char c;
    std::string ret {""};

    while ( i > 0 )
    {
        c = i % 10 + 48;
        ret = c + ret;
        i /= 10;
    }
    
    return ret;
}

cGandalf::cGandalf(int x, int y):
board { gBoardSizeX, gBoardSizeY, gTileSizeX, gTileSizeY },
player { gDefaultPosition, gDefaultDir, board },
FOV { gDefaultFOV },
wsizex { x },
wsizey { y },
viewsize { gBoardSizeX * gTileSizeX / 2 },
zoomamount { 200 }
{
    window.create(sf::VideoMode(wsizex, wsizey), "Gandalf");
    //window.setVerticalSyncEnabled(true);
    
    mainview.setSize(wsizex, wsizey);
    mainview.setCenter(wsizex / 2, wsizey / 2);
    mainview.setViewport(sf::FloatRect(0,0,1,1));
    
    miniview.setSize(viewsize, viewsize);
    miniview.setViewport(sf::FloatRect(0.8,0.8,0.2,0.2));
    miniview.setCenter(gBoardSizeX * gTileSizeX / 2, gBoardSizeY * gTileSizeY / 2);
    
    step = static_cast<double>(FOV * piper180 / static_cast<double>(wsizex));
    
    // Setting up vertexarray for top-down lines and for screen
    for(auto i = 0; i < wsizex * 2; ++i)
    {
        VA.push_back(sf::Vertex { sf::Vector2f { 0.0, 0.0 } });
    }
    
    for (auto i = 0; i < wsizex * 2; ++i)
    {
        VA.push_back(sf::Vertex { sf::Vector2f { static_cast<float>(i / 2), 0.0 } });
    }
    
    // Adding some random blocks
    board.randomBlocks(60);

    // And putting the player in, making sure that it's not on a blocked tile
    int px, py;
    sf::Vector2f spos;
    do
    {
        px = (rand() % (gBoardSizeX * gTileSizeX) - 1) + 1;
        py = (rand() % (gBoardSizeY * gTileSizeY) - 1) + 1;
        spos.x = px;
        spos.y = py;
        
    } while ( board.blocked(spos) );
    player.setPosition(spos);
    
    // Finally, setting up the projection screen

    prDistance = (wsizex / 2) / tan((FOV / 2)*piper180);
    
    // Then load fonts, set up texts etc.
    mFont.loadFromFile(resourcePath() + "sansation.ttf");

    tFPS.setFont(mFont);
    tFPS.setCharacterSize(16);
    tFPS.setPosition(sf::Vector2f(20, 60));
    tFPS.setString("FPS: ");
    
    tFOV.setFont(mFont);
    tFOV.setCharacterSize(16);
    tFOV.setPosition(sf::Vector2f(20, 90));
    tFOV.setString("FOV: " + i2s(FOV));
    
    tHints.setFont(mFont);
    tHints.setCharacterSize(16);
    tHints.setPosition(sf::Vector2f(20,20));
    tHints.setString("Q / E: adjust Field of View.\nO / P: zoom minimap.");
    
    // Misc.
    
    currentFPS = 0;
}

void cGandalf::adjustZoom(float f)
{
    viewsize += f;
    if ( viewsize > gBoardSizeX * gTileSizeX ) viewsize = gBoardSizeX * gTileSizeX;
    if ( viewsize < 400 ) viewsize = 400;
    miniview.setSize(viewsize, viewsize);
}

void cGandalf::setFOV(int f)
{

    // When changing the field of view, we also need
    // to update the angle-step between consecutive rays
    
    FOV = f;
    if ( FOV > 130 ) FOV = 130;
    if ( FOV < 45 ) FOV = 45;
    step = static_cast<double>(FOV * piper180 / static_cast<double>(wsizex));
    tFOV.setString("FOV: " + i2s(FOV));
}

void cGandalf::adjustFOV(int f)
{
    setFOV(FOV + f);
}

cCoordinates cGandalf::h_intersect(const sf::Vector2f& p, double dgr, double tan_alpha) const
{
    // No point in checking horizontal
    // intersections if the line itself is
    // horizontal => returning empty coordinates,
    // initialized to wallhit=false.

    cCoordinates a;

	if ((dgr==0) || (dgr==PI)) return a;

    // Otherwise:
	// Find first horizontal intersection point "a",
    // and calculate delta x and delta y (dx, dy)
	
	int dy;

	if (dgr < PI)	// we're going up!
	{
		a.y = (static_cast<int>(p.y / gTileSizeY) * gTileSizeY)-1;
		dy = -gTileSizeY;
	} else		// we're going down!
	{
		a.y = ((static_cast<int>(p.y / gTileSizeY) + 1 ) * gTileSizeY);
		dy = gTileSizeY;
	}
    
    double dx = -(dy / tan_alpha);
	a.x = p.x + ((p.y-a.y) / tan_alpha);
    
	// Then simply start checking intersection points until found one or out of bounds
	
    a.wallHit = board.blocked(a.x, a.y);
    
	while (!a.wallHit)
    {
        // Corner protection: going down and left
        
        if ((dgr > PI) && (dgr < onehalfpi) &&
            board.blocked(a.x + gTileSizeX, a.y) &&
            board.blocked(a.x, a.y - gTileSizeY) &&
            board.blocked(a.x + 1, a.y))
        {
            a.x += 1;
            a.wallHit = true;
            return a;
        }
        
        // Corner protection: going down and right
        
        else if ((dgr > onehalfpi) &&
            board.blocked(a.x - gTileSizeX, a.y) &&
            board.blocked(a.x, a.y - gTileSizeY) &&
            board.blocked(a.x - 1, a.y))
        {
            a.x -= 1;
            a.wallHit = true;
            return a;
        }
        
		
        // Corner protection: going up and right
        // so checking for a.y+1:
        // if we're standing at this point, one pixel below which
        // is a wall, there is a wall to the left of us, and
        // we're coming
        // from down-left (i.e. in an up-right motion),
        // that means we've slipped through a corner.

        else if ((dgr < piper2) &&
            board.blocked(a.x - gTileSizeX, a.y) &&
            board.blocked(a.x, a.y + gTileSizeY) &&
            board.blocked(a.x, a.y + 1))
        {
            a.y += 1;
            a.wallHit = true;
            return a;
        }
    
        // Corner protection: going up and left
        // See above.
        
        else if (dgr > piper2 && dgr < PI &&
            board.blocked(a.x + gTileSizeX, a.y) &&
            board.blocked(a.x, a.y + gTileSizeY) &&
            board.blocked(a.x, a.y+1))
        {
            a.y += 1;
            a.wallHit = true;
            return a;
        }
        
        a.y += dy;
        a.x += dx;
        a.wallHit = board.blocked(a.x, a.y);
    };
	
	return a;
}

// Checks for vertical intersections
cCoordinates cGandalf::v_intersect(const sf::Vector2f& p, double dgr, double tan_alpha) const
{
	
    // First let's find first vertical intersection point;
    // But no point in checking for vertical intersection
    // if the ray itself is vertical

	cCoordinates a;
	if ((dgr==piper2) || (dgr==onehalfpi)) return a;
	
    // Calculate delta x
    
	int dx;
	
	if ((dgr < piper2) || (dgr > onehalfpi))	   // we're going right!
	{
		a.x = (static_cast<int>(p.x / gTileSizeX)+1)*gTileSizeX;
		dx = gTileSizeX;
	} else									// we're going left!
	{
		a.x = static_cast<int>(p.x / gTileSizeX) * gTileSizeX - 1;
		dx = -gTileSizeX;
	}
    
    double dy = -(dx * tan_alpha);
    a.y = p.y-((a.x-p.x) * tan_alpha);
    
	// Then simply start checking intersection points until found one or out of bounds
	a.wallHit = board.blocked(a.x, a.y);
	
    while (!a.wallHit)
    {
        a.x += dx;
        a.y += dy;
        a.wallHit = board.blocked(a.x, a.y);
    }
    
	return a;
}


cCoordinates cGandalf::castRay(const sf::Vector2f& p, double degree) const
{
    
    // Casts a ray from point p in direction "degree" (in radians);
    // returns endpoint coordinates.
    
    // First we need to normalize degree, so that it remains positive,
    // in case we've turned more than 360 degrees. Just in case.
	
    if (degree > 2*PI) degree -= 2*PI;
	if (degree < 0) degree += 2*PI;

    // Now cast the ray: first check for intersection with horizontal
    // grid lines, then with vertical ones.
    
    double tan_alpha = tan(degree);
    cCoordinates h = h_intersect(p, degree, tan_alpha);
    cCoordinates v = v_intersect(p, degree, tan_alpha);

    // If both hit the wall, let's choose whichever is closer to p.

	if (v.wallHit == false)
    {
        h.distance = sqrt(pow(h.y-p.y,2)+pow(h.x-p.x,2));
        return h;
    }
    else if (h.wallHit == false)
    {
        v.distance = sqrt(pow(v.y-p.y,2)+pow(v.x-p.x,2));
        return v;
    }
    else {
        h.distance = sqrt(pow(h.y-p.y,2)+pow(h.x-p.x,2));
        v.distance = sqrt(pow(v.y-p.y,2)+pow(v.x-p.x,2));
        return h.distance < v.distance ? h : v;
    }

    // We should never actually get here, this is just to appease Xcode
	return h;
}

void cGandalf::sweep()
{
    // We need to draw as many lines as there are columns on the screen
    // - e.g. wsizex.
    
    double dgr = player.getDirection() + 90;             // direction in degrees
    double ray_degree = (dgr + (FOV/2)) * piper180; // first ray needs cast in this
                                                    // angle ( in radians )
	double facing = dgr * piper180;                 // direction in radians
    
    tmpos = player.getPosition();
    
    for (auto i = 0; i < wsizex * 2; i += 2)
    {

        VA[i].position = tmpos;
        cTmp = std::move(castRay(VA[i].position, ray_degree));
        VA[i+1].position = sf::Vector2f { cTmp.x, cTmp.y };
        ray_degree -= step;
        
        // cTmp shows what kinda wall we're hitting.
        
        // Fisheye-removal:
        cTmp.distance *= cos ( ray_degree - facing );
        
        // In the following, gTileSizeX should stand for Unit -
        // i.e. we're working with squares here!
        
        int height = ( gTileSizeX / cTmp.distance ) * prDistance;
        if ( height < 0 ) height = 0;
        if ( height > wsizey ) height = wsizey;
        
        // dmod: distance modifier: the farther away something is,
        // the darker it'll look
        
        float dmod = (0.1 * cTmp.distance / gTileSizeX);
        if ( dmod > 1 ) dmod = 1;
        
        switch ( board.status(cTmp.x, cTmp.y) )
        {
            case Terrain::wall1: colTmp = wall1_color; break;
            case Terrain::wall2: colTmp = wall2_color; break;
            case Terrain::wall3: colTmp = wall3_color; break;
            case Terrain::wall4: colTmp = wall4_color; break;
            default: break;
        }
        
        colTmp.a = 100 + 150 * (1-dmod);
        
        // OK, time to actually add the line. No need to worry about
        // x coordinates though. They will never change, and they are
        // set up in cGandalf's constructor.
        // So only the y coordinate is a question.
        // VA is the vertexarray: so in the end we'll just draw the
        // whole screen with a single draw() call.
        
        VA[i + wsizex * 2].position.y = wsizey / 2 - height / 2;
        VA[i + 1 + wsizex * 2].position.y = wsizey / 2 + height / 2;
        
        VA[i + wsizex * 2].color = colTmp;
        VA[i + 1 + wsizex * 2].color = colTmp;
    }
}

void cGandalf::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        
        if (event.type == sf::Event::KeyPressed)
        {
            switch ( event.key.code )
            {
                case sf::Keyboard::Escape: window.close(); break;
                case sf::Keyboard::O: adjustZoom(zoomamount); break;
                case sf::Keyboard::P: adjustZoom(-zoomamount); break;
                case sf::Keyboard::Q: adjustFOV(-5); break;
                case sf::Keyboard::E: adjustFOV(5); break;
                default: break;
            }
        }
    }
    
    playerMove[0] = sf::Keyboard::isKeyPressed(sf::Keyboard::W);    // forward
    playerMove[1] = sf::Keyboard::isKeyPressed(sf::Keyboard::S);    // backward
    playerMove[2] = sf::Keyboard::isKeyPressed(sf::Keyboard::A);    // strafe left
    playerMove[3] = sf::Keyboard::isKeyPressed(sf::Keyboard::D);    // strafe right
    
    player.move(playerMove);
    
    playerMove[0] = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);    // turning left?
    playerMove[1] = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);    // turning right?
    
    player.turn(playerMove);
}

void cGandalf::run()
{
    clock.restart();
    fpsclock.restart();
    timeSinceLastRender = sf::Time::Zero;
    while ( window.isOpen() )
    {
        processEvents();
        timeSinceLastUpdate = clock.restart();
        
        player.update(timeSinceLastUpdate);
        
        window.clear();
    
        // Setting up the zoom on the minimap
        
        tmpos = player.getPosition();
    
        if ( tmpos.x < viewsize / 2 ) tmpos.x = viewsize / 2;
        if ( gBoardSizeX * gTileSizeX - tmpos.x < viewsize / 2)
            tmpos.x = gBoardSizeX * gTileSizeX - viewsize / 2;
    
        if ( tmpos.y < viewsize / 2 ) tmpos.y = viewsize / 2;
        if ( gBoardSizeY * gTileSizeY - tmpos.y < viewsize / 2)
            tmpos.y = gBoardSizeY * gTileSizeY - viewsize / 2;

        miniview.setCenter(tmpos);
        
        // OK. Sweep, then.
        
        sweep();
        
        // First draw the actual scene
        
        window.setView(mainview);
        window.draw(&VA[wsizex * 2], wsizex * 2, sf::Lines);
        
        // Texts - still on main view

        window.draw(tHints);
        window.draw(tFOV);
        window.draw(tFPS);

        // Then minimap etc.
        
        window.setView(miniview);
        board.render(window);
        player.render(window);
        window.draw(&VA[0], wsizex * 2, sf::Lines);
        
        ++currentFPS;
        
        timeSinceLastRender += fpsclock.restart();
        if ( timeSinceLastRender >= sf::seconds(1.0) )
        {
            tFPS.setString("FPS: " + i2s(currentFPS));
            currentFPS = 0;
            timeSinceLastRender -= sf::seconds(1.0);
        }
        
        window.display();
    }
}