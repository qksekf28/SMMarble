//
//  smm_object.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//
//----------------------------------------------
// LAST ONE BEFORE RESET
//----------------------------------------------

#include "smm_object.h"
#include <string.h>

#define MAX_NODETYPE    7
#define MAX_GRADE       9
#define MAX_NODE        100 //  �߰��� 
#define MAX_CHARNAME        100

static char smmNodeName[SMMNODE_TYPE_MAX][MAX_CHARNAME] = {
	"**LECTURE**",
	"**RESTAURANT**",
    "**LABORATORY**",
    "**HOME**",
    "**MOVE to LAB**",
    "**FOOD CHANCE**",
    "**FESTIVAL**"
};

char* smmObj_getTypeName(int type)
{
    return (char*)smmNodeName[type];
}

// 1. STRUCT type definition
typedef struct smmObject
{
    char name[MAX_CHARNAME];
    smmObjType_e objType;
    int type;
    int credit;
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

// 2. STRUCT ARRAY VARIABLE definition
//---struct smmObject smm_node[MAX_NODE];	

// encapsulation�� ���� ���� static���� ����
#if 0
static char smmObj_name[MAX_NODE][MAX_CHARNAME];
static int smmObj_type[MAX_NODE];
static int smmObj_credit[MAX_NODE];
static int smmObj_energy[MAX_NODE];
#endif

//---static int smmObj_noNode=0; 

//object generation 
// 3. related function CHANGE
/*
void smmObj_genNode(char* name, int type, int credit, int energy) // ��ȣ �� �߰� 
{
	#if 0
	strcpy(smmObj_name[smmObj_noNode], name); // ���ڿ� �����Ϸ��� ���� '=' ���� �ȵ�.  ���ڿ��� �迭�̶� �ϳ��� �����ؾ� ��. 
    smmObj_type[smmObj_noNode] = type; // data ������ �Է°� ������ �ϳ��� �־��ذ� �� 
    smmObj_credit[smmObj_noNode] = credit;
    smmObj_energy[smmObj_noNode] = energy;
    #endif
    strcpy(smm_node[smmObj_noNode].name, name);
    
    smmObj_noNode++;    
}

char* smmObj_getNode---------------

int smmObj_getNodeType(int node_nr)
{
	return smmObj_type[node_nr];]
}

//member retrieving


//element to string
char* smmObj_getNodeName(smmNode_e type)
{
    return smmNodeName[type];
}

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}

*/



void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade)
{    
    smmObject_t* ptr;
    
    ptr = (smmObject_t*)malloc(sizeof(smmObject_t));
    
    strcpy(ptr->name, name);
    ptr->objType = objType;
    ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    
    return ptr;
}

//3. ???? ??? ???? 
char* smmObj_getNodeName(void* obj)
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

int smmObj_getNodeCredit(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->credit;
}

int smmObj_getNodeEnergy(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->energy;
}

int smmObj_getGrade(void* obj)
{
    smmObject_t* ptr = (smmObject_t*)obj;
    
    return ptr->grade;
}

