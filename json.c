/*
 * @fileoverview    {FileName}
 *
 * @version         2.0
 *
 * @author          Dyson Arley Parra Tilano <dysontilano@gmail.com>
 *
 * @copyright       Dyson Parra
 * @see             github.com/DysonParra
 *
 * History
 * @version 1.0     Implementation done.
 * @version 2.0     Documentation added.
 */
#include "json.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

CALLING void updateJsonItemLevels(JsonItem* root, int level);
CALLING void deleteJsonItemChilds(JsonItem** toErase);
CALLING JsonItem* findRoot(JsonItem* firstJsonItem);
CALLING int countJsonItems(JsonItem* root);
CALLING int findMaxLevel(JsonItem* firstJsonItem, int level);
CALLING void printArray(char* array);
CALLING void sprintArray(char* output, char* input);
CALLING void printAllJsonItems(FILE* file, JsonItem* firstJsonItem, int childs, int brothers, int tab1, int tab2);
CALLING char isLetter(char c);
CALLING char isNumber(char c);
CALLING int validateBoolean(char* array);
CALLING int validateNumber(char* array);
CALLING int getJsonType(char* fileSource);
CALLING JsonItem* obtainJsonItems(FILE* file, int jsonType, JsonItem* father, int status, char* word);

#if (!defined STATIC_LIB_) && (!defined NO_DLL_MAIN_)
/**
 * Entrada principal de la dll (creado si no se indica que es biblioteca estática)
 *
 * @param instance identificador del módulo.
 * @param reason   razón de la llamada.
 * @param reserved argumento reservado.
 * @return {@code TRUE} si se completó exitosamente la llamada, caso contrario {@code FALSE}.
 */
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    /*
     * Evalúa la razón de la llamada a la dll.
     */
    switch (reason) {
        /*
         * Se cargó la dll en el proceso actual.
         */
        case DLL_PROCESS_ATTACH:
            // printf("%s\n", "DLL_PROCESS_ATTACH");
            break;

        /*
         * Se liberó la dll en el proceso actual.
         */
        case DLL_PROCESS_DETACH:
            // printf("%s\n", "DLL_PROCESS_DETACH");
            break;

        /*
         * Se cargó la dll en un nuevo hilo.
         */
        case DLL_THREAD_ATTACH:
            // printf("%s\n", "DLL_THREAD_ATTACH");
            break;

        /*
         * Se liberó la dll en el nuevo hilo.
         */
        case DLL_THREAD_DETACH:
            // printf("%s\n", "DLL_THREAD_DETACH");
            break;
    }

    return TRUE;
}
#endif

/**
 * FIXME: Description of {@code newJsonItem}.
 * Crea un nuevo JsonItem y asociarlo a un JsonItem parámetro brother que dirá si es un hermano (diferente a 0), o un hijo (0).
 */
LANGUAGE DLLIMPORT CALLING JsonItem* newJsonItem(int type, char* name, char* value, JsonItem* ref, int brother) {
    JsonItem* nuevo;                                            // Declara un nuevo JsonItem.
    nuevo = (JsonItem*)malloc(sizeof(JsonItem));                // Inicializa el JsonItem en el heap.

    nuevo->level = 1;                                           // Inicializa el campo level en 1 por defecto.
    nuevo->type = type;                                         // A nuevo le lleva una copia de type en su campo type.
    nuevo->numberChilds = 0;                                    // Inicializa el campo numberChilds de nuevo en 0.

    nuevo->father = NULL;                                       // A nuevo le pone su padre en NULL.
    nuevo->prevBrother = NULL;                                  // A nuevo le pone su anterior hermano en NULL.
    nuevo->nextBrother = NULL;                                  // A nuevo le pone su siguiente hermano en NULL.
    nuevo->firstChild = NULL;                                   // A nuevo le pone su primer hijo en NULL.

    int nameSize = 0;                                           // Crea variable que dirá la cantidad de caracteres del nombre del XmlItem a crear.
    int valueSize = 0;                                          // Crea variable que dirá la cantidad de caracteres del valor del XmlItem a crear.

    if (ref == NULL && (name == NULL || name[0] == 0))          // Si no se indicó referencia ni nombre (Raíz).
        name = (char*)"root";                                   // Dice que el nombre será root.

    if (name != NULL) {                                         // Si se indicó nombre.
        nameSize = strlen(name) + 1;                            // A nameSize le lleva la cantidad de letras de name.
        nuevo->name = (char*)malloc(nameSize * sizeof(char));   // Reserva espacio en el heap para el nombre.
        memcpy(nuevo->name, name, nameSize);                    // Asigna nombre al XmlItem actual.
    } else
        nuevo->name = (char*)calloc(1, sizeof(char));           // Pone como nombre el caracter nulo.

    if (value != NULL) {                                        // Si se indicó valor.
        valueSize = strlen(value) + 1;                          // A valueSize le lleva la cantidad de letras de value.
        nuevo->value = (char*)malloc(valueSize * sizeof(char)); // Reserva espacio en el heap para el valor.
        memcpy(nuevo->value, value, valueSize);                 // Asigna valor al XmlItem actual.
    } else
        nuevo->value = (char*)calloc(1, sizeof(char));          // Pone como valor el caracter nulo.

    if (ref != NULL && brother) {                               // Si se indicó que se creará un hermano del JsonItem de referencia.
        nuevo->level = ref->level;                              // A nuevo le pone el mismo nivel que su hermano.
        JsonItem* last = ref;                                   // Crea JsonItem apuntando a ref.

        while (last->nextBrother != NULL)                       // Mientras no encuentre al último hemano de ref.
            last = last->nextBrother;                           // A last le lleva su siguiente hermano.

        nuevo->prevBrother = last;                              // A nuevo le pone su anterior hermano como last.
        last->nextBrother = nuevo;                              // A last le pone su siguiente hermano como nuevo.

        if (ref->father != NULL) {                              // Si ref tiene padre.
            nuevo->father = ref->father;                        // A nuevo le pone el mismo padre que tiene ref.
            nuevo->father->numberChilds++;                      // Aumenta la cantidad de hijos del padre de nuevo.
        }
    }
    elif (ref != NULL && !brother) {                            // Si se indicó que se creará un hijo del JsonItem de referencia.
        nuevo->level = ref->level + 1;                          // A nuevo le pone un nivel más su padre.
        nuevo->father = ref;                                    // Pone ref como el padre de nuevo.
        nuevo->father->numberChilds++;                          // Aumenta la cantidad de hijos del padre de nuevo.

        if (ref->firstChild == NULL) {                          // Si ref no tiene hijos.
            ref->firstChild = nuevo;                            // Pone que nuevo es el primer hijo de ref.
        } else {                                                // Si ref ya tenía hijos.
            JsonItem* child = ref->firstChild;                  // Crea JsonItem apuntando al primer hijo de ref.
            while (child->nextBrother != NULL)                  // Mientras no encuentre al último hermano de child (último hijo de ref).
                child = child->nextBrother;                     // A child le lleva su siguiente hermano.

            nuevo->prevBrother = child;                         // Pone que el anterior hermano de nuevo es child.
            child->nextBrother = nuevo;                         // Pone que el siguiente hermano de child es nuevo.
        }
    }

    return nuevo;                                               // Devuelve el JsonItem creado.
}

/**
 * FIXME: Description of {@code addChild}.
 * Asigna un nuevo hijo a un JsonItem.
 */
LANGUAGE DLLIMPORT CALLING void addChild(JsonItem* father, JsonItem* newChild) {
    if (father != NULL && newChild != NULL) {               // Si ninguno de los JsonItem es NULL.
        newChild->father = father;                          // Pone father como el padre de newChild.
        newChild->father->numberChilds++;                   // Aumenta la cantidad de hijos de father.

        if (father->firstChild == NULL)                     // Si father no tiene hijos.
            father->firstChild = newChild;                  // Pone que newChild es el primer hijo de father.
        else {                                              // Si father ya tenía hijos.
            JsonItem* child = father->firstChild;           // Crea JsonItem apuntando al primer hijo de father.
            while (child->nextBrother != NULL)              // Mientras no encuentre al último hermano de child (último hijo de father).
                child = child->nextBrother;                 // A child le lleva su siguiente hermano.

            newChild->prevBrother = child;                  // Pone que el anterior hermano de newChild es child.
            child->nextBrother = newChild;                  // Pone que el siguiente hermano de child es newChild.
        }

        updateJsonItemLevels(newChild, father->level + 1);  // Actualiza los niveles de newChild y sus descendientes.
    }
}

/**
 * FIXME: Description of {@code updateJsonItemLevels}.
 * Actualiza el nivel de un json item y sus descendientes.
 */
CALLING void updateJsonItemLevels(JsonItem* root, int level) {
    JsonItem* aux;                                                  // Crea apuntador a un JsonItem para recorrer los hermanos del JsonItem.
    aux = root;                                                     // A aux le lleva una copia del JsonItem parámetro.

    if (aux != NULL)                                                // Si el JsonItem parámetro no está vacío.
        while (aux != NULL) {                                       // Recorre los hermanos de aux.
            aux->level = level;

            if (aux->numberChilds)                                  // Si aux tiene hijos.
                updateJsonItemLevels(aux->firstChild, level + 1);   // Cuenta cada uno de los hijos de aux.

            aux = aux->nextBrother;                                 // Pasa al siguiente hermano de aux.
        }
}

/**
 * FIXME: Description of {@code deleteJsonItemChilds}.
 * Borra y libera la memoria de un JsonItem y todos sus descendientes.
 */
CALLING void deleteJsonItemChilds(JsonItem** toErase) {
    JsonItem* root = NULL;                                      // Crea un nuevo JsonItem.
    if (toErase != NULL)                                        // Si toErase no es NULL.
        root = *toErase;                                        // A root le lleva lo apuntado por el apuntador a un apuntador a JsonItem toErase.

    if (root != NULL) {                                         // Si root es un JsonItem válido.
        while (root->numberChilds)                              // Mientras root tenga hijos.
            deleteJsonItemChilds(&(root->firstChild));          // ejecuta el borrado de forma recursiva.

        if (root->prevBrother != NULL)                          // Si root no es el primer hermano.
            root->prevBrother->nextBrother = root->nextBrother; // Liga el anterior hermano de root con el siguiente hermano de root.

        if (root->nextBrother != NULL)                          // Si root no es el último hermano.
            root->nextBrother->prevBrother = root->prevBrother; // Liga el siguiente hermano de root con el anterior hermano de root.

        if (root->father != NULL) {                             // Si root tiene padre (root no es la raíz).
            root->father->numberChilds--;                       // Disminuye la cantidad de hijos del padre de root.

            if (root->father->firstChild == root)               // Si root es el primer hijo.
                root->father->firstChild = root->nextBrother;   // Dice que ahora el primer hijo es el siguiente hermano de root.
        }

        root->level = 0;                                        // Pone el nivel de root en cero para indicar que ya no es un JsonItem válido.
        root->type = 0;                                         // Pone el tipo de root en cero para indicar que ya no es un JsonItem válido.
        root->father = NULL;                                    // Pone el padre de root en NULL.
        root->prevBrother = NULL;                               // Pone el antrior hermano de root en NULL.
        root->nextBrother = NULL;                               // Pone el siguiente hermano de root en NULL.
        free(root->name);                                       // Borra la memoria ocupada por el nombre de root.
        free(root->value);                                      // Borra la memoria ocupada por el valor de root.
        free(root);                                             // Libera la memoria ocupada por root.
    }
}

/**
 * FIXME: Description of {@code deleteJsonItem}.
 * Borra y libera la memoria de un JsonItem y todos sus descendientes, y deja el json item apuntando a NULL.
 */
LANGUAGE DLLIMPORT CALLING void deleteJsonItem(JsonItem** toErase) {
    deleteJsonItemChilds(toErase);          // Borra los hijos de toErase.
    *toErase = NULL;                        // Indica que toErase ahora apunta a NULL.
}

/**
 * FIXME: Description of {@code setJsonItemName}.
 * Cambia el campo name de un JsonItem por el de un array especificado.
 */
LANGUAGE DLLIMPORT CALLING void setJsonItemName(JsonItem* aux, char* name) {
    int nameSize = (int)(strlen(name) + 1);                             // Crea entero que tendrá la longitud del array name.

    if (nameSize > (int)(strlen(aux->name) + 1))                        // Si la longitud del nuevo name es mayor que el anterior.
        aux->name = (char*)realloc(aux->name, nameSize * sizeof(char)); // Redimensiona el espacio en el heap del nombre de aux.

    memcpy(aux->name, name, nameSize);                                  // Copia el valor de name en el campo name de aux.
}

/**
 * FIXME: Description of {@code setJsonItemValue}.
 * Cambia el campo value de un JsonItem por el de un array especificado.
 */
LANGUAGE DLLIMPORT CALLING void setJsonItemValue(JsonItem* aux, char* value) {
    int valueSize = (int)(strlen(value) + 1);                               // Crea entero que tendrá la longitud del array value.
    if (valueSize > (int)(strlen(aux->value) + 1))                          // Si la longitud del nuevo value es mayor que el anterior.
        aux->value = (char*)realloc(aux->value, valueSize * sizeof(char));  // Redimensiona el espacio en el heap del valor de aux.

    memcpy(aux->value, value, valueSize);                                   // Copia el valor de value en el campo value de aux.
}

/**
 * FIXME: Description of {@code findJsonItem}.
 * Encuentra un JsonItem en un JsonItem father.
 */
LANGUAGE DLLIMPORT CALLING JsonItem* findJsonItem(JsonItem* father, int nargs, ...) {
    va_list ap;                                         // Crea una lista de parámetros.
    va_start(ap, nargs);                                // Inicializa la lista de parámetros.
    char* params[nargs];                                // Crea array para almacenar los parámetros.

    for (int i = 0; i < nargs; i++)                     // Recorre la lista de parámetros.
        params[i] = va_arg(ap, char*);                  // Almacena el parámetro en params.

    va_end(ap);                                         // Finaliza la lista de parámetros.

    // for (int i =0; i< nargs; i++)                    // Recorre el array con los parámetros.
    //   printf("Arg %d = \"%s\"\n", i, params[i]);     // Muestra el parámetro actual.

    JsonItem* aux = father->firstChild;                 // Crea JsonItem para recorrer los hijos del JsonItem parámetro.

    int findJsonItem = 1;                               // Crea variable que indicará si se encontró el JsonItem con el nombre especificado.

    for (int i = 0; i < nargs; i++) {                   // Recorre los parámetros.
        while (aux != NULL) {                           // Recorre aux para compararlo con el parámetro actual.
            if (strcmp(aux->name, params[i]))           // Si el nombre del JsonItem actual no es igual al del parámetro actual.
                findJsonItem = 0;                       // Pone findJsonItem en 0 indicando que no encontró el JsonItem.

            if (findJsonItem) {                         // Si el JsonItem fue encontrado.
                if (i != nargs - 1)                     // Si este no era el último parámetro.
                    aux = aux->firstChild;              // A aux le lleva su primer hijo.
                break;                                  // Sale del while.
            } else                                      // Si no encontró el JsonItem.
                findJsonItem = 1;                       // Reinicia findJsonItem para que buesque en el próximo hermano.

            aux = aux->nextBrother;                     // A aux le lleva su siguiente hermano.
        }

        if (aux == NULL)                                // Si recorrió todos los hermanos de aux y no encontró el JsonItem con nombre igual al parámetro.
            break;                                      // Sale del for.
    }

    return aux;                                         // Devuelve la posición de memoria donde encontró el JsonItem solicitado.
}

/**
 * FIXME: Description of {@code findRoot}.
 * Obtiene la raíz de un JsonItem.
 */
CALLING JsonItem* findRoot(JsonItem* firstJsonItem) {
    JsonItem* aux;                  // Crea apuntador a un JsonItem para recorrer los hermanos del JsonItem.
    aux = firstJsonItem;            // A aux le lleva una copia del JsonItem parámetro.

    if (aux != NULL)                // Si el JsonItem parámetro no está vacío.
        while (aux->father != NULL) // Recorre los padres de aux.
            aux = aux->father;      // Pasa al padre de aux.

    return aux;                     // Devuelve la raíz de firstJsonItem.
}

/**
 * FIXME: Description of {@code countJsonItems}.
 * Obtiene la cantidad de descendientes de un JsonItem indicado.
 */
CALLING int countJsonItems(JsonItem* root) {
    int quantity = 0;                                           // Variable que dirá la cantidad de JsonItems encontrados.
    JsonItem* aux;                                              // Crea apuntador a un JsonItem para recorrer los hermanos del JsonItem.
    aux = root;                                                 // A aux le lleva una copia del JsonItem parámetro.

    if (aux != NULL)                                            // Si el JsonItem parámetro no está vacío.
        while (aux != NULL) {                                   // Recorre los hermanos de aux.
            quantity++;                                         // Aumenta la cantidad de JsonItems.

            if (aux->numberChilds)                              // Si aux tiene hijos.
                quantity += countJsonItems(aux->firstChild);    // Cuenta cada uno de los hijos de aux.

            aux = aux->nextBrother;                             // Pasa al siguiente hermano de aux.
        }

    return quantity;                                            // Devuelve la cantidad de hijos del padre de aux.
}

/**
 * FIXME: Description of {@code findMaxLevel}.
 * Obtiene la mayor profundidad de un JsonItem.
 */
CALLING int findMaxLevel(JsonItem* firstJsonItem, int level) {
    JsonItem* aux;                                              // Crea apuntador a un JsonItem para recorrer los hermanos del JsonItem.
    aux = firstJsonItem;                                        // A aux le lleva una copia del JsonItem parámetro.

    if (aux != NULL)                                            // Si el JsonItem parámetro no está vacío.
        while (aux != NULL) {                                   // Recorre los hermanos de aux.
            if (aux->level > level)                             // Si el nivel de aux es mayor que level.
                level = aux->level;                             // A level le lleva el nivel de aux.

            if (aux->numberChilds)                              // Si aux tiene hijos.
                level = findMaxLevel(aux->firstChild, level);   // Busca el nivel de cada uno de los hijos de aux.

            aux = aux->nextBrother;                             // Pasa al siguiente hermano de aux.
        }

    return level;                                               // Devuelve el mayor nivel encontrado.
}

/**
 * FIXME: Description of {@code printArray}.
 * Imprime un array sin los caracteres de escape.
 */
CALLING void printArray(char* array) {
// Crea macro para imprimir caracteres de escape.
#define printChar(character)      \
    case character:               \
        printf("%s", #character); \
        break

    for (int i = 0; i < strlen(array); i++) {   // Recorre el array de entrada.
        switch (array[i]) {                     // Evalua el caracter actual.
            printChar('\n');                    // Imprime el caracter de escape.
            printChar('\r');                    // Imprime el caracter de escape.
            printChar('\t');                    // Imprime el caracter de escape.
            printChar('\a');                    // Imprime el caracter de escape.
            printChar('\b');                    // Imprime el caracter de escape.
            printChar('\v');                    // Imprime el caracter de escape.
                                                // printChar('\?');                                                          // Imprime el caracter de escape.
                                                // printChar('\"');                                                          // Imprime el caracter de escape.

            default:                            // Si no es un caracter de escape.
                printf("%c", array[i]);         // Imprime el caracter actual.
                break;                          // Termina de evaluar el caracter actual.
        }
    }

#undef printChar                                // Borra macro para imprimir caracteres de escape.
}

/**
 * FIXME: Description of {@code sprintArray}.
 * Copia un array de entrada en otro sin los caracteres de escape.
 */
CALLING void sprintArray(char* output, char* input) {
// Crea macro para imprimir caracteres de escape.
#define printChar(character)                                \
    case character:                                         \
        sprintf(&output[strlen(output)], "%s", #character); \
        break

    for (int i = 0; i < strlen(input); i++) {                       // Recorre el array de entrada.
        switch (input[i]) {                                         // Evalua el caracter actual.
            printChar('\n');                                        // Imprime el caracter de escape.
            printChar('\r');                                        // Imprime el caracter de escape.
            printChar('\t');                                        // Imprime el caracter de escape.
            printChar('\a');                                        // Imprime el caracter de escape.
            printChar('\b');                                        // Imprime el caracter de escape.
            printChar('\v');                                        // Imprime el caracter de escape.
                                                                    // printChar('\?');                                                          // Imprime el caracter de escape.
                                                                    // printChar('\"');                                                          // Imprime el caracter de escape.

            default:                                                // Si no es un caracter de escape.
                sprintf(&output[strlen(output)], "%c", input[i]);   // Imprime el caracter actual.
                break;                                              // Termina de evaluar el caracter actual.
        }
    }

#undef printChar                                                    // Borra macro para imprimir caracteres de escape.
}

/**
 * FIXME: Description of {@code printAllJsonItems}.
 * Muestra un JsonItem, childs dice si imprime todos sus hijos y brother dice si imprime todos sus hermanos, tab1 indica la cantidad de tabulaciones antes de imprimir cada JsonItem.
 */
CALLING void printAllJsonItems(FILE* file, JsonItem* firstJsonItem, int childs, int brothers, int tab1, int tab2) {
    char printer[10000] = "";                                                       // Crea variable para almacenar lo que se va a imprimir ya sea en un archivo o en pantalla.

#define print(...) sprintf(&printer[strlen(printer)], __VA_ARGS__)                  // crea macro almacenar la salida de sprintf en printer.
//#define prtArray(input) sprintf(&printer[strlen(printer)], "%s", input)           // crea macro almacenar la salida de sprintArray en printer.
#define prtArray(input) sprintArray(&printer[strlen(printer)], input)               // crea macro almacenar la salida de sprintArray en printer.

    JsonItem* aux;                                                                  // Crea apuntador a un JsonItem para recorrer los hermanos del JsonItem.
    aux = firstJsonItem;                                                            // A aux le lleva una copia del JsonItem parámetro.

    if (aux == NULL || (aux->father == NULL && aux->level != 1)) {                  // Si el JsonItem parámetro está vacío o es un JsonItem inválido (borrado).
        if (file == NULL)                                                           // Si el apuntador al archivo no es válido.
            printf("No JsonItems\n");                                               // Imprime que no hay JsonItems.
        else                                                                        // Si el apuntador a archivo indicado es válido.
            fprintf(file, "No JsonItems\n");                                        // Imprimer en el archivo que no hay JsonItems.
    } else {                                                                        // Si el JsonItem parámetro no está vacío.
        while (aux != NULL) {                                                       // Recorre los hermanos de aux.
            printer[0] = 0;                                                         // Pone vacío el array con los valores a imprimir.
            print("%2d  ", aux->level);                                             // Imprime el nivel de aux.

            for (int i = 0; i < (tab1)*9; i++)                                      // Recorre 9 veces parámetro tab1.
                print(" ");                                                         // Imprime un espacio.

            print("%p %p ", aux->father, aux);                                      // Imprime direcciones de memoria del JsonItem anterior y del actual.
            print("%p %p  ", aux->prevBrother, aux->nextBrother);                   // Imprime direcciones de memoria del JsonItem siguiente y el padre.

            for (int i = 0; i < (tab2)*9; i++)                                      // Recorre 9 veces parámetro tab2.
                print(" ");                                                         // Imprime un espacio.

            print("%2d  ", aux->level);                                             // Imprime el nivel de aux.

#define printType(nro, text) \
    case nro:                \
        print("%s", text);   \
        break                                                                       // Crea macro para imprimir el tipo de JsonItem.

            switch (aux->type) {                                                    // Evalúa el tipo de objeto.
                printType(1, "'Boolean'  ");                                        // Si el JsonItem es un booleano.
                printType(2, "' Number'  ");                                        // Si el JsonItem es un número.
                printType(3, "' String'  ");                                        // Si el JsonItem es un String.
                printType(4, "' Object'  ");                                        // Si el JsonItem es un objeto.
                printType(5, "'  Array'  ");                                        // Si el JsonItem es un array.

                default:                                                            // Si el JsonItem tiene un tipo desconocido.
                    print("'  Other'  ");                                           // Imprime el tipo de JsonItem actual.
                    break;                                                          // Sale del switch.
            }
#undef printType                                                                    // Borra la macro para imprimir el tipo de JsonItem.

            print("\"%2d\"  ", aux->numberChilds);                                  // Imprime cantidad de hijos del JsonItem actual.

            if (aux->name != NULL)                                                  // Si el JsonItem tiene nombre asignado.
                print("'%s'  ", aux->name);                                         // Imprime nombre del JsonItem actual.
            else                                                                    // Si el JsonItem no tiene un nombre asignado.
                print("''  ");                                                      // Imprime dos comillas vacías indicando que el XmlItem no tiene nombre.

            if (aux->value != NULL) {                                               // Si el JsonItem tiene valor asignado.
                print("'");                                                         // Imprime comillas.
                prtArray(aux->value);                                               // Imprime valor del JsonItem actual.
                // print("%s", aux->value);                                         // Imprime valor del JsonItem actual.
                print("'");                                                         // Imprime comillas.
            } else                                                                  // Si el JsonItem no tiene un valor asignado.
                print("''");                                                        // Imprime dos comillas vacías indicando que el XmlItem no tiene valor.
            print("  ");                                                            // Imprime espacios.

            print("\n");                                                            // Imprime salto de línea.

            if (file == NULL)                                                       // Si no se indicó un archivo de salida.
                printf("%s", printer);                                              // Muestra el contenido del array con la información del XmlItem actual en pantalla.
            else                                                                    // Si se indicó un archivo de salida.
                fprintf(file, "%s", printer);                                       // Escribe el contenido del array con la información del XmlItem actual en el archivo.

            if (aux->numberChilds && childs)                                        // Si aux tiene hijos y se indicó imprimir hijos.
                printAllJsonItems(file, aux->firstChild, 1, 1, tab1 + 1, tab2 - 1); // Imprime cada uno de los hijos de aux.

            if (brothers)                                                           // Si se indicó imprimir hermanos.
                aux = aux->nextBrother;                                             // Pasa al siguiente hermano de aux.
            else                                                                    // Si se indicó no imprimir hermanos.
                break;                                                              // Sale del ciclo.
        }
    }

#undef print                                                                        // Borra la macro para almacenar la salida de sprintf en printer.
#undef prtArray                                                                     // Borra la macro para almacenar la salida de sprintArray en printer.
}

/**
 * FIXME: Description of {@code printJsonItem}.
 * Invoca al método que imprime los JsonItems.
 */
LANGUAGE DLLIMPORT CALLING void printJsonItem(char* fileSource, char* openType, JsonItem* root, int childs) {
    FILE* file = fopen(fileSource, openType);                                                           // Crea un archivo en la ruta indicada por fileSource.

    if (!childs || root == NULL)                                                                        // Si indicó no imprimir los hijos del JsonItem o el JsonItem está vacío.
        printAllJsonItems(file, root, childs, 0, 0, 0);                                                 // Imprime los JsonItems.
    else                                                                                                // Si indicó imprimir los hijos del JsonItem.
        printAllJsonItems(file, root, childs, 0, 0, findMaxLevel(root->firstChild, 0) - root->level);   // Imprime los JsonItems.

    if (file == NULL)                                                                                   // Si el apuntador al archivo no es válido.
        printf("\n");                                                                                   // Imprime salto de línea en pantalla.
    else                                                                                                // Si el apuntador a archivo indicado es válido.
        fprintf(file, "\n");                                                                            // Imprime salto de línea en el archivo.

    fclose(file);                                                                                       // Cierra el archivo indicado por fileSource.
}

/**
 * FIXME: Description of {@code isLetter}.
 * Evalúa si un caracter es una letra. Devuelve 1 caso afirmativo o caso contrario 0.
 */
CALLING char isLetter(char c) {
    if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122)) // Si el caracter es una letra.
        return 1;                                      // Devuelve 1 indicando que es una letra.
    else                                               // Si el caracter no es una letra.
        return 0;                                      // Devuelve 0 indicando que no es una letra.
}

/**
 * FIXME: Description of {@code isNumber}.
 * Evalúa si un caracter es un número o un signo. Devuelve 1 caso afirmativo o caso contrario 0.
 */
CALLING char isNumber(char c) {
    if ((c >= 48 && c <= 57) || c == '+' || c == '-' || c == '.' || c == 'e' || c == 'E') // Si el caracter es un número.
        return 1;                                                                         // Devuelve 1 indicando que es un número.
    else                                                                                  // Si el caracter no es un número.
        return 0;                                                                         // Devuelve 0 indicando que no es un número.
}

/**
 * FIXME: Description of {@code validateBoolean}.
 * Valida si un array tiene un boolean válido.
 */
CALLING int validateBoolean(char* array) {
    int result = 1;                         // Crea variable para almacenar el resultado de validar el booleano.

    if (strcmp((char*)"true", array)        // Si array no tiene la palabbra "true".
        && strcmp((char*)"false", array)    // Si array no tiene la palabra "false".
        && strcmp((char*)"null", array))    // Si array no tiene la palabra "null".
        result = 0;                         // Pone result en cero indicando que noes un booleano válido.

    return result;                          // Devuelve el resultado de evaluar el array.
}

/**
 * FIXME: Description of {@code validateNumber}.
 * Valida si un array tiene un número válido.
 */
CALLING int validateNumber(char* array) {
    int result = 0;                                 // Crea variable que dirá si el array tiene un número válido (por defecto no).
    int status = 1;                                 // crea variable que dirá el stado actual del array.

// Crea macro para evalúar el estado actual del array.
#define statusCase(stat, posit, negat, cero, number, point, exponent) \
    case stat:                                                        \
        switch (array[i]) {                                           \
            case '+':                                                 \
                status = posit;                                       \
                break;                                                \
                                                                      \
            case '-':                                                 \
                status = negat;                                       \
                break;                                                \
                                                                      \
            case '0':                                                 \
                status = cero;                                        \
                break;                                                \
                                                                      \
            case '.':                                                 \
                status = point;                                       \
                break;                                                \
                                                                      \
            case 'e':                                                 \
            case 'E':                                                 \
                status = exponent;                                    \
                break;                                                \
                                                                      \
            default:                                                  \
                if (array[i] >= '1' && array[i] <= '9')               \
                    status = number;                                  \
                else                                                  \
                    status = 0;                                       \
                break;                                                \
        }                                                             \
        break;

    for (int i = 0; i < strlen(array); i++) {       // Recorre array.
        switch (status) {                           // Evalúa el estado actual del array.
            statusCase(1, 0, 2, 3, 4, 0, 0);        // Si el estado actual es "No se han leído números".
            statusCase(2, 0, 0, 3, 4, 0, 0);        // Si el estado actual es "Signo negativo número natural".
            statusCase(3, 0, 0, 0, 0, 5, 7);        // Si el estado actual es "Número natural es cero".
            statusCase(4, 0, 0, 4, 4, 5, 7);        // Si el estado actual es "Digito de número natural".
            statusCase(5, 0, 0, 6, 6, 0, 0);        // Si el estado actual es "Punto de inicio decimal".
            statusCase(6, 0, 0, 6, 6, 0, 7);        // Si el estado actual es "Digito de número decimal".
            statusCase(7, 8, 8, 9, 10, 0, 0);       // Si el estado actual es "Indicador exponencial".
            statusCase(8, 0, 0, 9, 10, 0, 0);       // Si el estado actual es "Signo de número exponencial".
            statusCase(9, 0, 0, 9, 0, 0, 0);        // Si el estado actual es "Número exponencial es cero".
            statusCase(10, 0, 0, 10, 10, 0, 0);     // Si el estado actual es "Digito de número exponencial".
            statusCase(0, 0, 0, 0, 0, 0, 0);        // Si el estado actual es "Número no válido".
        }

        // printf("%c %d\n", array[i], status);
        if (status == 0)                            // Si se obtuvo estado inválido luego de evaluar el caracter actual.
            break;                                  // Sale del for.
    }

#undef statusCase                                   // Borra la macro usada para evalúar el estado actual del array.

    switch (status) {                               // Evalúa el estado final.
        case 3:                                     // Si el estado final es "Número natural es cero".
        case 4:                                     // Si el estado final es "Digito de número natural".
        case 6:                                     // Si el estado final es "Digito de número decimal".
        case 9:                                     // Si el estado final es "Número exponencial es cero".
        case 10:                                    // Si el estado final es "Digito de número exponencial".
            result = 1;                             // Pone result en uno indicando que es un número válido.
            break;                                  // Termina de valuar el estado final.
    }

    return result;                                  // Devuelve si es o no un número válido.
}

/**
 * FIXME: Description of {@code getJsonType}.
 * Obtiene el tipo de json que hay en un archivo. tipo 1 = los caracteres especiales \" se escriben precedidos de slash (\\ \") y los unicode de dos slash (\\u) (json dentro de un string). tipo 0 = los caracteres especiales \" no se escriben precedidos de slash (\ ") y los unicode de un slash (\u) (json común).
 */
CALLING int getJsonType(char* fileSource) {
    int type = -1;                          // Crea variable que tendrá el tipo de json que es.
    char array[5000] = "";                  // Crea array para almacenar cada caracter del archivo.

    FILE* file = fopen(fileSource, "r+");   // Abre el archivo indicado por fileSource.
    if (file != NULL) {                     // Si el archivo existe.
        for (int i = 0; i < 5000; i++) {    // Recorre el archivo.
            array[i] = fgetc(file);         // Obtiene el siguiente caracter del archivo.
            // printf("%c", array[i]);      // Muestra el caracter obtenido.

            if (array[i] == '\"') {         // Si el caracter actual son comillas.
                if (array[i - 1] == '\\')   // Si las comillas están dentro de un string ("..\"..").
                    type = 1;               // Indica que es array tipo 1.
                else
                    type = 0;               // Indica que es un json de tipo 0.
                break;                      // Sale del for.
            }
            elif (feof(file))               // Si terminó de recorrer el archivo sin encontrar comillas.
                break;                      // Sale del for.
        }
    }
    fclose(file);                           // Cierra el archivo indicado por fileSource.

    return type;                            // Devuelve el tipo de array.
}

/**
 * FIXME: Description of {@code nextWord}.
 * Obtiene la siguiente palabra del json.
 */
LANGUAGE DLLIMPORT CALLING void nextWord(FILE* file, char* word, int jsonType) {
    // printf("%I32d\n", ftell(file));                                                      // Muestra la posición del cursor del archivo.
    word[0] = 0;                                                                            // Pone vacía el array donde se almacenará la palabra.
    int end = -2;                                                                           // Crea variable para almacenar la posición del último caracter de la palabra.

    if (!feof(file)) {                                                                      // Si no ha terminado de recorrer el archivo.
        word[0] = fgetc(file);                                                              // Obtiene el primer caracter del archivo.
        // printf("%c", word[0]);                                                           // Muestra el primer caracter obtenido.

        switch (word[0]) {                                                                  // Evalúa el primer caracter obtenido.
            case '\t':                                                                      // Si el primer caracter es una tabulación.
            case '\n':                                                                      // Si el primer caracter es un salto de línea.
            case ' ':                                                                       // Si el primer caracter es un espacio.
                word[0] = ' ';                                                              // Pone el primer caracter como un espacio.
                break;                                                                      // Termina de evaluar el primer caracter.

            case EOF:                                                                       // Si el primer caracter es fin de archivo.
                end = -1;                                                                   // Indica que el final de la palabra es en la posición -1.
                break;                                                                      // Termina de evaluar el primer caracter.

            case '\"':                                                                      // Si el primer caracter son comillas.
                if (jsonType)                                                               // Si es un json de tipo uno.
                    end = 0;                                                                // Indica que el final de la palabra es en la posición 0.
                break;                                                                      // Termina de evaluar el primer caracter.

            case '\\':                                                                      // Si el primer caracter es un slash invertido.
                if (jsonType) {                                                             // Si es un json de tipo uno.
                    word[0] = fgetc(file);                                                  // Obtiene el siguiente caracter del archivo.
                    if (word[0] != '\"')                                                    // Si el siguiente caracter no son comillas.
                        end = 0;                                                            // Indica que el final de la palabra es en la posición 0.
                }
                break;                                                                      // Termina de evaluar el primer caracter.

            default:                                                                        // Si el primer caracter no es ninguno de los indicados.
                if (!isLetter(word[0]) && !isNumber(word[0]))                               // Si el primer caracter no es una letra ni un número.
                    end = 0;                                                                // Indica que el final de la palabra es en la posición cero.
                break;                                                                      // Termina de evaluar el primer caracter.
        }
    }

    if (end == -2) {                                                                        // Si no se indicó el fin de la palabra luego de obtener el primer caracter.
        for (int i = 1; i < 10000000; i++) {                                                // Recorre el archivo.
            word[i] = fgetc(file);                                                          // Obtiene el siguiente caracter del archivo.
            // printf("%c", word[i]);                                                       // Muestra el caracter obtenido.

            switch (word[i]) {                                                              // Evalúa el caracter obtenido.
                case '\t':                                                                  // Si el caracter actual es una tabulación.
                case '\n':                                                                  // Si el caracter actual es un salto de línea.
                case ' ':                                                                   // Si el caracter actual es un espacio.
                    // word[i] = ' ';                                                       // Cambia el caracter obtenido por un espacio.
                    switch (word[0]) {                                                      // Evalúa el primer caracter.
                        case ' ':                                                           // Si el primer caracter es un espacio.
                        case '\"':                                                          // Si el primer caracter son comillas.
                            break;                                                          // Termina de evaluar el primer caracter.

                        default:                                                            // Si el primer caracter no es ninguno de los indicados.
                            end = i - 1;                                                    // Indica que el final de la palabra es en la posición anterior.
                            fseek(file, -1, SEEK_CUR);                                      // Retrocede el cursor del archivo una posición.
                            break;                                                          // Termina de evaluar el primer caracter.
                    }
                    break;                                                                  // Termina de evaluar el caracter actual.

                case '\"':                                                                  // Si el caracter actual son comillas.
                    switch (word[0]) {                                                      // Evalúa el primer caracter.
                        case '\"':                                                          // Si el primer caracter son comillas.
                            if (!jsonType) {                                                // Si es un json de tipo cero.
                                if (!(word[i - 1] == '\\' && word[i - 2] != '\\'))          // Si no se cumple que el caracter anterior es un slash y el anterior a ese no (?\").
                                    end = i;                                                // Indica que el final de la palabra es en la posición actual.
                            } else {                                                        // Si es un json de tipo uno.
                                if (word[i - 1] == '\\') {                                  // Si el anterior caracter es un slash invertido.
                                    if (!(word[i - 2] == '\\' && word[i - 3] == '\\')) {    // Si no se cumple que los tres caracteres anteriores son slash.
                                        word[i - 1] = '\"';                                 // Pone comillas en la posición anterior de la palabra.
                                        end = i - 1;                                        // Indica que el final de la palabra es en la posición anterior.
                                    }
                                } else                                                      // Si es json tipo uno y el anterior caracter no es slash invertido.
                                    end = -1;                                               // Indica que el final de la palabra es en la posición -1.
                            }

                            break;                                                          // Termina de evaluar el primer caracter.

                        default:                                                            // Si el primer caracter no son comillas.
                            end = i - 1;                                                    // Indica que el final de la palabra es en la posición anterior.
                            fseek(file, -1, SEEK_CUR);                                      // Retrocede el cursor del archivo una posición.
                            break;                                                          // Termina de evaluar el primer caracter.
                    }
                    break;                                                                  // Termina de evaluar el caracter actual.

                case EOF:                                                                   // Si el caracter actual es fin de archivo.
                    if (word[0] == '\"')                                                    // Si el primer caracter de la palabra son comillas.
                        end = -1;                                                           // Indica que el final de la palabra es en la posición -1.
                    else
                        end = i - 1;                                                        // Indica que el final de la palabra es en la posición anterior.
                    fseek(file, 0, SEEK_CUR);                                               // Retrocede el cursor del archivo antes de obtener el caracter de finalizacion.
                    break;                                                                  // Termina de evaluar el caracter actual.

                default:                                                                    // Si el caracter actual no es ninguno de los indicados.
                    switch (word[0]) {                                                      // Evalúa el primer caracter.
                        case '\"':                                                          // Si el primer caracter son comillas.
                            break;                                                          // Termina de evaluar el primer caracter.

                        case ' ':                                                           // Si el primer caracter es un espacio.
                            end = i - 1;                                                    // Indica que el final de la palabra es en la posición anterior.
                            fseek(file, -1, SEEK_CUR);                                      // Retrocede el cursor del archivo una posición.
                            break;                                                          // Termina de evaluar el primer caracter.

                        default:                                                            // Si el caracter actual no es ninguno de los indicados.
                            if (!isLetter(word[i]) && !isNumber(word[i])) {                 // Si el caracter actual no es una letra ni un número.
                                end = i - 1;                                                // Indica que el final de la palabra es en la posición anterior.
                                fseek(file, -1, SEEK_CUR);                                  // Retrocede el cursor del archivo una posición.
                            }
                            break;                                                          // Termina de evaluar el caracter actual.
                    }
                    break;                                                                  // Termina de evaluar el caracter actual.
            }

            if (end != -2)                                                                  // Si con el caracter actual se obtuvo el final de la palabra.
                break;                                                                      // Sale del ciclo.
        }
    }

    word[end + 1] = 0;                                                                      // Marca el fin de la palabra encontrada.
}

/**
 * FIXME: Description of {@code obtainJsonItems}.
 * Obtiene los JsonItem de un archivo, type indica el tipo de json, father indica un JsonItem de referencia para insertarle JsonItems hijo.
 */
CALLING JsonItem* obtainJsonItems(FILE* file, int jsonType, JsonItem* father, int status, char* word) {
    JsonItem* item = NULL;                                                                  // Variable usada para obtener la posición de los JsonItem que se vayan creando.
    char itemName[2000] = "";                                                               // Variable usada para almacenar el nombre de cada JsonItem a crear.
    char itemValue[200] = "";                                                               // Variable usada para almacenar el valor de cada JsonItem a crear.
    int itemNumber = 0;                                                                     // Crea variable usada para almacenar el nombre de cada JsonItem dentro de un array (entero).
    int itemType = 0;                                                                       // Crea variable que dirá el tipo de cada json item a crear.

// Crea macro para obtener el próximo estado del array.
#define updateStatus(stat, space, opObj, opArray, quotation, points, numbLetter, comma, clObjArray, other) \
    case stat:                                                                                             \
        switch (word[0]) {                                                                                 \
            case ' ':                                                                                      \
            case EOF:                                                                                      \
            case 0:                                                                                        \
                status = space;                                                                            \
                break;                                                                                     \
                                                                                                           \
            case '{':                                                                                      \
                status = opObj;                                                                            \
                break;                                                                                     \
                                                                                                           \
            case '[':                                                                                      \
                status = opArray;                                                                          \
                break;                                                                                     \
                                                                                                           \
            case '\"':                                                                                     \
                status = quotation;                                                                        \
                break;                                                                                     \
                                                                                                           \
            case ':':                                                                                      \
                status = points;                                                                           \
                break;                                                                                     \
                                                                                                           \
            case ',':                                                                                      \
                status = comma;                                                                            \
                break;                                                                                     \
                                                                                                           \
            case '}':                                                                                      \
            case ']':                                                                                      \
                status = clObjArray;                                                                       \
                break;                                                                                     \
                                                                                                           \
            default:                                                                                       \
                if (isLetter(word[0]) || isNumber(word[0]))                                                \
                    status = numbLetter;                                                                   \
                else                                                                                       \
                    status = other;                                                                        \
                break;                                                                                     \
        }                                                                                                  \
        break;

    /*-
    printf("\nNew recursive execution\n");
    printJsonItem(NULL, "a", findRoot(father), 1);                                          // Muestra todos los JsonItem creados hasta ahora.
    */

    while (word[0] != 0 && status) {                                                        // Recorre el archivo con el json mientras no encuentre el final o un estado no válido.
        nextWord(file, word, jsonType);                                                     // Obtiene la siguiente palabra.
        switch (status) {                                                                   // Evalúa el estado actual.
            updateStatus(1, 1, 2, 3, 0, 0, 0, 0, 0, 0);                                     // Si el estado actual es "start status".
            updateStatus(2, 2, 0, 0, 4, 0, 0, 0, 8, 0);                                     // Si el estado actual es "open object".
            updateStatus(3, 3, 2, 3, 6, 0, 6, 0, 8, 0);                                     // Si el estado actual es "open array".
            updateStatus(4, 4, 0, 0, 0, 5, 0, 0, 0, 0);                                     // Si el estado actual es "item Name".
            updateStatus(5, 5, 2, 3, 6, 0, 6, 0, 0, 0);                                     // Si el estado actual es "waiting item value".
            updateStatus(6, 6, 0, 0, 0, 0, 0, 7, 8, 0);                                     // Si el estado actual es "item value".
            updateStatus(7, 7, 0, 0, 4, 0, 0, 0, 0, 0);                                     // Si el estado actual es "next item".
            updateStatus(8, 8, 0, 0, 0, 0, 0, 0, 0, 0);                                     // Si el estado actual es "close object/array".
            updateStatus(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);                                     // Si el estado actual es "invalid".
        }

        // printf("%d '%s'\n", status, word);                                               // Muestra el estado actual y la palabra obtenida.

        switch (status) {                                                                   // Evalua el estado obtenido con la palabra actual.
            case 2:                                                                         // Si el estado actual es "open object".
            case 3:                                                                         // Si el estado actual es "open array".
                if (word[0] != ' ' && word[0] != 0) {                                       // Si la palabra obtenida no fue un espacio ni el final del archivo.
                    if (itemName[0] == 0 && father != NULL) {                               // Si el JsonItem a crear está dentro de un array.
                        sprintf(itemName, "%d", itemNumber);                                // Almacena en itemName el número de hijo del array actual.
                        itemNumber++;                                                       // Aumenta la cantidad de JsonItem dentro del array.
                    }

                    sprintf(itemValue, "%s %c", word, word[0] + 2);                         // Almacena en itemName el nombre del actual array u objeto.
                    item = newJsonItem(status + 2, itemName, itemValue, father, 0);         // Crea un nuevo JsonItem como hijo de father.
                    // printJsonItem(NULL, "a", item, 0);                                   // Muestra el JsonItem creado.
                    item = obtainJsonItems(file, jsonType, item, status, word);             // Inicia una ejecución recursiva.

                    itemName[0] = 0;                                                        // Reinicia itemName.

                    if (item == NULL)                                                       // Si luego de la ejecución recursiva se obtuvo error.
                        status = 0;                                                         // Pone el estado actual en cero indicando error.
                    else if (father == NULL)                                                // Si luego de la ejecución recursiva quedó en la raíz del json.
                        status = 8;                                                         // Pone el estado en "close object/array" indicando final del json.
                    else                                                                    // Si no obtuvo estado de error ni quedó en la raíz del json.
                        status = 6;                                                         // Pone el estado en "item value".

                    /*-
                    printf("\nCome Back ");
                    printf("%d '%s'\n", status, word);                                      // Muestra el estado actual y la palabra obtenida.
                    printJsonItem(NULL, "a", findRoot(item), 1);                            // Muestra todos los JsonItem creados hasta ahora.
                    */
                }
                break;                                                                      // Termina de evaluar el estado obtenido.

            case 4:                                                                         // Si el estado actual es "item Name".
                if (word[0] != ' ' && word[0] != 0) {                                       // Si la palabra obtenida no fue un espacio ni el final del archivo.
                    memcpy(itemName, &word[1], strlen(word) - 1);                           // Almacena el nombre del JsonItem a crear.
                    itemName[strlen(word) - 2] = 0;                                         // Marca el final del nombre del jsonIten a crear.
                            // printf("'%s'\n", itemName);
                }
                break;                                                                      // Termina de evaluar el estado obtenido.

            case 6:                                                                         // Si el estado actual es "item value".
                if (word[0] != ' ' && word[0] != 0) {                                       // Si la palabra obtenida no fue un espacio ni el final del archivo.
                    if (itemName[0] == 0) {                                                 // Si el JsonItem a crear está dentro de un array.
                        sprintf(itemName, "%d", itemNumber);                                // Almacena en itemName el número de hijo del array actual.
                        itemNumber++;                                                       // Aumenta la cantidad de JsonItem dentro del array.
                    }

                    if (word[0] == '\"') {                                                  // Si el valor del JsonItem actual es un string.
                        word[strlen(word) - 1] = 0;                                         // Quita las comillas al final del valor del JsonItem.
                        itemType = 3;                                                       // Asigna tipo String al JsonItem.
                    }
                    elif (validateNumber(word))                                             // Si el JsonItem actual es un número válido.
                        itemType = 2;                                                       // Asigna tipo numérico al JsonItem.
                    elif (validateBoolean(word))                                            // Si el JsonItem actual es un booleano válido.
                        itemType = 1;                                                       // Asigna tipo booleano al JsonItem.
                    else                                                                    // Si el JsonItem actual no tiene un valor válido.
                        status = 0;                                                         // Indica que el json no es válido.

                    if (status) {                                                           // Si el JsonItem actual es válido.
                        if (itemType != 3)                                                  // Si el JsonItem acrear no es un array.
                            item = newJsonItem(itemType, itemName, &word[0], father, 0);    // Crea un nuevo JsonItem como hijo de father.
                        else                                                                // Si el JsonItem a crear es un array.
                            item = newJsonItem(itemType, itemName, &word[1], father, 0);    // Crea un nuevo JsonItem como hijo de father.
                        // printJsonItem(NULL, "a", item, 0);                               // Muestra el JsonItem creado.

                        itemName[0] = 0;                                                    // Reinicia itemName.
                    }
                }
                break;                                                                      // Termina de evaluar el estado obtenido.

            case 7:                                                                         // Si el estado actual es "next item".
                if (father == NULL)                                                         // Si está en la raíz del json.
                    status = 0;                                                             // Indica que el json no es válido.
                else if (father->type == 5)                                                 // Si no está en la raíz del json y el tipo del padre es array.
                    status = 3;                                                             // Pasa al estado de "open array".
                break;                                                                      // Termina de evaluar el estado obtenido.

            case 8:                                                                         // Si el estado actual es "close object/array".
                if (word[0] != ' ' && word[0] != 0) {                                       // Si la palabra obtenida no fue un espacio.
                    if ((word[0] == '}' && father->type != 4)                               // Si encontró cierre de objeto y el padre no es un objeto.
                        || (word[0] == ']' && father->type != 5))                           // Si encontró cierre de array y el padre no es un array.
                        status = 0;                                                         // Indica que el json no es válido.
                    else                                                                    // Si el ciere del array u objeto corresponde al tipo de apertura.
                        return father;                                                      // Devuelve el item de referencia que entró como parámetro.
                }
                break;                                                                      // Termina de evaluar el estado obtenido.

            case 0:                                                                         // Si el estado actual es "invalid".
                break;                                                                      // Termina de evaluar el estado obtenido.
        }
    }

    if (status != 8 || father != NULL) {                                                    // Si el último estado obtenido no fue "close object/array" o no está en la raíz.
        /*-
        printf("\nError in json ");                                                         // Muestra la palabra obtenida.
        printf("%d '%s'\n", status, word);                                                  // Muestra el estado actual y la palabra obtenida.
        printJsonItem(NULL, "a", findRoot(item), 1);                                        // Muestra todos los JsonItem creados hasta ahora.
        printJsonItem(NULL, "a", father, 0);                                                // Muestra father.
        printJsonItem(NULL, "a", item, 0);                                                  // Muestra item.
        */
        deleteJsonItem(&father);                                                            // Borra father.
        deleteJsonItem(&item);                                                              // Borra item.
    } else                                                                                  // Si el último estado obtenido fue "close object/array" y está en la raíz.
        father = item;                                                                      // A father le lleva item para que lo retorne indicando json válido.
    /*-
    printf("\nTo return ");
    printf("%d '%s'\n", status, word);                                                      // Muestra el estado actual y la palabra obtenida.
    printJsonItem(NULL, "a", father, 1);                                                    // Muestra el JsonItem.
    */
    return father;                                                                          // Devuelve la raíz de los JsonItem.
}

/**
 * FIXME: Description of {@code getJsonItems}.
 * Crea JsonItem en base a la ruta de un archivo indicado por fileSource.
 */
LANGUAGE DLLIMPORT CALLING JsonItem* getJsonItems(char* fileSource) {
    int type = getJsonType(fileSource);                     // Obtiene el tipo de json que es.
    JsonItem* root = NULL;                                  // Crea el JsonItem que tendrá la raíz del json leído.

    if (type != -1) {                                       // Si el archivo existe.
        char word[100000] = "void";                         // Crea array para almacenar cada palabra del json.
        FILE* file = fopen(fileSource, "r+");               // Abre el archivo indicado por fileSource.
        root = obtainJsonItems(file, type, NULL, 1, word);  // Obtiene los JsonItem del archivo indicado por fileSource.
        fclose(file);                                       // Cierra el archivo indicado por fileSource.
    }

    return root;                                            // Devuelve la raíz de los JsonItem obtenidos.
}

/**
 * FIXME: Description of {@code swapJsonItems}.
 * Intercambia los valores de dos JsonItem.
 */
LANGUAGE DLLIMPORT CALLING void swapJsonItems(JsonItem** first, JsonItem** second) {
    JsonItem* one = *first;                     // Crea un nuevo JsonItem con lo apuntado por first.
    JsonItem* two = *second;                    // Crea un nuevo JsonItem con lo apuntado por second.
    JsonItem* aux = malloc(sizeof(JsonItem));   // Crea un nuevo JsonItem auxiliar.

    aux->type = one->type;                      // Almacena el campo type de one en aux.
    aux->name = one->name;                      // Almacena el campo name de one en aux.
    aux->value = one->value;                    // Almacena el campo value de one en aux.

    one->type = two->type;                      // Almacena el campo type de two en one.
    one->name = two->name;                      // Almacena el campo name de two en one.
    one->value = two->value;                    // Almacena el campo value de two en one.

    two->type = aux->type;                      // Almacena el campo type de aux en two.
    two->name = aux->name;                      // Almacena el campo name de aux en two.
    two->value = aux->value;                    // Almacena el campo value de aux en two.

    *first = two;                               // Pone a first a apuntar a two.
    *second = one;                              // Pone a second a apuntar a one.

    free(aux);                                  // Borra la memoria ocupada por third.
}
