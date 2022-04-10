#include "game_handler.h"

class battle : public system_handler
{
	public:
		battle(){}
		
		// adding new description dialogue for battle text 
		void actionLine(string command)
		{
			if(command.size() > 30)
			{
				lines.push_back(command.substr(0,30));
				lines.push_back(command.substr(30));
			}
			else
				lines.push_back(command);
			currentScript++;
			texttimer.start();
			currentPos = 0;
			finishedLine = false;
		}
		
		// generating/displaying battle text information
		void textInfo()
		{
			counter = (int)(16*texttimer.getTicks()/1000.f);		
			if(counter > currentPos && currentPos < lines[currentScript].size())
				currentPos++;
			
			// checks if text is done 
			if(currentPos >= lines[currentScript].size() && currentScript == lines.size()-1)
			{
				finishedLine = true;
				texttimer.stop();
			}
			
			combat_info.display(lines[currentScript].substr(0,currentPos),70,50);		
		}
		
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
			
			endHeart = image("resources/sprites/heart.png",g->renderer);
			
			player_port = image("resources/sprites/player_portrait.png",g->renderer);
			
			for(int i=0;i<numTypeEnemies;i++)
				enemies[i].scale = 3;
			
			textArea.scale = 3;
			border.scale = 2;
			party_menu.scale = 2;
			area.scale = 2;
			
			texttimer.start();
			
			lines.push_back("Encountered a Stranjger!");
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
				
				combat_info.display("HP:10/10",750,160);
				combat_info.display("STAMINA:10",750,205);
				combat_info.display("PLAYER",750,250);
			
			}
			textArea.render(main_game->renderer,20,25);
			textInfo();
			
			if(finishedLine)
			{
				endHeart.render(main_game->renderer,600,95+pointy);
				
				// end heart up and down animation 
				if(pointy > 5)
					reverse = true;
				else if(pointy < -5)
					reverse = false;
			
				if(reverse)
					pointy-=1;
				else
					pointy+=1;
			}
		}		
	
		// handle input/logic
		void handler() override
		{
			if(finishedLine)
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
					case SELECT:
					actionLine("You did something!");
					break;
				}
			}
		}
	private:
		//--------------image rendering variables--------------
			// hold images of enemy sprites
			image * enemies; 
			// the number of types of enemies used for the enemy sprite pointer
			int numTypeEnemies;
		
			// images used for UI
			image textArea;
			image menu;
			image party_menu;
			image player_port;
			image border;
			image area;
			image endHeart;
		
			// the option the player selects for combat 
			int option = 0;
		
			// animation variables for movement of heart at the end of line 
			double pointy = 0;
			bool reverse = true;
		
		//--------------text rendering variables--------------			
			// for displaying info on what's happening in battle  
			text combat_info; 
			
			// timer for keeping track of speed for displaying tex	t
			timer texttimer; 
		
			// rate variable for showing characters 
			int counter = 0;
		
			// current character in the string and the counter for what is displayed
			int currentPos = 0;
			
			// the text being displayed
			vector<string> lines = vector<string>();
			
			// current line being read
			int currentScript = 0;
			
			bool finishedLine = false;
		
		//--------------testing/debugging variables--------------		
			// the party size (for debugging)
			int partySize = 1;
};