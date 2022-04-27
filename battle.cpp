#include "battle_system.h"

//g++ battle.cpp -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -static-libstdc++ -std=c++11 -Wl,-rpath,./ -o battle

// stat viewing
class menu : public system_handler
{
	public:
		menu(){}
		
		menu(game_handler * g)
		{
			main_game = g;
			party_menu = image("resources/sprites/party_menu.png",g->renderer);
		
			combat_info = text(g->renderer,1);
			party_menu.scale = 2;
			option = numParty;
		}
		
		void display() override
		{
			int menx;
			int meny = 20;
			
			if(option < numParty)
			{	
				// display character sprite
				if(party[option].type == COBOL)
					examY = 300;
				else if(party[option].type == TRAOTIC)
					examY = 150;
				else
					examY = 100;
				
				party[option].sprite.setAlpha(100);
				party[option].sprite.render(main_game->renderer,10,examY);
				party[option].sprite.setAlpha(255);
			}

			// display party 
			for(int i=0;i<numParty;i++)
			{
				if(i==option)
					menx = 500;
				else
					menx = 580;
				
				party_menu.render(main_game->renderer,menx,meny+150*i); 
				party[i].portraitDisplay(main_game->renderer,menx+10,meny+5+150*i);
				combat_info.display(party[i].name,menx+150,meny+100+150*i);	
				combat_info.display("HP:" + to_string(party[i].health)+"/"+to_string(party[i].maxHealth),menx+150,meny+10+150*i);
				combat_info.display("STAMINA:" + to_string(party[i].stamina)+"/"+to_string(party[i].maxStamina),menx+150,meny+55+150*i);
			}
			if(option==numParty)
				menx = 500;
			else
				menx = 580;
			party_menu.scale = 1;
			party_menu.render(main_game->renderer,menx,meny+150*numParty); 
			combat_info.display("BACK",menx+20,meny+20+150*numParty);	
				
			if(option < numParty)
			{	
				// display stats
				combat_info.display("ATT:" + to_string(party[option].strength) + "/" + to_string(party[option].maxStr),10,40);
				combat_info.display("DEF:" + to_string(party[option].defense) + "/" + to_string(party[option].maxDef),10,80);
				combat_info.display("AGI:" + to_string(party[option].agility) + "/" + to_string(party[option].maxAgil),10,120);
				
				combat_info.display("Dealt Damage:" + to_string(party[option].dealtdamageNeeded) + "/" + to_string(party[option].maxDealtDam),10,550);
				combat_info.display("Damage Taken:" + to_string(party[option].damageNeeded) + "/" + to_string(party[option].maxDam),10,590);
				combat_info.display("Times Exhausted:" + to_string(party[option].expiredStam) + "/" + to_string(party[option].maxExpStam),10,630);
				
				combat_info.display("Scars:" + to_string(party[option].scars),320,550);				
				
				// display moves 
				combat_info.display("Moves:",200,40);
				for(int i=0;i<party[option].numMoves;i++)
				{
					party_menu.render(main_game->renderer,300,20+i*70); 
					combat_info.display(party[option].abilities[i].name,320,40+i*70);	
				}
			}	
			party_menu.scale = 2;
		}
		
		void handler() override
		{
			switch(main_game->input.state)
			{
				case UP:
				if(option != 0)
					option--;
				break;
				case DOWN:
				if(option != numParty)
					option++;
				break;
				case SELECT:
				if(option == numParty)
					endSystemHandler();
				break;
			}
		}
	private:
		int examY;
		
		int option = 0;
	
		// images used for UI
		image party_menu;
		
		// for displaying info on what's happening in battle  
		text combat_info; 
};

// game over 
class game_over : public system_handler
{
	public:	
		game_over(){}
		game_over(game_handler * g)
		{
			main_game = g;
		}
		void display() override
		{
			main_game->displayText.display("THE GAME IS OVER.",400,400);
		}
		
};

int main(int argc, char *argv[])
{
	srand((unsigned)time(NULL));
	
	game_handler game = game_handler(3);
	game.backgroundOn = true;

	party = new stats[4];
	numParty = 1;
	
	party[0] = stats(game.renderer,PROTAGONIST);
	/*party[1] = stats(game.renderer,COBOL);
	party[2] = stats(game.renderer,TINKER_TOY);
	party[3] = stats(game.renderer,TRAOTIC);
	*/
	
	party[0].name = "TWILIGHT";
	/*party[1].name = "COBOL";
	party[2].name = "TINKER TOY";
	party[3].name = "TRAOTIC";
	*/
	
	battle * test = new battle(&game);
	
	game_over go;
	menu test2;
	
	game.currentGame = test;
	
	while(game.input.state != EXIT)
	{
		if(test->endSystem)
		{
			bool over = true;
			test->endSystem = false;
			
			// checks if the player wasn't defeated
			for(int i=0;i<numParty;i++)
			{
				if(party[i].health > 0)
					over = false;	
			}
				
			if(over)
			{
				go = game_over(&game);
				game.currentGame = &go;
				game.switchBackground(2);
			}
			else
			{
				test2 = menu(&game);
				game.currentGame = &test2;
				game.switchBackground(0);
			}				
		}
		
		if(test2.endSystem)
		{
			delete test;
			test = new battle(&game);
			game.currentGame = test;
			test2.endSystem = false;
			game.switchBackground(3);
		}
		game.runSystem();
	}
	
	game.close();
	return 0;
}