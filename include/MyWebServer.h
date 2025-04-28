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
#if MYCARD == ESP32
#include <WebServer.h>
#elif MYCARD == ESP8266
#include <ESP8266WebServer.h>
#endif

// Variables
#if MYCARD == ESP32
WebServer monWebServeur(80);           // Serveur web sur le port 80
#elif MYCARD == ESP8266
ESP8266WebServer monWebServeur(80);
#endif
/**
 * Fonction de gestion de la route /
 */
void handleRoot() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete root");
  timeClient.update();

  // Préparation du code HTML à retourner
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Bienvenue</h1><br>";
  out += "<h1>";
  out += timeClient.getFormattedTime();
  out += "</h1>";
  out += "Depuis cette page, vous pouvez<br><ul>";
  out +="<li><a href=\"scan\"> Scanner le WiFi</a></li>";
  out +="<li><a href=\"adafruit\"> Adafruit</a></li>";
  out +="<li><a href=\"format\"> Formater le SPIFFS</a></li>";
  out +="<li><a href=\"config\"> Configurer le WiFi</a></li></ul>";
  out += "</body></html>";

  // Envoi de la réponse en HTML
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /scan
 */
void handleScan() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete scan");

  // La carte scanne les réseaux WiFi à proximité
  int n = WiFi.scanNetworks();

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='5'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Page de scan</h1><br>";

  // Intégration des réseaux WiFi trouvés dans la page HTML
  if (n == 0) {
    MYDEBUG_PRINTLN("- AUCUN réseau WiFi trouvé");
  } else {
    out += "<ul>";
    for (int i = 0; i < n; ++i) {
      out += "<li>"+ WiFi.SSID(i) +"</li>";
    }
    out += "</ul>";
  }

  // Fin de la réponse HTML
  out += "</body></html>";

  // Envoi de la page HTML
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /config
 */
void handleConfig() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete config");

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Page de config</h1><br>";
  out += "</body></html>";

  // Envoi de la réponse HTML
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /format
 */
void handleFormat() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete format");
  //setupSPIFFS(true);
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='30'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Formatage fini</h1><br>";
  out += "<a href=\"/\"> Retour</a>";
  out += "</body></html>";
  monWebServeur.send(200, "text/html", out);
}

/**
 * Fonction de gestion de la route /adafruit
 */
void handleAdafruit() {
  MYDEBUG_PRINTLN("-WEBSERVER : requete adafruit");

  // Construction de la réponse HTML
  String out = "";
  out += "<html><head><meta http-equiv='refresh' content='5'/>";
  out += "<title>YNOV - Projet IoT B2</title>";
  out += "<style>body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }</style>";
  out += "</head><body>";
  out += "<h1>Adafruit</h1><br>";
  out += "<form action=\"\" method=\"get\" class=\"form-example\">";
  out += "<label for=\"slider\">Valeur du slider :</label><input type=\"range\" id=\"slider\" name=\"slider\" min=\"0\" max=\"100\" value=\"";
//  out += dSliderValue;
  out += "On verra ça plus tard";
  out += "\" step=\"10\">";
  out += "</form>";
  out += "</body></html>";

  // Envoi de la réponse HTML 
  monWebServeur.send(200, "text/html", out);
}

/**
 * En cas d'erreur de route, renvoi d'un message d'erreur 404
 */
void handleNotFound() {
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

/**
 * Initialisation du serveur web
 */
void setupWebServer(){
  // On a besoin d'une connexion WiFi !
  // Test de la connexion WiFi. Si elle n'est pas active alors on démarre le WiFi
  if (WiFi.status() != WL_CONNECTED){setupWiFi();}  // Connexion WiFi
  MYDEBUG_PRINTLN("-WEBSERVER : Démarrage");

  // Configuration de mon serveur web en définissant plusieurs routes
  // A chaque route est associée une fonction
  monWebServeur.on("/", handleRoot);
  monWebServeur.on("/scan", handleScan);
  monWebServeur.on("/config", handleConfig);
  monWebServeur.on("/adafruit", handleAdafruit);
  monWebServeur.onNotFound(handleNotFound);
  monWebServeur.on("/format", handleFormat);            // A ajouter quand le SPIFFFS est activé

  monWebServeur.begin();                                  // Démarrage du serveur
  MYDEBUG_PRINTLN("-WEBSERVER : Serveur Web démarré");
}

/**
 * Loop pour le serveur web afin qu'il regarde s'il a reçu des requêtes afin de les traiter
 */
void loopWebServer(void) {
  monWebServeur.handleClient();
}
