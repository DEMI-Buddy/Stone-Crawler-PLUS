#include "game_handler.h"

enum facing 
{
	NORTH = 1,
	SOUTH = 2,
	WEST = 3,	
	EAST = 4
};

struct movableEnemy
{
	bool alive = true;
	int coords[2];
};

class dungeon_crawling : public system_handler
{
	public:
		dungeon_crawling(){}
		
		// new instance of dungeon_crawling, loading an existing file for the map layout 
		dungeon_crawling(game_handler * g, string filename)
		{
			// get dungeon file from memory
			fstream fileload("resources/maps/" + filename + ".txt", ios_base::in);
			fstream enemyload("resources/maps/" + filename + ".enemydat", ios_base::in);
			string reader; 
			
			if(fileload.is_open())
			{
				// get the dungeon crawling system running 
				main_game = g;
			
				// load images 
				enemy = image("resources/sprites/test_enemy.png",g->renderer);
				
				player = image("resources/sprites/kid.png",g->renderer);
				b_player = image("resources/sprites/back_kid.png",g->renderer);
				brick = image("resources/sprites/brick.png",g->renderer);
				brick.scale = scale;
				//player.scale = scale;
				
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
				
				for(int z=0;z<max_z;z++) // get map information stored into array 
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
			
			newGame = true;
			loadIn = true;
			
			moveEnemyTimer.start();
		}
		
		// display dungeon 
		void display() override
		{
			// set camera position for the dungeon
			cameraX = SCREEN_WIDTH/2-(pY*20*scale)-(pX*20*scale);
			cameraY = SCREEN_HEIGHT/2+(pX*10*scale)-(pY*10*scale);
			
			if(loadIn || switchOut)
			{
				brick.setAlpha(megaAlpha);
				if(megaAlpha <= 100)
					enemy.setAlpha(megaAlpha);
			}
			
			// go through data of the floor the player is on to display dungeon
			for(int y=1;y<max_y;y++)
			{
				for(int x=max_x-1;x>=0;x--)
				{
					// display specific block
					renderRect = {40*(map[pZ][y][x]-'0'),0,40,40};
					
					if((pY-1 == y || pY == y || pY+1 ==y) && (pX-1 == x || pX == x || pX+1 ==x))
					{
						enemy.setColor(255,255,255);
						if(!loadIn && !switchOut)	
							enemy.setAlpha(255);
						brick.setColor(255,255,255);
					}
					else
					{
						enemy.setColor(100,100,100);
						if(!loadIn && !switchOut)	
							enemy.setAlpha(100);
						brick.setColor(100,100,100);
					}
					
					brick.render(main_game->renderer,cameraX+(y*20*scale)+(x*20*scale),cameraY-(x*10*scale)+(y*10*scale),&renderRect);
					
					for(int i=0;i<numEnemies;i++)
					{
						if(coords[i][0] == x && coords[i][1] == y )
							enemy.render(main_game->renderer,cameraX+(coords[i][1]*20*scale)+(coords[i][0]*20*scale)+20,cameraY-(coords[i][0]*10*scale)+(coords[i][1]*10*scale)-40);		
					}
						
					if(pX == x && pY == y)
					{
						// display player 
						if(!loadIn && !switchOut) // if everything is loaded in
						{
							switch(direction)
							{
								case NORTH:
								b_player.flip = SDL_FLIP_HORIZONTAL;
								b_player.render(main_game->renderer,cameraX+(pY*20*scale)+(pX*20*scale)+20,cameraY-(pX*10*scale)+(pY*10*scale)-40);
								break;
								case EAST:
								b_player.flip = SDL_FLIP_NONE;
								b_player.render(main_game->renderer,cameraX+(pY*20*scale)+(pX*20*scale)+20,cameraY-(pX*10*scale)+(pY*10*scale)-40);
								break;
								case WEST:
								player.flip = SDL_FLIP_HORIZONTAL;
								player.render(main_game->renderer,cameraX+(pY*20*scale)+(pX*20*scale)+20,cameraY-(pX*10*scale)+(pY*10*scale)-40);
								break;
								case SOUTH:
								player.flip = SDL_FLIP_NONE;
								player.render(main_game->renderer,cameraX+(pY*20*scale)+(pX*20*scale)+20,cameraY-(pX*10*scale)+(pY*10*scale)-40);
								break;
							}
						}
						else // moving between floors or loading in/out of dungeon crawling 
						{
							player.render(main_game->renderer,cameraX+(pY*20*scale)+(pX*20*scale)+20,cameraY-(pX*10*scale)+(pY*10*scale)-40 + moveAnimation);					
							
							if(!newGame)
							{
								switch(up)
								{
									case true:
									moveAnimation-=10;
									break;
									case false:
									moveAnimation+=10;
									break;
								}
							}	
						}
					}
				}	
			}	
			
			// loading into new floor
			if(loadIn && megaAlpha == 255)
			{
				loadIn = false;
				newGame = false;
				
				// load enemies on the new floor 
				numEnemies = rand()%10;
				for(int i=0;i<numEnemies;i++)
				{
					movableEnemies[i] = true;
	
					coords[i][0] = (i+1);
					coords[i][1] = 1;
				}
			}
			else if(loadIn)
				megaAlpha+=5;
			else if(switchOut && megaAlpha == 0)
			{
				megaAlpha = 255;
				switchOut = false;
				loadIn = true;
				
				// clear current enemies 
				for(int i=0;i<10;i++)
				{
					movableEnemies[i] = false;
					coords[i][0] = 0;
					coords[i][1] = 0;
				}
			}
			else if(switchOut)
				megaAlpha-=5;
			
			
		}
		
		// input handling
		void handler() override
		{
			if(!loadIn && !switchOut)
			{
				switch(main_game->input.state)
				{
					case UP:
					if((pY != 1) && (map[pZ][pY-1][pX] != '0'))
					{
						pY--;
						direction = NORTH;
					}
					break;
					case LEFT:
					if((pX != 0) && (map[pZ][pY][pX-1] != '0'))
					{
						pX--;
						direction = WEST;
					}
					break;
					case RIGHT:
					if((pX != max_x-1) && (map[pZ][pY][pX+1] != '0'))
					{
						pX++;
						direction = EAST;
					}
					break;
					case DOWN:
					if((pY != max_y-1) && (map[pZ][pY+1][pX] != '0'))
					{
						pY++;
						direction = SOUTH;
					}
					break;
					case SELECT:
					break;
				}
				switch(map[pZ][pY][pX])
				{
					case '2':
					up = true;
					switchOut = true;
					pZ++;
					moveAnimation = 0;
					break;
					case '3':
					up = false;
					switchOut = true;
					pZ--;
				    moveAnimation = 0;
					break;
				}
			
				if(moveEnemyTimer.getTicks()/1000 >= 0.1) // move enemies after (number on the right) seconds
				{
					int addX, addY;
			
					moveEnemyTimer.stop();
					for(int i=0;i<numEnemies;i++)
					{
						addX = rand()%3-1;
						addY = rand()%3-1;
						if(coords[i][1]+addY == max_y-1 || coords[i][1]+addY == 1)
							addY = 0;
						if(coords[i][0]+addX == max_x-1 || coords[i][0]+addX == 0)
							addX = 0;
							
						if(map[pZ][coords[i][1]+addY][coords[i][0]+addX] != '0')
						{
							if(coords[i][1]+addY >= 1)
								coords[i][1]+=addY;
							coords[i][0]+=addX;
						}
						
					}
					moveEnemyTimer.start();	
				}
			}
		}
	
	private:
		// enemy handling 
		timer moveEnemyTimer; // for moving enemies without input 
		image enemy;
		int numEnemies = 0; // number of enemies in the area 
		int coords[10][2]; // position of enemies on the map 
		bool movableEnemies[10]; // bool for enemies that the player hasn't met 
		
		// variables used for loading in a new floor 
		bool loadIn = false;
		bool switchOut = false;
		int megaAlpha = 0;
		
		// used for vertically moving the player when they go up/down stairs 
		int moveAnimation = 0;
		
		bool newGame = false;
		bool up = true;
		
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
		int scale = 2;
		
		// image of dungeon blocks
		image brick;  
		
		// player sprite 
		image player; 
		image b_player;
		
		// the direction the player is facing 
		facing direction = SOUTH;
	
};

