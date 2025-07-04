/**
 * \file MySPIFFS.h
 * \page spiffs SPIFFS
 * \brief (S)erial (P)eripheral (I)nterface (F)lash (F)ile (S)ystem 
 * La structure mémoire de l'ESP32 se découpe en plusieurs "zones":
 * \verbatim
|--------------|-------|---------------|--|--|--|--|--|
^              ^       ^               ^     ^
Sketch    OTA update   File system   EEPROM  WiFi config (SDK)
\endverbatim
 * Même si l'espace "File System" est situé sur le même chip que le programme,
 * le fait d'installer un nouveau programme n'affectera pas le contenu du File System. Cela peut nous permettre
 * de stocker des fichiers de données (type historique), de configuration ou du contenu pour un serveur web (feuille de
 * style, javascript ...) par exemple.
 * 
 * - Sketch : Cette zone est lue à chaque démarrage , les variables sont donc réinitialisées avec les valeurs par défaut.
 * - OTA Update : L’espace OTA est un dossier temporaire pour les mises à jour via OTA (Over The Air, cf. Device Management).
 * - EEPROM : Permet de stocker des infos, qui pourront être lues après redémarrage. Nous sommes sur une mémoire plus lente 
 *   (que la mémoire dédiée au sketch et qui dispose d’une durée de vie limitée en écriture . Ne pas en abuser).
 * - File system (SPIFFS) est un espace de stockage &quivalent à celui d'une carte SD. Selon les modules utilisés, la taille 
 *   du fichier systéme peut varier de 4MB/32Mb à 16MB/128Mb.
 *   
 * ESP8266 Sketch Data Upload 
 * Il s'agit d'un plugin Arduino qui permet d’uploader des fichiers dans la mémoire SPIFFS depuis 
 * l’interface Arduino. Rendez-vous sur la page GitHub du plugin pour la procédure d'installation : https://github.com/me-no-dev/arduino-esp32fs-plugin
 * - Télécharger https://github.com/me-no-dev/arduino-esp32fs-plugin/releases/download/1.0/ESP32FS-1.0.zip
 * - Dans le dossier arduino (emplacement du carnet de croquis défini dans les préférences), créér un dossier "tools", s'il n'est pas déjà présent.
 * - Décompresser le fichier zip téléchargé dans ce dossier "tools".
 * - Re-démarrer l'IDE Arduino pour qu'il prenne en compte le nouveau plugin au redémarrage.
 * - Dans le dossier de votre Sketch, créér un dossier "data". Déposer un ou plusieurs petits fichiers format txt dans ce dossier; ils seront 
 *   transférés dans la mémoire SPIFFS par le plugin (le fichier spiffs_test.txt est a priori déjà présent).
 * - Ouvrez vos Sketch Arduino dans l'IDE et sélectionner dans le menu "Outils" : ESP32 Sketch data upload.
 * Le contenu du dossier data sera transféré dans la mémoire SPIFFS. Le transfert sera plus ou moins long en fonction de la 
 * vitesse d’upload sélectionnée.
 *  
 * Comment accéder au File System :
 * Pour pouvoir utiliser les fonctions du File System il faut inclure la bibiolthèque SPIFFS.h
 * #include "SPIFFS.h"
 * Documentation complète : https://github.com/esp8266/Arduino/blob/master/doc/filesystem.rst
 * 
 * Dans cet exemple nous utilisons le format JSON pour lire et écrire dans le fichier de configuration.
 * Nous utilisons la bibliothèque Arduino JSON de Benoît Blanchon. A télécharger dans le gestionnaire de bibliothèque.
 * 
 * Fichier \ref MySPIFFS.h
 */
#pragma once

#include <LittleFS.h>
#define SPIFFS LittleFS

#ifndef MYSPIFFS_H
#define MYSPIFFS_H
#include <ArduinoJson.h>   //Arduino JSON by Benoit Blanchon : https://github.com/bblanchon/ArduinoJson

#include "MyNTP.h"
#include "MyWiFi.h"

inline String strConfigFile("/config.json");
inline String strTestFile("/spiffs_test.txt");
inline String strTrackingFile("/spiffs_tracking.txt");
inline File configFile, trackingFile;

inline void logTracking(const String &strTrackingText) {
    trackingFile = SPIFFS.open(strTrackingFile, "a");
    if (trackingFile) {
        timeClient.update();
        trackingFile.print(timeClient.getFormattedTime());
        trackingFile.print("\t");
        trackingFile.println(strTrackingText);
        trackingFile.close();
    } else {
        MYDEBUG_PRINTLN("-SPIFFS : Impossible d'ouvrir le fichier");
    }
}

inline void setupSPIFFS(bool bFormat = false) {
    MYDEBUG_PRINTLN("-SPIFFS : Montage du système de fichier");

    if (SPIFFS.begin()) {
        MYDEBUG_PRINTLN("-SPIFFS : MONTE");

        if (bFormat) {
            SPIFFS.format(); // ------------- Au besoin, pour formatter le système de fichiers
            bFormat = false;
        }

        // Fichier de test
        if (SPIFFS.exists(strTestFile)) {
            // ------------------------- Le fichier existe
            File testFile = SPIFFS.open(strTestFile, "r");
            if (testFile) {
                MYDEBUG_PRINTLN("-SPIFFS : Lecture du fichier spiffs_test.txt");
                while (testFile.available()) {
                    Serial.write(testFile.read());
                }
                MYDEBUG_PRINTLN("");
            }
        }

        // Fichier de configuration
        if (!SPIFFS.exists(strConfigFile)) {
            MYDEBUG_PRINTLN("-SPIFFS: Le fichier de configuration n'existe pas");
            File configFile = SPIFFS.open(strConfigFile, "w");
            if (configFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier créé");
                DynamicJsonDocument jsonDocument(2048); // Augmenter la taille pour les distributeurs

                // Création de la structure des distributeurs
                JsonObject distributeurs = jsonDocument.createNestedObject("distributeurs");

                // Configuration Croquette
                JsonObject croquette = distributeurs.createNestedObject("croquette");
                croquette["nom"] = "Croquette";
                croquette["nbRation"] = 30000;
                croquette["nbMin"] = 1000;
                croquette["nbMax"] = 50000;
                croquette["nbBySecSend"] = 10;
                croquette["nbSendRation"] = 1000;
                croquette["copulation"] = 0;
                croquette["copulationSec"] = 0;
                croquette["eat"] = 0;

                // Configuration Poisson Rouge
                JsonObject poissonRouge = distributeurs.createNestedObject("poissonRouge");
                poissonRouge["nom"] = "Poisson Rouge";
                poissonRouge["nbRation"] = 30;
                poissonRouge["nbMin"] = 20;
                poissonRouge["nbMax"] = 100;
                poissonRouge["nbBySecSend"] = 10;
                poissonRouge["nbSendRation"] = 3;
                poissonRouge["copulation"] = 3;
                poissonRouge["copulationSec"] = 15;
                poissonRouge["eat"] = 2000;

                // Configuration Achigan
                JsonObject achigan = distributeurs.createNestedObject("achigan");
                achigan["nom"] = "Achigan";
                achigan["nbRation"] = 10;
                achigan["nbMin"] = 5;
                achigan["nbMax"] = 20;
                achigan["nbBySecSend"] = 10;
                achigan["nbSendRation"] = 2;
                achigan["copulation"] = 1;
                achigan["copulationSec"] = 40;
                achigan["eat"] = 4;

                // Configuration Achigan Restaurant
                JsonObject achiganResto = distributeurs.createNestedObject("achiganResto");
                achiganResto["nom"] = "Achigan du Restaurant";
                achiganResto["nbRation"] = 12;
                achiganResto["nbMin"] = 5;
                achiganResto["nbMax"] = 30;
                achiganResto["nbBySecSend"] = 10;
                achiganResto["nbSendRation"] = 1;
                achiganResto["copulation"] = 1;
                achiganResto["copulationSec"] = 40;
                achiganResto["eat"] = 1;

                // Ajout des paramètres WiFi
                jsonDocument["ssid"] = String("");
                jsonDocument["password"] = String("");

                // Sérialisation du JSON dans le fichier
                if (serializeJson(jsonDocument, configFile) == 0) {
                    MYDEBUG_PRINTLN("-SPIFFS : Impossible d'écrire le JSON dans le fichier de configuration");
                }
                configFile.close();
                MYDEBUG_PRINTLN("-SPIFFS : Fichier fermé");
            } else {
                MYDEBUG_PRINTLN("-SPIFFS : Impossible d'ouvrir le fichier en écriture");
            }
        }


        // Fichier de Tracking
        if (SPIFFS.exists(strTrackingFile)) {
            // ------------------------- Le fichier existe
            //Ouverture du fichier en lecture
            MYDEBUG_PRINTLN("-SPIFFS : Lecture du fichier de tracking");
            trackingFile = SPIFFS.open(strTrackingFile, "r");
            if (trackingFile) {
                while (trackingFile.available()) {
                    Serial.write(trackingFile.read());
                }
                MYDEBUG_PRINTLN("");
            }
            //trackingFile.close();
            //MYDEBUG_PRINTLN("-SPIFFS: Fichier fermé");
        } else {
            // ------------------- Le fichier n'existe pas
            // Initialisation du fichier de configuration avec des valeurs vides
            MYDEBUG_PRINTLN("-SPIFFS: Le fichier de tracking n'existe pas");
            trackingFile = SPIFFS.open(strTrackingFile, "w");
            if (trackingFile) {
                MYDEBUG_PRINTLN("-SPIFFS: Fichier créé");
                // Fermeture du fichier
                trackingFile.close();
                MYDEBUG_PRINTLN("-SPIFFS : Fichier fermé");
            } else {
                MYDEBUG_PRINTLN("-SPIFFS : Impossible d'ouvrir le fichier en ecriture");
            }
        }

        //SPIFFS.end();
    } else {
        MYDEBUG_PRINT("-SPIFFS : Impossible de monter le système de fichier");
    }
}
#endif
