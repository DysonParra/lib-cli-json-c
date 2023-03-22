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
 * @version 1.0     Implementación realizada.
 * @version 2.0     Documentación agregada.
 */
#include "json.h"
#include "tester.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Entrada principal del sistema.
 *
 * @param argc cantidad de argumentos pasados en la linea de comandos.
 * @param argv argumentos de la linea de comandos.
 * @return {0} si el programa se ejecutó correctamente.
 */
int main(int argc, char** argv) {

    char output[50] = "";
    // memcpy(output, (char*)"out.json", 9);

    // JsonItem* root = getJsonItems((char*)"test/file.json");
    // setJsonItemName(root, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa-------------------sssssssssssssssssss2");
    // setJsonItemValue(root, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa-------------------sssssssssssssssssss2");
    // printf("\nResult:\n");
    // printJsonItem(output, "w+", root, 1);

    char value[500] = "12?3";

    JsonItem* item0 = newJsonItem(1, (char*)"abc", (char*)"jhj", NULL, 0);
    JsonItem* item1 = newJsonItem(1, (char*)"def", &value[0], item0, 0);
    JsonItem* item2 = newJsonItem(1, (char*)"ghi", &value[0], item1, 0);
    JsonItem* item3 = newJsonItem(1, (char*)"jkl", &value[0], item1, 0);
    JsonItem* item4 = newJsonItem(1, (char*)"mno", &value[0], item0, 0);
    JsonItem* item5 = newJsonItem(1, (char*)"pqr", &value[0], item4, 0);
    JsonItem* item6 = newJsonItem(1, (char*)"stu", &value[0], item4, 0);
    JsonItem* item7 = newJsonItem(1, (char*)"wxy", &value[0], item0, 0);
    JsonItem* item8 = newJsonItem(1, (char*)"zab", &value[0], item7, 0);
    JsonItem* item9 = newJsonItem(1, (char*)"123", &value[0], item7, 0);
    JsonItem* item10 = newJsonItem(1, (char*)"456", &value[0], item9, 0);
    JsonItem* item11 = newJsonItem(1, (char*)"789", &value[0], item10, 0);
    JsonItem* item12 = newJsonItem(1, (char*)"0ab", &value[0], item10, 0);
    JsonItem* item13 = newJsonItem(1, (char*)"1bc", &value[0], item9, 0);
    JsonItem* item14 = newJsonItem(1, (char*)"2de", &value[0], item0, 0);
    JsonItem* item15 = newJsonItem(1, (char*)"3fg", &value[0], item14, 0);
    JsonItem* item16 = newJsonItem(1, (char*)"4hi", &value[0], item15, 0);
    JsonItem* item17 = newJsonItem(1, (char*)"5jk", &value[0], item14, 0);
    JsonItem* item18 = newJsonItem(1, (char*)"6lm", &value[0], item0, 0);
    JsonItem* item19 = newJsonItem(1, (char*)"7no", &value[0], item18, 0);
    JsonItem* item20 = newJsonItem(1, (char*)"8pq", &value[0], item19, 0);

    JsonItem* item21 = newJsonItem(1, (char*)"9rs", &value[0], NULL, 0);
    JsonItem* item22 = newJsonItem(1, (char*)"0tu", &value[0], item21, 0);
    JsonItem* item23 = newJsonItem(1, (char*)"1vw", &value[0], item22, 0);
    JsonItem* item24 = newJsonItem(1, (char*)"2xy", &value[0], item23, 0);

    printJsonItem(output, "w+", item0, 1);
    // setJsonItemName(item0, (char*)"namae1234567890");
    // setJsonItemValue(item0, (char*)"value1234567890");
    // printJsonItem(output, "w+", item0, 1);

    // printJsonItem(output, "w+", item21, 1);

    addChild(item20, item21);
    printJsonItem(output, "w+", item0, 1);

    /*-
    printJsonItem(output, "w+", item7, 1);
    updateJsonItemLevels(item7, 5);
    printJsonItem(output, "w+", item7, 1);

    printJsonItem(output, "w+", item0, 1);
    */
    /*-
    printf("DELETING\n");
    printJsonItem(output, "a", item7, 1);
    deleteJsonItem(&item7);

    printJsonItem(output, "a", item0, 1);
    printJsonItem(output, "a", item7, 1);
    */

    /*-
    swapJsonItems(&item0, &item2);
    printJsonItem(output, item0, 1);
    printJsonItem(output, item2, 1);
    */

    /*-
    char number[256] = "0.345e-20";
    printf("%d\n", validateNumber(number));
    */

    return 0;
}
