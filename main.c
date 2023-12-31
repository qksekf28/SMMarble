//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//
//=========================================
// UNSOLVED PROBLEM
// -> 단순 30을 넘기는게 아니라 30도 넘기고 집도 가야 종료 => SOLVED
//=========================================

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"8
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

#define REPLENISH_ENERGY_AMOUNT 18

//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;

static int player_nr;

int turn=0;

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
void takeLectureAction(int player, void *boardPtr);
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

//=========================================================
// For PlayerStatus
// -> calcAverageGrade : 학점 평균 계산 
// -> printPlayerStatus : 턴 시작 시 모든 플레이어의 상태 (위치, 실험 중 상태, 누적 학점, 현재 에너지)를 출력해야 함
//=========================================================
float calcAverageGrade(int player)
{
    int i;
    float totalGrade = 0.0;

    // 학점 기록의 개수가 0인 경우 평균 학점은 0.0
    if (smmdb_len(LISTNO_OFFSET_GRADE + player) == 0)
    {
        return 0.0;
    }

    // SUM every credit
    for (i = 0; i < smmdb_len(LISTNO_OFFSET_GRADE + player); i++)
    {
        void *gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        totalGrade += smmObj_getGrade(gradePtr);
    }

    // calculate average grade
    return totalGrade / smmdb_len(LISTNO_OFFSET_GRADE + player);
}


void printPlayerStatus(void)
{
    int i;

    for (i = 0; i < player_nr; i++)
    // print as player number
    {   
		//void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[i].position);
        //printf("Board Name at current position: %s\n", smmObj_getName(boardPtr));
        //%s at position %i, credit %i, energy %i\n
        void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[i].position);
        
        printf("%s at position %i.%s, credit %i, energy %i\n"
        	   "Average Grade: %.2f\n"
               "Grade History:\n",
               cur_player[i].name,
               cur_player[i].position,
               smmObj_getName(smmdb_getData(LISTNO_NODE, cur_player[i].position)),
               cur_player[i].accumCredit,
               cur_player[i].energy,
			   (cur_player[i].flag_graduate == 1) ? "Graduated" : "Not Graduated",
               calcAverageGrade(i));
        printGrades(i);
        printf("\n");
    }
}
//=========================================================


//=========================================================
// For GRADUATE
// -> isGraduated : check condition(accumCredit & position)
//=========================================================
int isGraduated(int player)
{
    if (cur_player[player].accumCredit >= GRADUATE_CREDIT && cur_player[player].position == 0) {
        return 1; // Condition satisfy -> 1
    }
    return 0; // Not yet -> 0
}
//=========================================================


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

         //set current position/energy/accumCredit/flag_graduate
         cur_player[i].position = 0;
         cur_player[i].energy = initEnergy;
         cur_player[i].accumCredit = 0;
         cur_player[i].flag_graduate = 0;
     }
}

//=========================================================
// DICE
//=========================================================
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
//=========================================================


// findGrade: Find and return the grade corresponding to the given player and lecture name
void* findGrade(int player, const char *lectureName)
{
    int i;
    void *gradePtr;

    // Iterate through the player's grade list and compare lecture names
    for (i = 0; i < smmdb_len(LISTNO_OFFSET_GRADE + player); i++)
    {
        gradePtr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);

        // Return the grade if the lecture name matches
        if (strcmp(smmObj_getName(gradePtr), lectureName) == 0)
        {
            return gradePtr;
        }
    }

    // Return NULL if no grade corresponding to the lecture name is found
    return NULL;
}


// generate random grade
smmObjGrade_e generateRandomGrade(void)
{
    smmObjGrade_e grades[] = {smmObjGrade_Ap, smmObjGrade_A0, smmObjGrade_Am,
                              smmObjGrade_Bp, smmObjGrade_B0, smmObjGrade_Bm,
                              smmObjGrade_Cp, smmObjGrade_C0, smmObjGrade_Cm};
    return grades[rand() % (sizeof(grades) / sizeof(grades[0]))];
}

// takeLectureAction : lecture condition check & generate grade
void takeLectureAction(int player, void *boardPtr)
{
    int lectureEnergy = smmObj_getEnergy(boardPtr);
    int lectureCredit = smmObj_getCredit(boardPtr);

    if (cur_player[player].energy < lectureEnergy)
    {
        printf("Not enough energy to take the lecture: %s\n", smmObj_getName(boardPtr));
        return;
    }

    if (findGrade(player, smmObj_getName(boardPtr)) != NULL)
    {
        printf("You have already taken this lecture: %s\n", smmObj_getName(boardPtr));
        return;
    }

    char answer;
    printf("Do you want to take the lecture (y/n)? ");
    scanf(" %c", &answer);

    if (answer == 'y' || answer == 'Y')
    {
        smmObjGrade_e randomGrade = generateRandomGrade();
        void *gradePtr = smmObj_genObject(smmObj_getName(boardPtr), smmObjType_grade, 0, lectureCredit, 0, randomGrade);
        smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
        
        printf("You have successfully taken the lecture: %s\nGrade:  %s \n", smmObj_getName(boardPtr), gradeToString(randomGrade));
        cur_player[player].accumCredit += lectureCredit;
        cur_player[player].energy -= lectureEnergy;
    }
    else
    {
        printf("You have chosen to drop the lecture: %s\n", smmObj_getName(boardPtr));
    }
}


//=========================================================
// For SMMNODE_TYPE_FOODCHANCE 5
//	-> handleFoodChance : get random food card & display FOOD/ENERGY & update energy status
//=========================================================
void handleFoodChance(int player)
{
	char food_name[MAX_CHARNAME];
    int food_energy;
    printf("%s landed on a Food Chance node!\n", cur_player[player].name);

    // read food parameter from marbleFoodConfig.txt
    FILE *fp_food;
    if ((fp_food = fopen(FOODFILEPATH, "r")) == NULL) {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return;
    }

    // count number of food items
    int food_nr = 0; // but it's already static int
    while (fscanf(fp_food, "%s %*d", food_name) == 1) {
        food_nr++;
    }
    fclose(fp_food);

    // choose random food item
    int randomFoodIndex = rand() % food_nr;

    // Read CHOSEN random food item
    if ((fp_food = fopen(FOODFILEPATH, "r")) == NULL) {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return;
    }
    
    int i;
    for (i = 0; i <= randomFoodIndex; i++)
	{
        fscanf(fp_food, "%s %d", food_name, &food_energy);
    }
    fclose(fp_food);

    // Update player ENERGY
    printf("  -> %s got a food card: %s. Energy increased by %d.\n", cur_player[player].name, food_name, food_energy);
    cur_player[player].energy += food_energy;
}
//=========================================================



//=========================================================
// For SMMNODE_TYPE_FESTIVAL 6
//	-> handleFestival : get random food card & display FOOD/ENERGY & update energy status
//=========================================================
void handleFestival(int player)
{
    printf("%s landed on a Festival node!\n", cur_player[player].name);

    // read mission parameter from marbleFestConfig.txt
    FILE *fp_festival;
    if ((fp_festival = fopen(FESTFILEPATH, "r")) == NULL)
    {
        printf("[ERROR] failed to open %s. Check the file path.\n", FESTFILEPATH);
        return;
    }

    // count number of mission
    int randomMissionIndex = rand() % festival_nr;

    if ((fp_festival = fopen(FESTFILEPATH, "r")) == NULL)
    {
        printf("[ERROR] failed to open %s. Check the file path.\n", FESTFILEPATH);
        return;
    }

    // Read CHOSEN random food item
    int i;
    char festivalMission[MAX_CHARNAME];
    for (i = 0; i <= randomMissionIndex; i++)
    {
        fscanf(fp_festival, "%s", festivalMission);
    }
    fclose(fp_festival);

    // ---MISSION-ing---
    printf("MISSION : %s\n", festivalMission);
    printf("(Press any key when mission is ended.)  <3 \n");
    getchar(); // Waiting for answer

    turn = (turn + 1) % player_nr;
}
//=========================================================


//=========================================================
void actionNode(int player)
{
    void *boardPtr = smmdb_getData(LISTNO_NODE, cur_player[player].position);
    int type = smmObj_getNodeType(boardPtr);

    switch (type)
    {
    	// !!!!!!!!!!!!! case need : not ; !!!!!!!!!!!!!!! CHECK ALWAYS !!!!!!!
        case SMMNODE_TYPE_LECTURE:
            takeLectureAction(player, boardPtr);
            break;
            
        case SMMNODE_TYPE_RESTAURANT:
        	// current energy + getenergy of food
            printf("  -> %s went to a restaurant(%s). Energy replenished by %d.\n",
								cur_player[player].name, smmObj_getName(boardPtr), smmObj_getEnergy(boardPtr));
            cur_player[player].energy += smmObj_getEnergy(boardPtr);
            break;
        
        case SMMNODE_TYPE_HOME:
        	cur_player[player].energy += REPLENISH_ENERGY_AMOUNT;
            break;
            
        case SMMNODE_TYPE_FOODCHANCE:
        	handleFoodChance(player);
        	break;
        	
        case SMMNODE_TYPE_FESTIVAL:
        	handleFestival(player);
        	break;
        	
        // Another Type Node

        default:
    
	break;
    }
}
//=========================================================


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
		{
        	// Print ALL the name of each node the player passes through
        	cur_player[player].position = (cur_player[player].position + 1) % board_nr;
			// Wrap around to the first node if reaching the last node
        	printf("%s is passing node %i (name: %s)\n",
            cur_player[player].name, cur_player[player].position,
            smmObj_getName(smmdb_getData(LISTNO_NODE, cur_player[player].position)));
    	}
		
		/*
		// Print ALL the name of each node the player passes through
	        printf("%s is passing node %i (name: %s)\n",
	               cur_player[player].name, cur_player[player].position + i + 1,
	               smmObj_getName(smmdb_getData(LISTNO_NODE, cur_player[player].position + i + 1)));
	    
	     cur_player[player].position += step;
	     */
     
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
    //int turn=0;
    
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
        
        
        // Check if the current player has graduated and is at the first node
		if (isGraduated(turn))
		{
		    printf("Game over! Player %s has graduated and is at the first node.\n", cur_player[turn].name);
		    break; // End the game
		}
        
    }
    
    free(cur_player);
    system("PAUSE");
    return 0;
}
