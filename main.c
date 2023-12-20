//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"



//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;


typedef struct player  //combining several variables of different types
{
        int energy;
        int position;
        char name[MAX_CHARNAME];
        int accumCredit;
        int flag_graduate;
} player_t;

static player_t *cur_player; // pointer declared -> point player_t
//static player_t cur_player[MAX_PLAYER]; // array_allocate memory_size:[MAX_PLAYER]

#if 0
static int player_energy[MAX_PLAYER];
static int player_position[MAX_PLAYER];
static char player_name[MAX_PLAYER][MAX_CHARNAME];
#endif

//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
 //print grade history of the player
void goForward(int player, int step); //make player go "step" steps on the board (check if player is graduated)
void printPlayerStatus(void); //print all player status at the beginning of each turn
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif


void printGrades(int player)
{
     int i;
     void *gradePtr;
     for (i=0;i<smmdb_len(LISTNO_OFFSET_GRADE + player);i++)
     {
         gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
         printf("%s : %i\n", smmObj_getName(gradePtr), smmObj_getGrade(gradePtr));
     }
}

void printPlayerStatus(void)
{
     int i;

     for (i=0;i<player_nr;i++)
     // print as player number
     {
         printf("%s at position %i,credit %i, energy %i\n", 
                      cur_player[i].name,
                      cur_player[i].position,
                      cur_player[i].accumCredit,
                      cur_player[i].energy);
    	
		//void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[i].position);
        //printf("Board Name at current position: %s\n", smmObj_getName(boardPtr));
     }
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     //n time loop
     
     
     for (i=0;i<n;i++)
     {
         //input name
         printf("Input player No.%i's name:", i); // input player no.:
         scanf("%s", cur_player[i].name);
         fflush(stdin);
         
         //--------------------------------------------------------
         //printf("input done\n");
         //--------------------------------------------------------
         
         //set position
         //player_position[i] = 0;
         cur_player[i].position = 0;
         
         //set energy
         //player_energy[i] = initEnergy;
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0;
         cur_player[i].flag_graduate = 0;
     }
}


int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 1
    if (c == 'g')
        printGrades(player);
#endif
    
    return (rand()%MAX_DIE + 1);
}

//action code when a player stays at a node
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
    int type = smmObj_getNodeType( boardPtr );
    char *name = smmObj_getName( boardPtr );
    void *gradePtr;
    
    switch(type)
    {
        //case lecture:
        case SMMNODE_TYPE_LECTURE:
            cur_player[player].accumCredit += smmObj_getCredit( boardPtr );
            cur_player[player].energy -= smmObj_getEnergy( boardPtr );
            
            //grade generation
            gradePtr = smmObj_genObject(name, smmObjType_grade, 0, smmObj_getCredit( boardPtr ), 0, smmObjGrade_Ap);
            smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
            
            break;
            
        default:
            break;
    }
}

void goForward(int player, int step)
{
     void *boardPtr;

     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position );
     printf("%s is at node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getName(boardPtr));
				
	 //---Player is MOVING---
	 int i;
		for (i = 0; i < step; i++)
		// Print ALL the name of each node the player passes through
	        printf("%s is passing node %i (name: %s)\n",
	               cur_player[player].name, cur_player[player].position + i + 1,
	               smmObj_getName(smmdb_getData(LISTNO_NODE, cur_player[player].position + i + 1)));
	    
	     cur_player[player].position += step;
     
     //---UPDATE boardPtr to CHANGED POSITION
     boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position ); // <-- necessary for Chage
     printf("%s go to node %i (name: %s)\n", 
                cur_player[player].name, cur_player[player].position,
                smmObj_getName(boardPtr));
}


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    FILE* fp_food;
    FILE* fp_festival;
    char name[MAX_CHARNAME];
    char food_name[MAX_CHARNAME];
    char festival_name[MAX_CHARNAME];
    int food_energy;
    int type;
    int credit;
    int energy;
    int i;
    int initEnergy;
    int turn=0;
    
    board_nr = 0;
    food_nr = 0; // Role: Counter for numbering food items
    festival_nr = 0; // Role: Counter for numbering festival items
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    // fopen : open "BOARDFILEPATH" => marbleBoardConfig.txt
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    //read a node parameter set
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 )
    // fscanf : read contents in "marbleBoardConfig.txt"
    {
        //store the parameter set
        //(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
        void *boardObj = smmObj_genObject(name, smmObjType_board, type, credit, energy, 0);
        smmdb_addTail(LISTNO_NODE, boardObj);
        // smmObj_genObject : generate node object & add to list
        
        if (type == SMMNODE_TYPE_HOME)
           initEnergy = energy;
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    for (i = 0;i<board_nr;i++)
    {
        void *boardObj = smmdb_getData(LISTNO_NODE, i);
        
        printf(">> node %i << : %s, %i(%s), credit %i, energy %i\n", 
                     i, smmObj_getName(boardObj), 
                     smmObj_getNodeType(boardObj), smmObj_getNodeTypeName(smmObj_getNodeType(boardObj)),
                     smmObj_getCredit(boardObj), smmObj_getEnergy(boardObj));
    }
    // printf("(%s)", smmObj_getTypeName(SMMNODE_TYPE_LECTURE));
    
    
    //2. food card config 
    if ((fp_food = fopen(FOODFILEPATH,"r")) == NULL)
    // fopen : open "FOODFILEPATH" => marbleFoodConfig.txt
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        getchar(); //
        return -1; // -1 meaning??
    }
    
    printf("\n\nReading food card component......\n");
    while ( fscanf(fp_food, "%s %i", food_name, &food_energy) == 2) //read a food parameter set
    // fscanf : read contents in "marbleFoodConfig.txt"
    // marbleFoodConfig.txt : FoodName Energy
    {
        //store the parameter set
        printf("=> %d <=. Food: %s, Food_Energy: %i\n", food_nr, food_name, food_energy);
        food_nr++;

    }
    fclose(fp_food);
    printf("Total number of food items: %i\n", food_nr);
    
    
    //3. festival card config 
    // * space makes line change. Don't make space
    if ((fp_festival = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp_festival, "%s", festival_name) == 1) //read a festival card string
    {
        //store the parameter set
        printf("=> %d <=. Festival: %s\n", festival_nr, festival_name);
        festival_nr++;
    }
    fclose(fp_festival);
    printf("Total number of festival cards : %i\n", festival_nr);
    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    // print GAME TITLE HERE!!
    printf("\n=======================================================================\n");
	printf("\n****---------------------------------------------------------------****\n");
	printf("\n  Sookmyung Marble !! Let me Graduate (total credit : 30)......  T_T   \n");
	printf("\n****---------------------------------------------------------------****\n");
	printf("\n=======================================================================\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	
    do
    {
        //input player number to player_nr
        // This is where 1st input print
        printf("input no. of player(1~10):");
        scanf("%d", &player_nr);
        fflush(stdin);
    }
    while (player_nr < 0 || player_nr >  MAX_PLAYER);
    
    cur_player = (player_t*)malloc(player_nr*sizeof(player_t));		//-------------------------------------
    generatePlayers(player_nr, initEnergy);
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (1) //is anybody graduated?
    {
        int die_result;
        
        
        //4-1. initial printing
        printf("\n");
        printf("========================== PLAYER STATUS ==========================\n");
        printPlayerStatus();
        printf("========================== PLAYER STATUS ==========================\n");
        printf("\n");
        
        //4-2. die rolling (if not in experiment)        
        printf("This is >> %s << turn ::: \n", cur_player[turn].name);
		die_result = rolldie(turn);
        printf("RESULT of rolling die is %d\n", die_result);

        //4-3. go forward
        goForward(turn, die_result);

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1)%player_nr;
    }
    
    
    free(cur_player);
    system("PAUSE");
    return 0;
}


