#include "game_handler.h"

class battle : public system_handler
{
	public:
		battle(){}
		
		battle(game_handler * g)
		{
			main_game = g;
			
			combat_info = text(g->renderer,1);
			
			textArea = image("resources/sprites/battle_text_area.png",g->renderer);
			 
			border = image("resources/sprites/battle_border.png",g->renderer);
			area = image("resources/sprites/test_area.png",g->renderer);
			
			menu = image("resources/sprites/battle_menu.png",g->renderer);
			party_menu = image("resources/sprites/party_menu.png",g->renderer);
			
			enemies = new image("resources/sprites/Monster/007.png",g->renderer);
			numTypeEnemies = 1;
			
			player_port = image("resources/sprites/player_portrait.png",g->renderer);
			
			for(int i=0;i<numTypeEnemies;i++)
				enemies[i].scale = 3;
			
			textArea.scale = 3;
			border.scale = 2;
			party_menu.scale = 2;
			area.scale = 2;
		}
		
		// display battle 
		void display() override
		{
			int x;
			
			area.render(main_game->renderer,75,100);
			border.render(main_game->renderer,50,90);
				
			enemies[0].render(main_game->renderer,120,160);	
			
			for(int i=0;i<4;i++)
			{
				if(i==option)
					x = 150;
				else
					x = 100;
				
				menu.render(main_game->renderer,x,450+i*55);
				switch(i)
				{
					case 0:
					combat_info.display("Skills",x+20,460+i*55);
					break;
					case 1:
					combat_info.display("Talk",x+20,460+i*55);
					break;
					case 2:
					combat_info.display("Fusion",x+20,460+i*55);
					break;
					case 3:
					combat_info.display("Items",x+20,460+i*55);
					break;
					
				}
				
			}
			
			for(int i=0;i<partySize;i++)
			{
				party_menu.render(main_game->renderer,580,150+i*120);
				if(i==0)
					player_port.render(main_game->renderer,600,140);
				
				main_game->displayText.display("HP:10/10",730,160);
				main_game->displayText.display("PLAYER",750,220);
			
			}
			textArea.render(main_game->renderer,20,25);
			combat_info.display("TESTING",70,50);
			
		}		
	
		// handle input/logic
		void handler() override
		{
			switch(main_game->input.state)
			{
				case DOWN:
				if(option != 3)
					option++;
				break;
				case UP:
				if(option != 0)
					option--;
				break;
			}
		}
	private:
		// for displaying info on what's happening in battle  
		text combat_info; 
		
		// hold images of enemy sprites
		image * enemies; 
		// the number of types of enemies used for the enemy sprite pointer
		int numTypeEnemies;
		
		image textArea;
		image menu;
		image party_menu;
		image player_port;
		int option = 0;
		int partySize = 1;
		
		image border;
		image area;
};