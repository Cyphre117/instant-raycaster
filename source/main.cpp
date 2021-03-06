//
// This is a port of the raycaster Lode into SDL2
// http://lodev.org/cgtutor/raycasting.html
//

// Copyright (c) 2004-2007, Lode Vandevenne
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <iostream>
#include <SDL2/SDL.h>

#include "SDLsetup.h"
#include "InstantCG.h"
using namespace InstantCG;

// replacements for quickcg functionality

#define mapWidth 24
#define mapHeight 24

// function prototypes
void print(float num);
bool processEvents();

// globals
SDL_Texture* loadImage(std::string path);
int mouseXDist; // horizontal distance traveled by the mouse

int worldMap[mapWidth][mapHeight]=
{
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int main()
{

    screen(640, 480);
    SDL_SetRelativeMouseMode( SDL_bool(true) ); // lock the cursor to the window

	// loading in the textures

    SDL_Texture* test = loadImage("img/test.bmp");
    SDL_Texture* wall1 = loadImage("img/wall1.bmp");
    SDL_Texture* wall2 = loadImage("img/wall2.bmp");

    // set the default texture
    SDL_Texture* tex = test;

	// get the width and height of the texture
	int texWidth  = 0;
	int texHeight = 0;
	SDL_QueryTexture(tex, NULL, NULL, &texWidth, &texHeight);

	double posX = 22, posY = 12; //x and y start position
	double dirX = -1, dirY = 0;  //initial direction vector
	double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

	double time = 0;    //time of current frame
	double oldTime = 0; //time of previous frame

	while( !processEvents() ) // START OF GAME LOOP
	{
        for(int x = 0; x < w; x++)
	    {
			//calculate ray position and direction
			double cameraX = 2 * x / double(w) - 1; //x-coordinate in camera space
			double rayPosX = posX;
			double rayPosY = posY;
			double rayDirX = dirX + planeX * cameraX;
			double rayDirY = dirY + planeY * cameraX;

			//which box of the map we're in
			int mapX = int(rayPosX);
			int mapY = int(rayPosY);

			//length of ray from current position to next x or y-side
			double sideDistX;
			double sideDistY;

			//length of ray from one x or y-side to next x or y-side
			double deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
			double deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
			double perpWallDist;

			//what direction to step in x or y-direction (either +1 or -1)
			int stepX;
			int stepY;

			int hit = 0; //was there a wall hit?
			int side; //was a NS or a EW wall hit?

			//calculate step and initial sideDist
			if (rayDirX < 0)
			{
				stepX = -1;
				sideDistX = (rayPosX - mapX) * deltaDistX;
			}
			else
			{
				stepX = 1;
				sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
			}
			if (rayDirY < 0)
			{
				stepY = -1;
				sideDistY = (rayPosY - mapY) * deltaDistY;
			}
			else
			{
				stepY = 1;
				sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
			}

			//perform DDA
			while (hit == 0)
			{
				//jump to next map square, OR in x-direction, OR in y-direction
				if (sideDistX < sideDistY)
				{
					sideDistX += deltaDistX;
					mapX += stepX;
					side = 0;
				}
				else
				{
					sideDistY += deltaDistY;
					mapY += stepY;
					side = 1;
				}
				//Check if ray has hit a wall
				if (worldMap[mapX][mapY] > 0) hit = 1;
			}

			//Calculate distance projected on camera direction (oblique distance will give fisheye effect!)
			if (side == 0)
			    perpWallDist = fabs((mapX - rayPosX + (1 - stepX) / 2) / rayDirX);
			else
			    perpWallDist = fabs((mapY - rayPosY + (1 - stepY) / 2) / rayDirY);

			//Calculate height of line to draw on screen
			int lineHeight = abs(int(h / perpWallDist));

			//choose a texture 
			switch(worldMap[mapX][mapY])
			{
				case 1:  tex = wall1;  break;
				case 2:  tex = wall2;  break;
				default: tex = test;   break;
			}

			//calculate value of wallX
			double wallX; //where exactly the wall was hit
			if (side == 1)
				wallX = rayPosX + ((mapY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
			else
				wallX = rayPosY + ((mapX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
			
			wallX -= floor((wallX));

			//x coordinate on the texture
			int texX = int(wallX * double(texWidth));

			if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
			if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

			//calculate lowest and highest pixel to fill in current stripe
			int drawStart = -lineHeight / 2 + h / 2;
			int drawEnd = lineHeight / 2 + h / 2;

			// set up the rectangle to sample the texture
			SDL_Rect line = {x, drawStart, 1, drawEnd - drawStart};
			SDL_Rect sample = {texX, 0, 1, texHeight};
			
			renderTexture(tex, ren, line, &sample);
	    }

	    //timing for input and FPS counter
		oldTime = time;
		time = getTicks();
		double frameTime = (time - oldTime) / 1000.0; //frameTime is the time this frame has taken, in seconds
		print(1.0 / frameTime); //FPS counter
		redraw();
		cls();

        // draw ground and sky
        drawRect(0,   0, w, h/2, RGB_Navy);
        drawRect(0, h/2, w, h  , RGB_Darkgreen);

		//speed modifiers
		double moveSpeed = frameTime * 5.0; //the constant value is in squares/second
		double rotSpeed = frameTime * 3.0; //the constant value is in radians/second
        if( mouseXDist > 0 ) rotSpeed *= mouseXDist * 0.2;
        else if( mouseXDist < 0 ) rotSpeed *= mouseXDist * -0.2;

		//move forward if no wall in front of you
		if (keyDown(SDLK_UP) || keyDown(SDL_SCANCODE_W) )
		{
		  if(worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
		  if(worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
		}
		//move backwards if no wall behind you
		if (keyDown(SDLK_DOWN) || keyDown(SDL_SCANCODE_S) )
		{
		  if(worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
		  if(worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
		}
		//rotate to the right
		if (keyDown(SDLK_RIGHT) || keyDown(SDL_SCANCODE_D) || mouseXDist > 0 )
		{
		  //both camera direction and camera plane must be rotated
		  double oldDirX = dirX;
		  dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
		  dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
		  double oldPlaneX = planeX;
		  planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
		  planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
		}
		//rotate to the left
		if (keyDown(SDLK_LEFT) || keyDown(SDL_SCANCODE_A) || mouseXDist < 0 )
		{
		  //both camera direction and camera plane must be rotated
		  double oldDirX = dirX;
		  dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
		  dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
		  double oldPlaneX = planeX;
		  planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
		  planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
		}
	}

	return 0;
}

bool processEvents()
{
    SDL_Event event;
    mouseXDist = 0;

    readKeys();
    while( SDL_PollEvent(&event) )
    {
        if( event.type == SDL_QUIT )    return true;
        if( keyDown(SDLK_ESCAPE) )      return true;
        if( event.type == SDL_MOUSEMOTION )
        {
            mouseXDist += event.motion.xrel;
        }
    }
    return false;
}

void print(float num)
{
	//std::cout << num << std::endl;
}

SDL_Texture* loadImage(std::string path)
{
	static std::string projectPath = getProjectPath();

	SDL_Surface *bmp = SDL_LoadBMP((projectPath + path).c_str());
	if (bmp == nullptr){
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
	}

	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
	SDL_FreeSurface(bmp);
	if (tex == nullptr){
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
	}

    return tex;
}

