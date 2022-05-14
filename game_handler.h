#ifndef SDL_MAIN_HANDLED
#include "systems.h"
#endif

#define HANDLER

// initalize the game with background value
game_handler::game_handler(int x = -1)
{
	if(SDL_Init( IMG_INIT_JPG | IMG_INIT_PNG | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) >= 0)
	{
		//set texture filtering to linear
		//sets render quality to where images aren't blurry when resized
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
				
		// create window 
		window = SDL_CreateWindow( "Graphics", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(window != NULL)
		{
			// creates renderer
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
					
			//Initialize renderer color
			SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF );
					
			//Initialize PNG loading
			int imgFlags = IMG_INIT_PNG;
			if( !( IMG_Init( imgFlags ) & imgFlags ))
			{
				printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
				success = false;
			}
					
			//Initialize text handling 
			if( TTF_Init() == -1 )
			{
				printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
				success = false;
			}
						
					//Initialize music handling 
			if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024*2 ) < 0 )
			{
				printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
				success = false;
			}
		}
	}
	
	// load game background assets 
	if(x == -1)
		b = 1;
	else
		b = x;
	loadBackground(b);
	
	// start framekeeper timer for limiting frame 
	framekeeper.start();
	
	// set up main text handler 
	displayText = text(renderer,0);
};
		
// loads background assets 
void game_handler::loadBackground(int type)
{
	if(background_assets != NULL)
		delete background_assets;
	switch(type)
	{
		case 0:
		background_assets = new image[2];
		background_assets[0] = image("resources/sprites/background/background_asset2.png",renderer);
		background_assets[1] = image("resources/sprites/background/background_asset1.png",renderer);
		background_assets[1].scale = 5;
		background_assets[0].scale = 3;	
		background_assets[1].setAlpha(0);
		background_assets[0].setAlpha(0);
		break;
		case 1:
		break;
		case 3:
		back_type = rand()%3+1;
		background_assets = new image[1];
		background_assets[0] = image("resources/sprites/background/background_asset3.png",renderer);
		background_assets[0].scale = 3;
		background_assets[0].setAlpha(0);
		break;
		case 5:
		background_assets = new image[3];
		background_assets[0] = image("resources/sprites/background/2_background_asset.png",renderer);
		background_assets[1] = image("resources/sprites/background/2_background_asset_2.png",renderer);
		background_assets[2] = image("resources/sprites/background/theslayer.png",renderer);
		
		background_assets[1].scale = 3;
		background_assets[0].scale = 3;	
		background_assets[2].scale = 3;	
		
		background_assets[1].setAlpha(0);
		background_assets[0].setAlpha(0);
		background_assets[2].setAlpha(0);
		break;
		default:
		break;
		
	}
	loadIn = true;
	megaAlpha = 0;
}

// switching backgrounds 
void game_handler::switchBackground(int newOne)
{
	switchOut = true;
	switchTo = newOne;
}

// display background 
void game_handler::background(bool down)
{
	switch(b)
	{
		case 0: // background with multiple assets (ritual wheels)
		if(down && colors[0] != 0 && colors[1] != 0)
		{
			colors[0] = (colors[0] - 1);
		}
		else if(colors[0] != 255 && colors[1] != 255)
		{
			colors[0] = (colors[0] + 1);
		}
	
		background_assets[1].angle = ((((int)background_assets[1].angle + 1)%360)*60/SCREEN_FPS);
		background_assets[1].render(renderer,-100,500);
		background_assets[1].angle *= -1; 
		background_assets[1].render(renderer,640,-90);
		background_assets[1].angle *= -1;	
		
		background_assets[0].angle = ((((int)background_assets[0].angle + 2)%360)*60/SCREEN_FPS);
		
		for(int i=0;i<3;i++)
		{
			background_assets[0].render(renderer,-100+i*150,-100);
			background_assets[0].angle*=-1;
			background_assets[0].render(renderer,760-i*150,600);
			background_assets[0].angle*=-1;
		}
		
		if(loadIn || switchOut)
		{
			background_assets[0].setAlpha(megaAlpha);
			background_assets[1].setAlpha(megaAlpha);
		}
		break;
		
		case 1: // backgrounds with one asset (spinning starts)
		if(switchOut)
		{
			if (colors[0] < 255)
				colors[0] = colors[0] + 1;
			if (colors[1] < 255)
				colors[1] = colors[1] + 1;
		}
		else if(down && colors[1] != 0)
			colors[1] = (colors[1] - 1);
		else if(colors[1] != 255)
			colors[1] = (colors[1] + 1);	
		
		break;
		
		case 2: // black background 
		
		if(!loadIn && !switchOut)
		{
			colors[0] = 0;
			colors[1] = 0;
			colors[2] = 0;
			displayText.textColor.a = 255;
		}
		else if(loadIn)
		{
			if(colors[0] > 0)
				colors[0] = colors[0] - 1;
			if(colors[1] > 0)
				colors[1] = colors[1] - 1;
			if(colors[2] > 0)
				colors[2] = colors[2] - 1;
		}
		break;
		
		case 3: // purple words 
		if(down && colors[0] != 0)
			colors[0] = (colors[0] - 1);
		else if(colors[0] != 255)
			colors[0] = (colors[0] + 1);
				
		if (colors[1] > 0)
			colors[1] = colors[1] - 1;
			
		if (colors[2] < 92)
			colors[2] = colors[2] + 1;
		else if(colors[2] > 92)
			colors[2] = colors[2] - 1;
		
		if(loadIn || switchOut)
		{
			background_assets[0].setAlpha(megaAlpha);
			displayText.textColor.a = megaAlpha;
		}
		background_assets[0].angle = ((((int)background_assets[0].angle + 1)%360)*60/SCREEN_FPS);
		for(int i=0;i<8;i++)
		{
			switch(back_type)
			{
				default:
				case 1:
				displayText.display("i  t urnsidewaystothesun",10,10+i*90,colors[0]);
				displayText.display("keepmythoughtsfromeveryo n  e",200,30+i*90,colors[0]+i);
				break;
				case 2:
				displayText.display("Afriendissomeonewhoisneverfar",10,10+i*90,colors[0]);
				displayText.display("Noneversofaratall",200,30+i*90,colors[0]+i);
				break;
				case 3:
				displayText.display("Tocarryon,we'llcarryon",10,10+i*90,colors[0]);
				displayText.display("Andthoughyou'redeadandgone,believeme",200,30+i*90,colors[0]+i);
				break;
			}
			background_assets[0].setColor(155-i*10,100,155-i*50);
			background_assets[0].render(renderer,50+90*i,300);
		}
		
		break;
		case 4: // white background 
		if(switchOut)
		{
			if (colors[0] < 255)
				colors[0] = colors[0] + 1;
			if (colors[1] < 255)
				colors[1] = colors[2] + 1;
			if (colors[2] < 255)
				colors[2] = colors[2] + 1;
		}
		
		colors[0] = 255;
		colors[1] = 255;
		colors[2] = 255;
		break;
		
		case 5: // battle screen 
		colors[0] = 61;
		colors[1] = 0;
		colors[2] = 92;
		
		for(int i=0;i<6;i++)
		{
			background_assets[2].render(renderer,i*180,50);
			background_assets[2].render(renderer,i*180,300);
		}
		
		background_assets[2].angle = ((((int)background_assets[2].angle + 2)%360)*60/SCREEN_FPS);		
		
		background_assets[0].render(renderer,0,0);
		background_assets[1].render(renderer,0,SCREEN_HEIGHT-180);
	
		background_assets[1].render(renderer,SCREEN_WIDTH-137*3,0);
		background_assets[0].render(renderer,SCREEN_WIDTH-137*3,SCREEN_HEIGHT-180);
		
		if(loadIn || switchOut)
		{
			background_assets[0].setAlpha(megaAlpha);
			background_assets[1].setAlpha(megaAlpha);
			background_assets[2].setAlpha(megaAlpha);
		}
		break;
	}
	if(loadIn && megaAlpha == 255)
		loadIn = false;
	else if(loadIn)
		megaAlpha+=5;
	else if(switchOut && megaAlpha == 0)
	{
		b = switchTo;
		megaAlpha = 255;
		switchOut = false;
		loadBackground(b);
	}
	else if(switchOut)
	{
		megaAlpha-=5;
	}
}

// run the current game system 
void game_handler::runSystem()
{
	// clears display 
		SDL_SetRenderDrawColor(renderer, colors[0], colors[1], colors[2], colors[3]);
		SDL_RenderClear(renderer);
		capkeeper.start();
			
	// display background
		if((int)(colors[0]) < 100)
			down = false;
		else if((int)(colors[0]) > 175)
			down = true;
		if(backgroundOn)
			background(down);

	// display game system 
		if(currentGame != NULL)
			currentGame->display();
			
		SDL_SetRenderDrawColor(renderer, colors[0], colors[1], colors[2], colors[3]);
		
	// getting input 
		input.update();
			
	// handle game system 
		if(currentGame != NULL)
			currentGame->handler();	
			
	// displaying images 
		SDL_RenderPresent(renderer);
		++(frames); // adds to frame tally
		
		int framet = capkeeper.getTicks(); // for capping frame rate
		if( framet < SCREEN_TICK_PER_FRAME)
			SDL_Delay( SCREEN_TICK_PER_FRAME - framet );
	
	// clears current game pointer if the current game system ended
		if(currentGame != NULL && currentGame->endSystem)
			currentGame = NULL;
}	
	
// ending game, deconstructor  
void game_handler::close()
{
	if(background_assets != NULL)
		delete background_assets;
	// destroy used texture
	SDL_DestroyTexture(texture);
	texture = NULL;
	
	//Destroy window and renderer 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
			
	// close handlers 
	Mix_Quit();
	IMG_Quit();
		
	// close main SDL extension 
	SDL_Quit();
			
	exit(0);	
}