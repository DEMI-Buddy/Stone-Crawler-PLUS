// for holding basic stats that are used in combat
enum move_types
{
	SLASH = 1,
	PIERCE = 2,
	BLUDGEON = 3,
	RANGED = 4,
	SPELL = 5,
	STRATEGIES = 6
};

// different characters
enum race
{
	PROTAGONIST = -1,
	COBOL = 1,
	TRAOTIC = 2,
	TINKER_TOY = 3,
	LOATING = 4,
	STRANJER = 5
};

struct weapon
{
	string name;
	
	// attack type of weapon 
	move_types type;
	
	// weapon strength 
	int strength;
	
	// durability of the weapon 
	int health;
	int max_health;
};

struct moves
{
	move_types type;
	string name;
};

class stats 
{
	public:
		stats(){}
		
		stats(SDL_Renderer * g, race pick)
		{
			type = pick;
			abilities = new moves[4];
			
			switch(pick)
			{
				case PROTAGONIST:
				maxHealth = rand()%5+5;
				maxStamina = rand()%5+5;
			
				maxStr = rand()%5+10;
				maxDef = rand()%5+1;
				maxAgil = rand()%5+1;
			
				health = maxHealth;
				stamina = maxStamina;
			
				strength = rand()%maxStr+1;
				defense = rand()%maxDef+1;
				agility = rand()%maxAgil+1;
				
				portCoords[0] = 67;
				portCoords[1] = 226;
				sprite = image("resources/sprites/battle/Monster/kid.png",g);
				
				abilities[0] = {SLASH, "Slash"};
				abilities[1] = {PIERCE, "Pierce"};
				abilities[2] = {SPELL, "Fusion"};
				abilities[3] = {SPELL, "Heal"};
				
				numMoves = 4;
				break;
				
				case COBOL:
				maxHealth = rand()%1+1;
				maxStamina = rand()%7+5;
			
				maxStr = rand()%3+1;
				maxDef = rand()%7+1;
				maxAgil = rand()%2+1;
			
				health = maxHealth;
				stamina = maxStamina;
			
				strength = rand()%maxStr+1;
				defense = rand()%maxDef+1;
				agility = rand()%maxAgil+5;
				
				portCoords[0] = 20;
				portCoords[1] = 33;
				sprite = image("resources/sprites/battle/Monster/Cobol.png",g);
				
				abilities[0] = {SLASH, "Slash"};
				
				numMoves = 1;
				break;
				
				case TINKER_TOY:
				maxHealth = rand()%20+15;
				maxStamina = rand()%17+5;
			
				maxStr = rand()%10+1;
				maxDef = rand()%7+1;
				maxAgil = rand()%3+1;
			
				health = maxHealth;
				stamina = maxStamina;
			
				strength = rand()%maxStr+1;
				defense = rand()%maxDef+1;
				agility = rand()%maxAgil+1;
				
				portCoords[0] = 33;
				portCoords[1] = 93;
				sprite = image("resources/sprites/battle/Monster/Tinker Toy.png",g);
				
				abilities[0] = {SPELL, "Spirit"};
				
				numMoves = 1;
				break;
				
				case LOATING:
				maxHealth = rand()%5+7;
				maxStamina = rand()%17+5;
			
				maxStr = rand()%10+1;
				maxDef = rand()%7+1;
				maxAgil = rand()%3+1;
			
				health = maxHealth;
				stamina = maxStamina;
			
				strength = rand()%maxStr+1;
				defense = rand()%maxDef+1;
				agility = rand()%maxAgil+1;
				
				portCoords[0] = 93;
				portCoords[1] = 123;
				sprite = image("resources/sprites/battle/Monster/Loating.png",g);
				
				abilities[0] = {SPELL, "Spirit"};
				
				numMoves = 1;
				break;
			
				case TRAOTIC:
				maxHealth = rand()%20+15;
				maxStamina = rand()%17+5;
			
				maxStr = rand()%10+1;
				maxDef = rand()%7+1;
				maxAgil = rand()%3+1;
			
				health = maxHealth;
				stamina = maxStamina;
			
				strength = rand()%maxStr+1;
				defense = rand()%maxDef+1;
				agility = rand()%maxAgil+1;
				
				portCoords[0] = 123;
				portCoords[1] = 63;
				sprite = image("resources/sprites/battle/Monster/Traotic.png",g);
				
				abilities[0] = {SPELL, "Spirit"};
				abilities[1] = {SLASH, "Slash"};
				
				numMoves = 2;
				break;
				case STRANJER:
				maxHealth = 1;//rand()%20+15;
				maxStamina = rand()%17+5;
			
				maxStr = rand()%10+1;
				maxDef = rand()%7+1;
				maxAgil = rand()%3+1;
			
				health = maxHealth;
				stamina = maxStamina;
			
				strength = rand()%maxStr+1;
				defense = rand()%maxDef+1;
				agility = rand()%maxAgil+1;
				
				portCoords[0] = 0;
				portCoords[1] = 0;
				sprite = image("resources/sprites/battle/Monster/stranjer.png",g);
				
				abilities[0] = {SPELL, "Spirit"};
				abilities[1] = {SLASH, "Slash"};
				
				numMoves = 2;
				break;
				
			}
			maxDealtDam = strength*20;
			maxDam = maxHealth*4;
			maxExpStam = maxStamina*5;
		}
		
		// party portrait display 
		void portraitDisplay(SDL_Renderer * g, int x, int y,bool bye = false)
		{
			if(!bye)
				sprite.setAlpha(255);
			SDL_Rect port = {portCoords[0],portCoords[1],130,130};
			sprite.render(g,x,y,&port);
		}
		
		// comparison for turn order
		bool operator > (const stats &b) const
		{
			int Val = stamina*agility + health;
			int bVal = b.stamina*b.agility + b.health;
			return (Val > bVal);
		}
		bool operator < (const stats &b) const
		{
			int Val = stamina*agility + health;
			int bVal = b.stamina*b.agility + b.health;
			return (Val < bVal);
		}
		
		string name = "";
	
		// character stats
		int strength = 1;
		int defense = 1;
		int agility = 1;
		
		int health = 7;
		int maxHealth = 7;
		
		int stamina = 10;
		int maxStamina = 10;
		
		// the weapon the character is holding 
		weapon holding;
		
		// regular moves 
		moves * abilities;
		int numMoves;
		
		// used for identifying the sprite and starting stats of the character
		race type;
		
		// the sprite of the character
		image sprite;
	
		int maxStr = 2;
		int maxDef = 2;
		int maxAgil = 2;
		
		// for upgrading strength
		int dealtdamageNeeded = 0;
		int maxDealtDam = 0;
		
		// for upgrading defense 
		int damageNeeded = 0;
		int maxDam = 0;
		
		// for upgrading agility 
		int expiredStam = 0;
		int maxExpStam = 0;
		
		// scars accumulated
		int scars = 0;
		
		// position of sprite on the battle screen as an enemy 
		int battleSpriteCoords[2];
		
		// used in battle to keep track of enemy ai turns 
		bool isEnemy = false;
		
		private:
		// coordinates for the party portrait sprite 	
		int portCoords[2];
	
};