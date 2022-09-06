#ifndef JSON_H_

#define JSON_H_

/* Includes for the header file */
#include <stdio.h>

#ifdef __cplusplus
#define LANGUAGE extern "C"
#elif defined __GNUC__
#define LANGUAGE extern
#endif

#ifdef JSON_STATIC_
#define DLLIMPORT
#elif defined BUILDING
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#define CALLING __cdecl
//#define CALLING __stdcall

/* Macros and definitions for the source file. */
#ifdef JSON_DEFINITIONS_
#define elif else if
#define null NULL
#define String char*
#endif

DLLIMPORT typedef struct JsonItem JsonItem;
LANGUAGE DLLIMPORT CALLING JsonItem* newJsonItem(int type, char* name, char* value, JsonItem* ref, int brother);
LANGUAGE DLLIMPORT CALLING void addChild(JsonItem* father, JsonItem* newChild);
LANGUAGE DLLIMPORT CALLING void deleteJsonItem(JsonItem** toErase);
LANGUAGE DLLIMPORT CALLING void setJsonItemName(JsonItem* aux, char* name);
LANGUAGE DLLIMPORT CALLING void setJsonItemValue(JsonItem* aux, char* value);
LANGUAGE DLLIMPORT CALLING JsonItem* findJsonItem(JsonItem* father, int nargs, ...);
LANGUAGE DLLIMPORT CALLING void printJsonItem(char* fileSource, char* openType, JsonItem* root, int childs);

LANGUAGE DLLIMPORT CALLING void nextWord(FILE* file, char* word, int arrayType);
LANGUAGE DLLIMPORT CALLING JsonItem* getJsonItemsV2(char* fileSource);

LANGUAGE DLLIMPORT CALLING JsonItem* getJsonItems(char* fileSource);
LANGUAGE DLLIMPORT CALLING void swapJsonItems(JsonItem** first, JsonItem** second);

struct JsonItem {
	/* Es el nivel del JsonItem. */
    int level;
	/* Es el tipo de JsonItem (1 = Boolean, 2 = Number, 3 = String, 4 = Object, 5 = Array). */
    int type;
	/* Es donde inicia el nombre del JsonItem. */
    char* name;
	/* Es donde inicia el valor del JsonItem. */
    char* value;
   	/* Es la cantidad de hijos del JsonItem. */
    int numberChilds;
	/* Es el padre del JsonItem. */
    JsonItem* father;
	/* Es el anterior hermano de JsonItem. */
    JsonItem* prevBrother;
	/* Es el próximo hermano del JsonItem. */
    JsonItem* nextBrother;
	/* Es el primer hijo del JsonItem. */
    JsonItem* firstChild;
};

#endif

#if !defined BUILDING
#undef LANGUAGE
#undef DLLIMPORT
#undef CALLING
#endif
