/**
 * \file MyWifi.h
 * \page wifi Wifi ESP32
 * 
 * \subsection howWiFi Comment ça marche ?
 * Il existe plusieurs façons différentes d'initialiser le mode WiFi d'un ESP32.
 * - Les terminaux qui se connectent aux réseaux WiFi proposés par des Access Points sont appelés 
 *   des stations (mode WIFI_STA).
 *   \image html ESP32-Station-Mode.png
 * - Les terminaux peuvent fournir un point d'accès ou Access Point (WIFI-AP) et se comporter comme 
 *   un hub pour une ou plusieurs stations. Classiquement un Access Point est connecté à un réseau 
 *   filaire et intégre un routeur pour donner accès à internet via le WiFi. Chaque Access Point 
 *   fournit un SSID (Service Set IDentifier) visible et accessible pour les stations (autres terminaux 
 *   comme un téléphone mobile ou un ordinateur).
 *   \image  html ESP32-Access-Point-Mode.png
 * - Les terminaux peuvent également activer les 2 modes de fonctionnement en même temps (Station et
 *   Access Point, en mode WIFI_AP_STA).
 * 
 * \subsection foncWiFi Principales fonctions
 * Les principales fonctions de la librairie sont :
 * - Wifi.mode(<MODE>) qui prend en paramètre un des modes suivants :
 *   - WIFI_STA quand la carte veut accéder à une réseau WiFi en mode client
 *   - WIFI_AP quand la carte veut être un point d'accès (être un SSID)
 *   - WIFI_AP_STA pour faire les 2 à la fois
 *   - WIFI_OFF comme son nom l'indique
 * - WiFi.begin("nom-du-SSID", "mot-de-passe")
 * - WiFi.status() qui vous retourne
 *   - 0 : WL_IDLE_STATUS when Wi-Fi is in process of changing between statuses
 *   - 1 : WL_NO_SSID_AVAIL quand le SSID indiqué n'est pas atteingnable
 *   - 3 : WL_CONNECTED quand la connexion a été établi
 *   - 4 : WL_CONNECT_FAILED si le mot de passe du SSID est incorrect
 *   - 6 : WL_DISCONNECTED si le module n'est pas configuration en mode Station
 * - WiFi.localIP() en mode WIFI_STA affiche l'adresse IP de la carte
 * - WiFi.scanNetworks() vous permet de scanner et récupérer les informations sur les
 *   SSID à portée.
 * 
 * Les classes WiFiClient et WiFiServer vous permettent de manipuler les communications
 * en tant que client ou server.
 * 
 * \subsection TPWIfi Prise en main
 * Pour prendre en main le Wifi sur votre carte, je vous invite à découvrir les fichiers exemples suivants :
 * -# <b>Exemples/WiFi/WiFiScan</b> 
 *    scanne les réseaux WiFi (Access Points) visibles par la carte qui est en mode STAtion et de les liste 
 *    sur le moniteur de sortie.
 * -# <b>Exemples/WiFi/WiFiClient</b> 
 *    permet à la carte d'accéder à un réseau WiFi en mode STAtion; la sélection du réseau WiFi se fait en 
 *    remplissant les valeurs de STASSID et STAPSK. Une fois connecté la carte créé un client WiFi TCP et va 
 *    requéter un serveur et afficher les informations qu'elle récupère.
 * -# <b>Exemples/WiFi/WiFiMulti</b> 
 *    permet de lister plusieurs Access Points auquels vous souhaitez/pouvez accéder. Si le premier SSID 
 *    n'est pas disponible alors la carte tente de se connecter au suivant ... Cette fonctionnalité est 
 *    proposée par la classe WiFiMulti.
 *    Vous pouvez ainsi configurer les réseaux WiFi de l'école, de chez vous et celui de votre téléphone, 
 *    par exemple. A noter qu'il faut bien penser à appeler la méthode wifiMulti.run() dans la loop() pour 
 *    surveiller l'état de la connexion à chaque itération et faire le nécessaire si besoin est.
 * -# <b>Exemples/WiFi/WiFiClientBasic</b> 
 *    se connecte en mode STA à un SSID via WiFiMulti et réalise une requête à un serveur HTTP, récupère la 
 *    réponse du serveur et l'affiche sur le moniteur série; et recommence 10 seconds après.
 * -# <b>Exemples/WiFi/WiFiAccessPoint</b> 
 *    permet de créer un point d'accès WiFi. En lançant cet exemple la carte crée un réseau WiFi avec le nom 
 *    renseigné dans ssid; vous pourrez le voir depuis votre ordinateur ou votre téléphone, et y accéder 
 *    en renseignant le mot de passe indiqué dans password.
 *    Une fois connecté, vous pourrez vous rendre sur l'adresse http://192.168.4.1 depuis un navigateur web et
 *    la carte vous renverra le code HTML présent dans la méthode handleRoot() que le serveur appelle pour 
 *    la route "/".
 * -# <b>Exemples/DNSServer/CaptivePortal</b>
 *    démarre la carte en mode Access Point et répond à toutes les requêtes en retournant une page web.
 *    Ainsi dès qu'un terminal (ordinateur, téléphone ...) se connecte au SSID il est routé sur le portail 
 *    captif sur la base de cette page web.
 *
 * Je vous invite à installer la librairie WiFiManager et à tester l'exemple fourni :
 * -# Dans le menu "Outils/Gérer les bibliothèques", renseigner "captive" dans la barre de recherche et installez
 *    la dernière version de WiFiManager by Tablatronix.
 * -# Dans tester l'exemple qui se trouve sur <b>Exemple/WiFiManager/Basic</b>.
 * 
 * \subsection exemple Exemple
 * Dans cet exemple, l'objet est utilisé avec les 2 modes Access Point et Station :
 * - Access Point pour que l'utilisateur puisse accéder à un serveur web proposé par la carte
 * - Station pour se connecter à un Access Point pour accéder au réseau externe (internet).
 * 
 * Je peux ajouter un serveur DNS pour réaliser un portail captif ...
*/

// Librairies nécessaires, en fonction de la carte utilisée
#pragma once
#include <ESP8266WiFi.h>  // WiFi ESP8266

// Variables
// pour le mode STATION
#include "MyDebug.h"
#include "WIFI_CREDENTIALS.h"

// ------------------------------------------------------------------------------------------------
// CONFIGURATION DU WIFI
// ------------------------------------------------------------------------------------------------
/**
 * Fonction de configuration WiFi de ma carte.
 * 
 * La mode WIFI_AP_STA est utilisé afin de pouvoir à la fois :
 * - Se connecter à un point d'accès réseau en tant que station
 * - Proposer une connexion à l'utilisateur pour accéder à mon serveur web
 */
#ifndef MY_WIFI_H
#define MY_WIFI_H

inline void setupWiFi() {
    MYDEBUG_PRINTLN();
    MYDEBUG_PRINT("-WIFI : Configuration");

    // Configuration de la carte en mode Access Point ET Station
    WiFi.mode(WIFI_AP_STA);
    //WiFi.mode(WIFI_STA);

    // Démarrage du mode Access Point
    WiFi.softAP(ap_ssid, ap_password);
    // J'affiche l'adresse IP de mon point d'accès
    MYDEBUG_PRINT("-WIFI : Access Point mis à disposition : ");
    MYDEBUG_PRINTLN(WiFi.softAPIP());

    // Démarrage du mode Station
    WiFi.begin(station_ssid, station_password);

    MYDEBUG_PRINT("-WIFI : Connexion au réseau : ");
    MYDEBUG_PRINTLN(station_ssid);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        MYDEBUG_PRINT(".");
    }
    MYDEBUG_PRINTLN("");
    // J'affiche l'adresse IP de ma carte
    MYDEBUG_PRINT("-WIFI : connecté en mode Station avec l'adresse IP : ");
    MYDEBUG_PRINTLN(WiFi.localIP());
}
#endif
