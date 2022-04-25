#include "game_handler.h"
#include "models.h"
#include <algorithm> // vector sort function
#include <utility>  // pair 

// player information/stats
stats * party;
int numParty;

enum menuStatus
{
	FIGHT_OVER = -2,
	NOT_TURN = -1,
	SELECTION = 0,
	SKILLS = 1
};

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
			if(currentPos >= lines[currentScript].size())
			{
				finishedLine = true;
				texttimer.stop();
			}
			
			if(currentScript == lines.size()-1 && finishedLine)
				inputGo = true;

			combat_info.display(lines[currentScript].substr(0,currentPos),70,50);		
		}
		
		// used for sorting the turn order 
		struct turn_order
		{
			bool operator()(const stats * a, const stats * b)
			{ 
				return (*a < *b);
			}
		};
		
		// constructor
		battle(game_handler * g)
		{
			main_game = g;
			
			// main text used 
			combat_info = text(g->renderer,1);
			
			// set up image info needed
			textArea = image("resources/sprites/battle_text_area.png",g->renderer);
			 
			border = image("resources/sprites/battle_border.png",g->renderer);
			area = image("resources/sprites/test_area.png",g->renderer);
			
			menu = image("resources/sprites/battle_menu.png",g->renderer);
			party_menu = image("resources/sprites/party_menu.png",g->renderer);
			
			endHeart = image("resources/sprites/heart.png",g->renderer);
			
			player_port = image("resources/sprites/player_portrait.png",g->renderer);
					
			textArea.scale = 3;
			border.scale = 2;
			party_menu.scale = 2;
			area.scale = 2;
			
			texttimer.start();
			
			// set up enemy information 
			enemySide = new stats[4];
			indivAlpha = new pair<bool, int>[4]; 
			enemySide[0] = stats(g->renderer,STRANJER);
			enemySide[0].name = "Stranjer";
			numEnemies = 1;
			
			// sprite values 
			for(int i=0;i<numEnemies;i++)
			{
				indivAlpha[i].first = false;
				indivAlpha[i].second = 255;
			}
			
			// start load in animation
			loadIn = true;
			
			// first lines in combat
			lines.push_back("Encountered a " + enemySide[0].name);
			
			// set up turn calculating 
			turnOrder = vector<stats*>();
			
			// add all party members and enemies to turn variable 
			for(int i=0;i<numParty;i++)
			{
				party[i].isEnemy = false;
				turnOrder.push_back(&party[i]);
			}
			
			for(int i=0;i<numEnemies;i++)
			{
				enemySide[i].isEnemy = true;
				turnOrder.push_back(&enemySide[i]);
			}
			
			// sort initial turn order 
			sort(turnOrder.begin(),turnOrder.end(),turn_order());
			
			// debug starting turn order 
			for(int i=0;i<turnOrder.size();i++)
			{ 
				cout << "\n" << i << ":" << turnOrder[i]->name << " TURN MATH:" << turnOrder[i]->stamina*turnOrder[i]->agility + turnOrder[i]->health;;
			}
		}

		// for displaying party/player options menu
		void partyOption(int x)
		{
			for(int i=0;i<3;i++)
			{
				if(i==option)
					x = 60;
				else
					x = 10;
				
				menu.render(main_game->renderer,x,500+i*55);
				switch(i)
				{
					case 0:
					combat_info.display("Skills",x+20,510+i*55);
					break;
					case 1:
					combat_info.display("Talk",x+20,510+i*55);
					break;
					case 2:
					combat_info.display("Items",x+20,510+i*55);
					break;
				}
			}	
			
			switch(currentSelection)
			{
				case SKILLS:
				for(int i=0;i<currentChar->numMoves;i++)
				{
					if(i==option)
						x = 300;
					else
						x = 350;
				
					menu.render(main_game->renderer,x,500+i*55);
					combat_info.display(currentChar->abilities[i].name,x+20,510+i*55);	
				}	
				break;				
			}
		}
		
		// display battle 
		void display() override
		{
			int x;
			
			// for loading in animation 
			if(loadIn || switchOut)
			{
				area.setAlpha(megaAlpha);
				border.setAlpha(megaAlpha);
				menu.setAlpha(megaAlpha);
				combat_info.textColor.a = megaAlpha;
				party_menu.setAlpha(megaAlpha);
				
				for(int i=0;i<numParty;i++)
					party[i].sprite.setAlpha(megaAlpha);
				for(int i=0;i<numEnemies;i++)
					enemySide[i].sprite.setAlpha(megaAlpha);
				
				textArea.setAlpha(megaAlpha);
			}
			
			// main area/border
			area.render(main_game->renderer,35,170);
			border.render(main_game->renderer,10,160);
			
			// display enemy sprites 			
			for(int i=0;i<numEnemies;i++)
			{
				if(indivAlpha[i].first)
				{
					if(indivAlpha[i].second != 0)
						indivAlpha[i].second-=5;
					enemySide[i].sprite.setAlpha(indivAlpha[i].second);
				}
				enemySide[i].sprite.render(main_game->renderer,200+100*i,300);
			}
			
			// party information
			for(int i=0;i<numParty;i++)
			{
				if(currentChar == &party[i])
					x=495;
				else
					x=595;
				
				party_menu.render(main_game->renderer,x,120+150*i); 
				party[i].portraitDisplay(main_game->renderer,x+10,125+150*i);
				combat_info.display(party[i].name,x+150,220+150*i);	
				combat_info.display("HP:" + to_string(party[i].health)+"/"+to_string(party[i].maxHealth),x+145,130+150*i);
				combat_info.display("STAMINA:" + to_string(party[i].stamina)+"/"+to_string(party[i].maxStamina),x+145,175+150*i);
			}
			
			// display for specific turn stuff  
			if(finishedLine && (currentScript+1 == lines.size()) && currentSelection != FIGHT_OVER && currentSelection != NOT_TURN)
				partyOption(x);
			
			// text battle info 
			textArea.render(main_game->renderer,20,5);
			textInfo();
			
			// cursor at the end of a finished line 
			if(finishedLine)
			{
				endHeart.render(main_game->renderer,600,75+pointy);
				
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
			
			// loading in and switching out battle system animatino 
			if(loadIn && megaAlpha == 255)
				loadIn = false;
			else if(loadIn)
				megaAlpha+=5;
			else if(switchOut && megaAlpha == 0)
			{
				megaAlpha = 255;
				switchOut = false;
				endSystemHandler();
			}
			else if(switchOut)
				megaAlpha-=5;
		}		
	
		// for handling character turn operations 
		void turn_character_handler()
		{
			if(turnOrder.size() == 0) // sets new turn order if all turns are done 
			{
				for(int i=0;i<numParty;i++)
					turnOrder.push_back(&party[i]);
			
				for(int i=0;i<numEnemies;i++)
					turnOrder.push_back(&enemySide[i]);
			
				sort(turnOrder.begin(),turnOrder.end(),turn_order());
				
				// debug view of turn order 
				cout << "\n\n";
				for(int i=0;i<turnOrder.size();i++)
					cout << "\n" << i << ":" << turnOrder[i]->name << " TURN MATH:" << turnOrder[i]->stamina*turnOrder[i]->agility + turnOrder[i]->health;;
			}
			
			// the current character in the turn order 
			currentChar = turnOrder.back();
			
			if(startTurn && currentChar->isEnemy)// enemy turn
			{
				lines.push_back("It's " + currentChar->name + "'s turn.");
				currentChar->stamina--;
				startTurn = false;
				endTurn = true;
				currentSelection = NOT_TURN;
			}
			else if(startTurn)// player party character turn 
			{
				lines.push_back("It's " + currentChar->name + "'s turn.");
				startTurn = false;
				endTurn = true;
				currentSelection = SELECTION;
			}
			
		}
	
		// handle input/logic
		void handler() override
		{
			// damage calculation
			int damage;
			
			// input for dialogue 
			if(finishedLine && !(currentScript+1 == lines.size())) 
			{
				switch(main_game->input.state)
				{
					case SELECT: // if the line is done, pressing enter/select goes to the next line 
					currentScript++;
					currentPos = 0;
					finishedLine = false;
					texttimer.start();
					break;
				}
			}
			else if(!finishedLine && main_game->input.state == SELECT) // complete line 
			{
				currentPos = lines[currentScript].size();
				finishedLine = true;
			}
			else if(finishedLine)
			{
				if(main_game->input.state == SELECT && finishedLine && currentSelection == FIGHT_OVER) // end combat 
					switchOut = true;
				else if(currentSelection == SELECTION && (!loadIn && !switchOut && inputGo)) // input for basic menu when all dialogue is done 
				{
					switch(main_game->input.state)
					{
						case DOWN:
						if(option != 2)
							option++;
						break;
						case UP:
						if(option != 0)
							option--;
						break;
						case SELECT:
						currentSelection = static_cast<menuStatus>(option+1);
						break;
					}
				}
				else if(currentSelection != SELECTION && currentSelection != NOT_TURN) // menu selection for other options 
				{
					switch(main_game->input.state)
					{
						case SELECT:
						damage = rand()%(currentChar->strength)+1;
						lines.push_back(enemySide[0].name + " took "+ to_string(damage)+ " damage!");
						enemySide[0].health -= damage;
						
						if(enemySide[0].health <= 0)
							indivAlpha[0].first = true;
						
						// move to next turn and read the line of the command being done 
						turnOrder.pop_back();
						startTurn = true;
						
						currentScript++;
						currentPos = 0;
						finishedLine = false;
						texttimer.start();
					
						break;
						case CANCEL:
						currentSelection = SELECTION;
						break;
					}
				}
				else if(currentSelection == NOT_TURN && main_game->input.state == SELECT) // press enter after reading dialogue on enemy turn 
				{
					turnOrder.pop_back();
					startTurn = true;
						
					currentScript++;
					currentPos = 0;
					finishedLine = false;
					texttimer.start();
					
				}
			}
			
			// checks if the fight is over 
			if(currentSelection != FIGHT_OVER && !switchOut && enemySide[0].health <= 0)
			{
				startTurn = false;
				endTurn = false;
				lines.push_back("You defeated the " + enemySide[0].name);
				currentSelection = FIGHT_OVER;
			}
			
			turn_character_handler();
		}
	private:
		//--------------battle operation variables--------------
			// enemy stats
			stats * enemySide;
			int numEnemies;
			
			// overall combat turn order 
			vector<stats*> turnOrder;
			
			// current character in the turn order
			stats * currentChar;
			
			// handling menu selections
			menuStatus currentSelection = NOT_TURN;
			
			// booleans used for timing the start/end of a character turn 
			bool startTurn = true;
			bool endTurn = false;
		
		//--------------image rendering variables--------------		
			// hold images of enemy sprites
			image * enemies; 
			// used for each enemy sprite's alpha 
			pair<bool, int> * indivAlpha; 
			
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
			
			// for animation timing when loading in and switching out 
			bool loadIn = false;
			bool switchOut = false;
			int megaAlpha = 0;
		
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
			
			// is the line being read done?
			bool finishedLine = false;
			
			// can the player input stuff now that the lines are done?
			bool inputGo = false;
};