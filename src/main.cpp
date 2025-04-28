#define MYDEBUG         1
#define MYCARD          ESP8266

int i = 0;

#include "MyDebug.h"        // Debug
#include "MyWiFi.h"         // WiFi
#include "MyTicker.h"       // Tickers
#include "MyDistributeur.h"


void setup() {
    // 1. Initialisation du debug
    Serial.begin(115200);
    delay(2000); // Augmenté à 2 secondes
    setupDebug();
    MYDEBUG_PRINTLN("----- SETUP -----");

    // 2. WiFi avec gestion d'erreur
    try {
        setupWiFi();
        MYDEBUG_PRINTLN("----- WIFI OK -----");
        delay(5000);
        // Attente de stabilisation du WiFi
    } catch (const std::exception &e) {
        MYDEBUG_PRINT("Erreur WiFi : ");
        MYDEBUG_PRINTLN(e.what());
        return;
    }
    delay(5000);

    // 3. Ticker avec délai plus long
    try {
        setupTicker();
        MYDEBUG_PRINTLN("----- TICKER OK -----");
        delay(5000); // Attente de stabilisation du Ticker
    } catch (const std::exception &e) {
        MYDEBUG_PRINT("Erreur Ticker : ");
        MYDEBUG_PRINTLN(e.what());
        return;
    }

    // 4. Distributeur avec délai et surveillance
    try {
        MYDEBUG_PRINTLN("Démarrage de l'initialisation du distributeur");
        setupDistributeur();
        MYDEBUG_PRINTLN("----- DISTRIBUTEUR OK -----");
        delay(5000); // Attente de stabilisation
    } catch (const std::exception &e) {
        MYDEBUG_PRINT("Erreur Distributeur : ");
        MYDEBUG_PRINTLN(e.what());
        return;
    }

    MYDEBUG_PRINTLN("----- SETUP TERMINÉ -----");
}

void loop() {
    static bool firstLoop = true;
    static unsigned long lastWifiCheck = 0;
    static unsigned long lastMqttProcessing = 0;
    const unsigned long processingInterval = 1000; // 1 seconde entre les traitements

    // Ne pas utiliser yield() directement
    delay(100);

    if (firstLoop) {
        MYDEBUG_PRINTLN("Première itération de la boucle");
        firstLoop = false;
        delay(2000);
    }

    // Vérification périodique du WiFi
    unsigned long currentMillis = millis();
    if (currentMillis - lastWifiCheck >= 30000) {
        if (WiFi.status() != WL_CONNECTED) {
            MYDEBUG_PRINTLN("Perte de connexion WiFi - Tentative de reconnexion");
            WiFi.reconnect();
            delay(5000);
        }
        lastWifiCheck = currentMillis;
    }

    // Traitement MQTT périodique
    if (currentMillis - lastMqttProcessing >= processingInterval) {
        try {
            loopDistributeur();
            lastMqttProcessing = currentMillis;
        } catch (const std::exception &e) {
            MYDEBUG_PRINT("Erreur dans la boucle : ");
            MYDEBUG_PRINTLN(e.what());
            delay(5000);
        }
    }

    // Délai de base pour éviter la surcharge
    delay(100);
}
