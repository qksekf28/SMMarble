//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100


static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
   "**LECTURE**",
   "**RESTAURANT**",
    "**LABORATORY**",
    "**HOME**",
    "**MOVE to LAB**",
    "**FOOD CHANCE**",
    "**FESTIVAL**"
};

char* smmObj_getNodeTypeName(int type)
{
      return (char*)smmNodeName[type];
}



// 1. STRUCT type definition
typedef struct smmObject {
       char name[MAX_CHARNAME];
       smmObjType_e objType; 
       int type;
       int credit; // storing each object's course
       int energy;
       smmObjGrade_e grade;
} smmObject_t;

/* same with below code
typedef struct smmObject
{
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
} smmObject_t;
*/

const char* gradeToString(smmObjGrade_e grade) {
    switch (grade) {
        case smmObjGrade_Ap: return "A+ (4.3)";
        case smmObjGrade_A0: return "A0 (4.0)";
        case smmObjGrade_Am: return "A- (3.7)";
        case smmObjGrade_Bp: return "B+ (3.3)";
        case smmObjGrade_B0: return "B0 (3.0)";
        case smmObjGrade_Bm: return "B- (2.7)";
        case smmObjGrade_Cp: return "C+ (2.3)";
        case smmObjGrade_C0: return "C0 (2.0)";
        case smmObjGrade_Cm: return "C- (1.7)";
        default: return "Unknown";
    }
}


// 2. STRUCT ARRAY VARIABLE definition
//static smmObject_t smm_node[MAX_NODE];
//static int smmObj_noNode = 0;



//object generation 
// 3. related function CHANGE
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
{    
    smmObject_t* ptr;
    
    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    
    strcpy(ptr->name, name);
    ptr->objType = objType;
    ptr->type = type;
    ptr->credit = credit; // storing course history inform.
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ptr;
}

//3. ???? ??? ???? 
char* smmObj_getName(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->name;
}

//3. ???? ??? ???? 
int smmObj_getNodeType(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->type;
}

int smmObj_getCredit(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->credit;
}

int smmObj_getEnergy(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->energy;
}

int smmObj_getGrade(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->grade;
}
