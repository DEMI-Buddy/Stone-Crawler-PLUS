#include "game_handler.h"

class title_screen : public system_handler
{
	public:
		title_screen(){}
		
		title_screen(game_handler * g)
		{
			main_game = g;
			test = image("resources/sprites/kid.png",g->renderer);
			test2 = image("resources/sprites/stone_crawler.png",g->renderer);
			test_logo = image("resources/sprites/logo.png",g->renderer);
			
			megaAlpha = 0;
			
			loadIn = true;
		}
		
		void display() override
		{
			if(!kidLoaded)
				test_logo.setAlpha(megaAlpha);
			test.setAlpha(megaAlpha);
			test2.setAlpha(megaAlpha);
			
			if(!loadMonster)
				test.render(main_game->renderer,200,0);
			else
				test2.render(main_game->renderer,0,0);
				
			test_logo.render(main_game->renderer,400,300);
			
			if(!loadedMonster)
			{
				if(loadIn && megaAlpha == 255)
				{
					if(loadMonster)
						loadedMonster = true;
					loadIn = false;
					kidLoaded = true;
					switchOut = true;
				}
				else if(loadIn)
					megaAlpha+=1;
				else if(switchOut && megaAlpha == 0)
				{	
					switchOut = false;
					loadMonster = true;
					loadIn = true;
				}
				else if(switchOut)
					megaAlpha-=5;
			}
			else
			{
				main_game->displayText.display("New Game",600,520,angleSelect[0]);
				main_game->displayText.display("Continue",600,590,angleSelect[1]);
			}
		}		
	
		// handle input/logic
		void handler() override
		{
			if(loadedMonster)
			{
				switch(main_game->input.state)
				{
					case UP:
					angleSelect[option]=0;
					if(option != 0)
						option--;
					break;
					case DOWN:
					angleSelect[option]=0;
					if(option != 1)
						option++;
					break;
				}
				angleSelect[option]+=3;
				angleSelect[option]%=360;
				
			}
			
		}
	private:
	
		// option on the title screen
		int option = 0;
		
		// for spinning the current selected option
		int angleSelect[2] = {0,0};
		
		// variables used for animation timing
		bool textPart = true;
			
		bool loadIn = false;
		bool switchOut = false;
		int megaAlpha = 0;
		
		bool kidLoaded = false;
		bool loadMonster = false;
		bool loadedMonster = false;
		
		// images used
		image test;
		image test2;
		image test_logo;
};
