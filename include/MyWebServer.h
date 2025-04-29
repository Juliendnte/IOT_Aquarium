/**
 * \file MyWebServer.h
 * \page webserver Serveur Web
 * \brief Un serveur web interne à l'ESP32 pour configurer et voir les données
 * 
 * Comme son nom l'indique, un objet WebServer permet d'écouter des requêtes web, de les analyser et
 * d'y répondre. Le documentation est disponible ici : https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/src/WebServer.h.
 * Ainsi que de nombreux exemples : https://github.com/espressif/arduino-esp32/tree/master/libraries/WebServer/examples
 * 
 * Le WebServer est instancié avec le constructeur avec un numéro de port.
 * Les princales fonctions sont :
 * - begin() pour démarrer le serveur
 * - on(uri, fonction) pour appeler une fonction dédiée lorsqu'une route est sollicitée
 * - onNotFound(fonction) pour aller une fonction par défaut quand la route n'est pas connue
 * - handleClient() à appeler dans la loop pour gérer la récupération et le traitemnet de nouveaux messages
 * - send(code, type de contenu, contenu) pour envoyer une réponse avec un code, et un contenu d'un type donné
 * 
 * Dans cet exemple, le serveur web reçoit des requêtes HTTP et y répond sur les routes :
 * - / avec la fonction handleRoot()
 *   Elle donne accès aux fonctionnalités proposées
 * - /scan avec la fonction handleScan()
 *   Affiche la liste des réseaux WiFi disponibles
 * - /config avec la fonction handleConfig()
 *   Affiche un formulaire pour configurer la carte
 * - ...
 * - et avec handleNotFound() si la route n'est pas connue
 * 
 * Fichier \ref MyWebServer.h
 */

// Librairies nécessaires, en fonction de la carte utilisée
#pragma once
#include <ESP8266WebServer.h>

#include "MyDebug.h"
#include "MyWiFi.h"
#include <NTPClient.h>

// Déclaration des fonctions externes
extern void publishToMQTT(const char *feed, const String &value);

extern int getReadyCount();

// Variables
inline ESP8266WebServer monWebServeur(80);
/**
 * Fonction de gestion de la route /
 */
inline void handleRoot() {
    MYDEBUG_PRINTLN("-WEBSERVER : requete root");
    timeClient.update();

    if (monWebServeur.hasArg("commande")) {
        String commande = monWebServeur.arg("commande");
        publishToMQTT("commande", commande);
    }

    String out = "";
    out += "<html><head>";
    out += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    out += "<meta http-equiv='refresh' content='30'/>";
    out += "<title>YNOV - Projet IoT B2</title>";
    out += "<style>";
    out += "* { margin: 0; padding: 0; box-sizing: border-box; }";
    out += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background: #f0f2f5; color: #1a1a1a; line-height: 1.6; }";
    out += ".container { max-width: 1000px; margin: 2rem auto; padding: 0 20px; }";
    out += ".header { background: #ffffff; padding: 2rem; border-radius: 10px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-bottom: 2rem; }";
    out += ".header h1 { color: #2c3e50; font-size: 2rem; margin-bottom: 1rem; }";
    out += ".card { background: #ffffff; padding: 2rem; border-radius: 10px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-bottom: 2rem; }";
    out += ".form-group { margin-bottom: 1.5rem; }";
    out += ".form-group h2 { color: #2c3e50; margin-bottom: 1rem; }";
    out += "select { padding: 0.8rem; border: 1px solid #ddd; border-radius: 5px; width: 200px; margin-right: 1rem; font-size: 1rem; }";
    out += ".btn { background: #4CAF50; color: white; padding: 0.8rem 2rem; border: none; border-radius: 5px; cursor: pointer; font-size: 1rem; transition: background 0.3s ease; }";
    out += ".btn:hover { background: #45a049; }";
    out += ".status { background: #ffffff; padding: 2rem; border-radius: 10px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
    out += ".status h2 { color: #2c3e50; margin-bottom: 1rem; }";
    out += ".status-value { font-size: 2rem; color: #4CAF50; font-weight: bold; }";
    out += "@media (max-width: 600px) {";
    out += "  .container { margin: 1rem auto; }";
    out += "  .header, .card, .status { padding: 1rem; }";
    out += "  select { width: 100%; margin-bottom: 1rem; }";
    out += "  .btn { width: 100%; }";
    out += "}";
    out += "</style>";
    out += "</head><body>";

    out += "<div class='container'>";
    out += "<div class='header'>";
    out += "<h1>Tableau de bord - " + timeClient.getFormattedTime() + "</h1>";
    out += "</div>";

    out += "<div class='card'>";
    out += "<div class='form-group'>";
    out += "<form action='/' method='post'>";
    out += "<h2>Commande d'Achigan</h2>";
    out += "<select name='commande'>";
    for(int i = 1; i <= 10; i++) {
        out += "<option value='" + String(i) + "'>" + String(i) + "</option>";
    }
    out += "</select>";
    out += "<button type='submit' class='btn'>Envoyer la commande</button>";
    out += "</form>";
    out += "</div>";
    out += "</div>";

    out += "<div class='status'>";
    out += "<h2>Etat actuel</h2>";
    out += "<div class='status-value'>" + String(getReadyCount()) + "</div>";
    out += "<p>Recettes pretes</p>";
    out += "</div>";

    out += "</div></body></html>";

    monWebServeur.send(200, "text/html", out);
}

/**
 * En cas d'erreur de route, renvoi d'un message d'erreur 404
 */
inline void handleNotFound() {
    MYDEBUG_PRINTLN("-WEBSERVER : erreur de route");

    // Construction de la réponse HTML
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += monWebServeur.uri();
    message += "\nMethod: ";
    message += (monWebServeur.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += monWebServeur.args();
    message += "\n";
    for (uint8_t i = 0; i < monWebServeur.args(); i++) {
        message += " " + monWebServeur.argName(i) + ": " + monWebServeur.arg(i) + "\n";
    }

    // Envoi de la réponse HTML
    monWebServeur.send(404, "text/plain", message);
}

inline void handleDebug() {
    String out = "";
    out += "<html><head>";
    out += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    out += "<meta http-equiv='refresh' content='5'/>";
    out += "<title>Debug ESP8266</title>";
    out += "<style>";
    out += "body { font-family: monospace; background: #1e1e1e; color: #00ff00; margin: 20px; }";
    out += ".debug-container { background: #000; padding: 20px; border-radius: 5px; }";
    out += ".debug-title { color: #fff; margin-bottom: 20px; }";
    out += "#serial-output { white-space: pre-wrap; }";
    out += ".system-info { margin-bottom: 20px; padding-bottom: 20px; border-bottom: 1px solid #333; }";
    out += "</style>";
    out += "</head><body>";

    out += "<div class='debug-container'>";
    out += "<h1 class='debug-title'>ESP8266 Debug Console</h1>";

    // Informations système
    out += "<div class='system-info'>";
    out += "ESP8266 Debug Information:\n";
    out += "-------------------------\n";
    out += "Free Heap: " + String(ESP.getFreeHeap()) + " bytes\n";
    out += "WiFi Status: " + String(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected") + "\n";
    out += "WiFi SSID: " + String(WiFi.SSID()) + "\n";
    out += "IP Address: " + WiFi.localIP().toString() + "\n";  // Utilisation directe de toString()
    out += "Uptime: " + String(millis() / 1000) + " seconds\n";
    out += "</div>";

    // Affichage des logs
    out += "<div id='serial-output'>";
    out += "Debug Logs:\n";
    out += "-------------------------\n";
    for (int i = 0; i < LOG_BUFFER_SIZE; i++) {
        int index = (logIndex - 1 - i + LOG_BUFFER_SIZE) % LOG_BUFFER_SIZE;
        if (logBuffer[index].length() > 0) {
            out += logBuffer[index] + "\n";
        }
    }
    out += "</div></div>";
    out += "</body></html>";

    monWebServeur.send(200, "text/html", out);
}

/**
 * Initialisation du serveur web
 */
inline void setupWebServer() {
    // On a besoin d'une connexion WiFi !
    // Test de la connexion WiFi. Si elle n'est pas active alors on démarre le WiFi
    if (WiFi.status() != WL_CONNECTED) { setupWiFi(); } // Connexion WiFi
    MYDEBUG_PRINTLN("-WEBSERVER : Démarrage");

    // Configuration de mon serveur web en définissant plusieurs routes
    // A chaque route est associée une fonction
    monWebServeur.on("/", HTTP_GET, handleRoot);
    monWebServeur.on("/", HTTP_POST, handleRoot);
    monWebServeur.on("/debug", HTTP_GET, handleDebug);

    monWebServeur.onNotFound(handleNotFound);

    monWebServeur.begin(); // Démarrage du serveur
    MYDEBUG_PRINTLN("-WEBSERVER : Serveur Web démarré");
}

/**
 * Loop pour le serveur web afin qu'il regarde s'il a reçu des requêtes afin de les traiter
 */
inline void loopWebServer() {
    monWebServeur.handleClient();
}
