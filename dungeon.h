#include "game_handler.h"

class dungeon_crawling : public system_handler
{
	public:
		dungeon_crawling(){}
		
		// new instance of dungeon_crawling, loading an existing file for the map layout 
		dungeon_crawling(game_handler * g, string filename)
		{
			// get dungeon file from memory
			fstream fileload("resources/maps/" + filename + ".txt", ios_base::in);
			string reader; 
			
			if(fileload.is_open())
			{
				// get the dungeon crawling system running 
				main_game = g;
			
				// load images 
				player = image("resources/sprites/player.png",g->renderer);
				brick = image("resources/sprites/brick.png",g->renderer);
				brick.scale = scale;
				player.scale = scale;
				
				// obtain basic dungeon info 
				fileload >> max_z;
				fileload >> max_x;
				fileload >> max_y;
				
				// set up map storage
				map = new char**[max_x];
				for(int i=0;i<max_x;i++)
				{
					map[i] = new char*[max_y];
					
					for(int j=0;j<max_y;j++)
					{
						map[i][j] = new char[max_z];
					}
				}
				
				for(int z=0;z<max_z;z++)
				{
					for(int y=0;y<max_y;y++)
					{
						getline(fileload,reader);
						for(int x=0;x<max_x;x++)
						{
							map[z][y][x] = reader[x];
						}
					}
					getline(fileload,reader);
					getline(fileload,reader);	
				}
			
			}
			else
				cout << "ERROR, DUNGEON MAP NOT FOUND";
			
			fileload.close();
		}
		
		// display dungeon 
		void display() override
		{
			cameraX = SCREEN_WIDTH/2-(pY*20*scale)-(pX*20*scale);
			cameraY = SCREEN_HEIGHT/2+(pX*10*scale)-(pY*10*scale);
			
			// go through data of the floor the player is on to display dungeon
			for(int y=1;y<max_y;y++)
			{
				for(int x=max_x-1;x>=0;x--)
				{
					// display specific block
					renderRect = {0+40*((map[pZ][y][x]-1)%2),0,40,40};
							
					if((pY-1 == y || pY == y || pY+1 ==y) && (pX-1 == x || pX == x || pX+1 ==x))
						brick.setColor(255,255,255);
					else
						brick.setColor(100,100,100);
					
					brick.render(main_game->renderer,cameraX+(y*20*scale)+(x*20*scale),cameraY-(x*10*scale)+(y*10*scale),&renderRect);
										
					// display player 
					player.render(main_game->renderer,cameraX+(pY*20*scale)+(pX*20*scale),cameraY-(pX*10*scale)+(pZ*20*scale)+(pY*10*scale)-25*scale);
					
				}	
			}	
			main_game->displayText.display("X: "+to_string(pX),100,100);
			main_game->displayText.display("Y: "+to_string(pY),100,160);
			main_game->displayText.display("Z: "+to_string(pZ),100,220);
			
		}
		
		// input handling
		void handler() override
		{
			switch(main_game->input.state)
			{
				case UP:
				if((pY != 0) && (map[pZ][pY-1][pX] == '1'))
					pY--;
				break;
				case LEFT:
				if((pX != 0) && (map[pZ][pY][pX-1] == '1'))
					pX--;
				break;
				case RIGHT:
				if((pX != max_x) && (map[pZ][pY][pX+1] == '1'))
					pX++;
				break;
				case DOWN:
				if((pY != max_y-1) && (map[pZ][pY+1][pX] == '1'))
					pY++;
				break;
			}
		}
	private:
		// for rendering specific blocks from the block image 
		SDL_Rect renderRect = {0,0,40,40};
		
		// map data in flexible char 3d array
		char *** map;
		
		// max size of dungeon
		int max_z; 
		int max_y;
		int max_x;
		
		// player coords
		int pX = 0;
		int pY = 1;
		int pZ = 0;
		
		// camera variables
		int cameraX = 400;
		int cameraY = 300;
		
		// scale of images
		int scale = 1;
		
		// image of dungeon blocks
		image brick;  
		
		// player sprite 
		image player; 
	
};

