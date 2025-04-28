/**
 * \file MyDebug.h
 * \page debug Debug
 * \brief Optimisation de l'utilisation du port série en mode DEBUG
 * 
 * L'utilisation du port série demande des ressources, donc si on n'est pas en mode DEBUG, on peut s'en passer.<br>
 * Ainsi tous les appels direct au port série de type Serial.print() ou Serial.println() seront remplacés par
 * l'utilisation de macros.
 * 
 * Ainsi, la macro MYDEBUG permet de configurer le comportement :
 * - #define MYDEBUG 1 : le compilateur remplace les macros par des appels au port série,
 * - #define MYDEBUG 0 : le compilateur remplace les macros par ... rien et les ignore donc.
 * En plaçant la définition de MYDEBUG dans le fichier principal, cela permet d'accéder directement à la "configuration" de votre éxécutable.
 * 
 * \note L'activation du mode debug se fait à l'aide de la macro MYDEBUG. Il faut donc qu'elle soit initialisée \b avant 
 * le #include de ce fichier si on veut qu'il soit pris en compte.
 * 
 * 
 * Fichier \ref MyDebug.h
 */

#ifndef MYDEBUG_H
#define MYDEBUG_H

#include <Arduino.h>

#ifdef MYDEBUG
 #define MYDEBUG_PRINT(x)     Serial.print(x)
 #define MYDEBUG_PRINTDEC(x)  Serial.print(x, DEC)
 #define MYDEBUG_PRINTHEX(x)  Serial.print(x, HEX)
 #define MYDEBUG_PRINTLN(x)   Serial.println(x)
 #define MYDEBUG_PRINTF(a,b,c,d,e) Serial.printf(a,b,c,d,e)
#else
 #define MYDEBUG_PRINT(x)
 #define MYDEBUG_PRINTDEC(x)
 #define MYDEBUG_PRINTHEX(x)
 #define MYDEBUG_PRINTLN(x)
 #define MYDEBUG_PRINTF(x)
#endif

inline void setupDebug() {
#ifdef MYDEBUG
 Serial.begin(115200);
 MYDEBUG_PRINTLN("Ouverture du port série");
#endif
}

#endif

