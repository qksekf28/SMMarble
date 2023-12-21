//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

#define SMMNODE_TYPE_LECTURE            0
#define SMMNODE_TYPE_RESTAURANT         1
#define SMMNODE_TYPE_LABORATORY         2
#define SMMNODE_TYPE_HOME               3
#define SMMNODE_TYPE_GOTOLAB            4
#define SMMNODE_TYPE_FOODCHANCE         5
#define SMMNODE_TYPE_FESTIVAL           6

#define SMMNODE_TYPE_MAX                7

typedef enum smmObjType {
    smmObjType_board = 0,
    smmObjType_card,
    smmObjType_grade
} smmObjType_e;

typedef enum smmObjGrade {
    smmObjGrade_Ap = 0,
    smmObjGrade_A0,
    smmObjGrade_Am,
    smmObjGrade_Bp,
    smmObjGrade_B0,
    smmObjGrade_Bm,
    smmObjGrade_Cp,
    smmObjGrade_C0,
    smmObjGrade_Cm
} smmObjGrade_e;


// gradeToString definition
const char* gradeToString(smmObjGrade_e grade);
/*
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
*/


/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/


/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/


//object generation
void* smmObj_genObject(char* name, smmObjType_e objType, int type, int credit, int energy, smmObjGrade_e grade);

//member retrieving
char* smmObj_getName(void* obj);
int smmObj_getNodeType(void* obj);
int smmObj_getCredit(void* obj);
int smmObj_getEnergy(void* obj);

//element to string
char* smmObj_getNodeTypeName(int type);


#endif /* smm_object_h */

