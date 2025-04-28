// Distributeur.h
#ifndef DISTRIBUTEUR_H
#define DISTRIBUTEUR_H

#include <ESP8266WiFi.h>
#include <iostream>
#include <utility>
#include <Ticker.h>
#include <WiFiClient.h>
#include "MyDebug.h"
#include "Adafruit_MQTT_Client.h"
#include "../lib/env.hpp"
/************************* Configuration *************************************/
#define IO_SERVER         "io.adafruit.com"
#define IO_SERVERPORT     1883

static std::string key = env.get("IO_KEY");

#define IO_USERNAME       "Juliendnte"
#define IO_KEY           key.c_str()


// Feeds
#define FEED_NB_RATION_CROQUETTE       "/feeds/croquette.nbration"
#define FEED_NB_RATION_POISSON_ROUGE       "/feeds/poisson-rouge.nbration"
#define FEED_NB_RATION_ACHIGAN       "/feeds/achigan.nbration"
#define FEED_NB_RATION_RESTO       "/feeds/resto.nbration"
#define FEED_COMMANDE       "/feeds/commande"
#define FEED_READY       "/feeds/ready"

/************************** Variables ****************************************/
// Instanciation du client WiFi qui servira à se connecter au broker Adafruit
inline WiFiClient client;
// Instanciation du client Adafruit avec les informations de connexion
// En haut du fichier Distributeur.h
#define MQTT_TIMEOUT_MS     5000
#define MQTT_MAX_PACKET_SIZE 1024

// Dans la création du client MQTT
inline Adafruit_MQTT_Client MyAdafruitMqtt(&client, IO_SERVER, IO_SERVERPORT, IO_USERNAME, IO_USERNAME, IO_KEY);

inline void setupMQTT() {
    client.setTimeout(MQTT_TIMEOUT_MS);
    client.setNoDelay(true);

    // Configuration supplémentaire pour le client MQTT
    MyAdafruitMqtt.setKeepAliveInterval(15); // 15 secondes
}


// Variable de stockage de la valeur du slider
inline Ticker MyAdafruitTicker;
/****************************** Feeds ****************************************/
// Création des Feed auxquels nous allons souscrire :

inline Adafruit_MQTT_Subscribe subNbRationCroquette = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt,
                                                                              IO_USERNAME FEED_NB_RATION_CROQUETTE,
                                                                              MQTT_QOS_1);
inline Adafruit_MQTT_Subscribe subNbRationPoissonRouge = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt,
    IO_USERNAME FEED_NB_RATION_POISSON_ROUGE,
    MQTT_QOS_1);
inline Adafruit_MQTT_Subscribe subNbRationAchigan = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt,
                                                                            IO_USERNAME FEED_NB_RATION_ACHIGAN,
                                                                            MQTT_QOS_1);
inline Adafruit_MQTT_Subscribe subNbRationResto = Adafruit_MQTT_Subscribe(
    &MyAdafruitMqtt, IO_USERNAME FEED_NB_RATION_RESTO,
    MQTT_QOS_1);
inline Adafruit_MQTT_Subscribe subCommande = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt, IO_USERNAME FEED_COMMANDE,
                                                                     MQTT_QOS_1);
inline Adafruit_MQTT_Subscribe subReady = Adafruit_MQTT_Subscribe(&MyAdafruitMqtt, IO_USERNAME FEED_READY,
                                                                  MQTT_QOS_1);

inline Adafruit_MQTT_Publish pubNbRationCroquette = Adafruit_MQTT_Publish(&MyAdafruitMqtt,
                                                                          IO_USERNAME FEED_NB_RATION_CROQUETTE);
inline Adafruit_MQTT_Publish pubNbRationPoissonRouge = Adafruit_MQTT_Publish(&MyAdafruitMqtt,
                                                                             IO_USERNAME FEED_NB_RATION_POISSON_ROUGE);
inline Adafruit_MQTT_Publish pubNbRationAchigan = Adafruit_MQTT_Publish(
    &MyAdafruitMqtt, IO_USERNAME FEED_NB_RATION_ACHIGAN);
inline Adafruit_MQTT_Publish pubNbRationResto =
        Adafruit_MQTT_Publish(&MyAdafruitMqtt, IO_USERNAME FEED_NB_RATION_RESTO);
inline Adafruit_MQTT_Publish pubCommande = Adafruit_MQTT_Publish(&MyAdafruitMqtt, IO_USERNAME FEED_COMMANDE);
inline Adafruit_MQTT_Publish pubReady = Adafruit_MQTT_Publish(&MyAdafruitMqtt, IO_USERNAME FEED_READY);

inline Ticker croquetteTicker;
inline Ticker poissonRougeTicker;
inline Ticker achiganTicker;
inline Ticker achiganRestoTicker;

inline void connectAdafruitIO() {
    static unsigned long lastAttempt = 0;
    constexpr unsigned long retryInterval = 15000;

    unsigned long now = millis();
    if (now - lastAttempt < retryInterval) {
        return;
    }
    lastAttempt = now;

    // Vérification du WiFi
    if (WiFi.status() != WL_CONNECTED) {
        MYDEBUG_PRINTLN("WiFi non connecté");
        return;
    }

    MYDEBUG_PRINT("-AdafruitIO : Connexion au broker... ");

    // Nettoyage des connexions existantes
    client.stop();
    delay(1000);

    // Tentative de connexion avec plus de détails
    int8_t ret = MyAdafruitMqtt.connect();
    uint8_t retries = 3;

    while (retries--) {
        ret = MyAdafruitMqtt.connect();
        if (ret == 0) break;

        MYDEBUG_PRINT("\nErreur de connexion (");
        MYDEBUG_PRINT(ret);
        MYDEBUG_PRINT("): ");
        MYDEBUG_PRINTLN(MyAdafruitMqtt.connectErrorString(ret));
        delay(2000);
    }

    if (ret == 0) {
        MYDEBUG_PRINTLN("OK");

        // Configuration des souscriptions
        if (!MyAdafruitMqtt.subscribe(&subNbRationCroquette))
            MYDEBUG_PRINTLN("Échec sub croquette");
        if (!MyAdafruitMqtt.subscribe(&subNbRationPoissonRouge))
            MYDEBUG_PRINTLN("Échec sub poisson rouge");
        if (!MyAdafruitMqtt.subscribe(&subNbRationAchigan))
            MYDEBUG_PRINTLN("Échec sub achigan");
        if (!MyAdafruitMqtt.subscribe(&subNbRationResto))
            MYDEBUG_PRINTLN("Échec sub resto");
        if (!MyAdafruitMqtt.subscribe(&subCommande))
            MYDEBUG_PRINTLN("Échec sub commande");
        MYDEBUG_PRINTLN("=== Connexion Adafruit IO réussie ===");
    } else {
        MYDEBUG_PRINTLN("=== Échec de connexion Adafruit IO ===");
        MYDEBUG_PRINTLN("Code d'erreur : " + String(ret));
        MYDEBUG_PRINTLN("Détail : " + String(MyAdafruitMqtt.connectErrorString(ret)));
    }
}

inline bool ensureConnected() {
    if (!MyAdafruitMqtt.connected()) {
        MYDEBUG_PRINTLN("Reconnexion nécessaire avant publication");
        connectAdafruitIO();
        return MyAdafruitMqtt.connected();
    }
    return true;
}

/**
 * Class Distributeur représente un distributeur de rations dans une suite N.
 * Le distributeur N reçoit une commande et doit donnée le nombre de ration demandé
 * S'il n'a pas le nombre demandé il doit faire une commande de nbSendRation toute les nbBySecSend seconde jusqu'à la différence de la commande et le nombre de ration à distributeur N - 1 en respectant le nombre minimal de rations.
 * Et lorsque nous arrivons au distributeur zéro il doit envoyer une notification.
 *
 * @author DANTE
 * @version 1.1
 * @date 2025-04-23
 *
 * @param int nbRation, Nombre de rations dans le distributeur (public)
 * @param int nbMin, Nombre minimal de rations (private)
 * @param int nbMax, Nombre maximal de rations (private)
 * @param int nbBySecSend, Nombre de seconde avant la prochaine ration (private)
 * @param int nbSendRation, Quantité a envoyé (private)
 * @param int copulation, Nombre de ration a augmenté (private)
 * @param int copulationSec, Nombre de seconde avant la copulation (private)
 * @param int eat, Quantité que l'on mange (private)
 * @param Distributeur* | nullptr precedent, le distributeur N - 1 (private)
 */
class MyDistributeur {
    int _nbMin = 2;
    int _nbMax = 20;
    int _copulation = 2;
    float _copulationSec = 10;
    float _nbBySecSend = 10;
    int _nbSendRation = 1;
    int _eat = 2;
    int nombreRestant = 0;
    Ticker envoyerRationTicker;
    Adafruit_MQTT_Publish adafruit_;
    MyDistributeur *_precedent;

public:
    String name;
    int nbRation;

    explicit MyDistributeur(String name,
                            const int nbRation, const Adafruit_MQTT_Publish &adafruit,
                            MyDistributeur *precedent = nullptr) : adafruit_(adafruit), _precedent(precedent),
                                                                   name(std::move(name)),
                                                                   nbRation(nbRation) {
    }

    MyDistributeur(String name, const int nbRation, const int nbMin, const int nbMax, const float nbBySecSend,
                   const int nbSendRation,
                   const int copulation,
                   const float copulationSec, const int eat, const Adafruit_MQTT_Publish &adafruit,
                   MyDistributeur *precedent = nullptr)
        : _nbMin(nbMin),
          _nbMax(nbMax),
          _copulation(copulation),
          _copulationSec(copulationSec),
          _nbBySecSend(nbBySecSend),
          _nbSendRation(nbSendRation),
          _eat(eat),
          adafruit_(adafruit),
          _precedent(precedent),
          name(std::move(name)) {
        if (nbRation < _nbMin) {
            throw std::invalid_argument("Le nombre de rations ne peut pas être inférieur au minimum requis");
        }
        this->nbRation = nbRation;
    }

    void commande(const int nombre) {
        MYDEBUG_PRINTLN("=== Commande " + name + " ===");
        MYDEBUG_PRINTLN(("Demande de " + std::to_string(nombre) + " rations").data());
        MYDEBUG_PRINTLN(("État actuel : " + std::to_string(nbRation) + " rations disponibles").data());

        if (nombre < 0) {
            throw std::invalid_argument("La commande ne peut pas être de " + std::to_string(nombre));
        }
        if (nbRation - nombre >= _nbMin) {
            MYDEBUG_PRINTLN("Commande acceptée - Envoi progressif démarré");
            nombreRestant = nombre;
            auto pNombreRestant = &nombreRestant;

            envoyerRationTicker.attach(_nbBySecSend, [this, pNombreRestant]() {
                if (*pNombreRestant > 0) {
                    const int envoi = std::min(this->_nbSendRation, *pNombreRestant);
                    this->nbRation -= envoi;
                    *pNombreRestant -= envoi;

                    // Publier le nombre de poissons prêts
                    pubReady.publish(envoi);
                    // Mettre à jour le nombre de rations restantes
                    this->adafruit_.publish(this->nbRation);
                    pubCommande.publish(std::max(*pNombreRestant - envoi, 0));

                    if (*pNombreRestant == 0) {
                        this->envoyerRationTicker.detach();
                    }
                }
            });
        } else if (_precedent != nullptr) {
            // Tenter la copulation en cascade jusqu'à avoir assez de rations
            MyDistributeur *distributeurCourant = _precedent;
            bool isCopul = false;

            while (distributeurCourant != nullptr && !isCopul) {
                isCopul = distributeurCourant->copulation();
                if (!isCopul) {
                    distributeurCourant = distributeurCourant->getPrecedent();
                }
            }

            if (isCopul) {
                // Réessayer la commande si une copulation a réussi
                commande(nombre);
            } else {
                MYDEBUG_PRINT("Aucun distributeur n'a pu effectuer la copulation !");
            }
        } else {
            MYDEBUG_PRINT("Le distributeur " + name + " n'a plus assez de rations !\nVeuillez le remplir !");
        }
    }

    bool copulation() {
        if (!ensureConnected()) {
            return false;
        }

        bool success = false;

        // Vérifications de base
        if (nbRation < _nbMax && nbRation >= 0 && _precedent) {
            // Protection contre les débordements
            if (_precedent->nbRation >= _eat + _precedent->_nbMin &&
                nbRation + _copulation <= _nbMax) {
                MYDEBUG_PRINTLN("=== Tentative de copulation " + name + " ===");
                MYDEBUG_PRINTLN(("État actuel : " + std::to_string(nbRation) + "/" + std::to_string(_nbMax)).data());

                // Sauvegarde des valeurs
                const int oldPrecedentRation = _precedent->nbRation;
                const int oldRation = nbRation;

                // Application des modifications
                _precedent->nbRation -= _eat;
                nbRation += _copulation;

                // Publication des modifications
                if (ensureConnected() &&
                    _precedent->adafruit_.publish(_precedent->nbRation) &&
                    ensureConnected() &&
                    adafruit_.publish(nbRation)) {
                    success = true;
                } else {
                    // Restauration en cas d'échec
                    _precedent->nbRation = oldPrecedentRation;
                    nbRation = oldRation;
                }
            }
        }
        if (success) {
            MYDEBUG_PRINTLN(("Copulation réussie : " + std::to_string(nbRation) + " rations après opération").data());
        } else {
            MYDEBUG_PRINTLN("Échec de la copulation - Conditions non remplies");
        }

        return success;
    }

    void setRation(const int ration) {
        this->nbRation = ration;
    }

    void setPrecedent(MyDistributeur *precedent) {
        this->_precedent = precedent;
    }

    [[nodiscard]] float getCopulationSec() const {
        return this->_copulationSec;
    }

    [[nodiscard]] MyDistributeur *getPrecedent() const {
        return this->_precedent;
    }

    friend std::ostream &operator<<(std::ostream &os, const MyDistributeur &distributeur) {
        os << "Distributeur " << distributeur.name.c_str() << " : "
                << distributeur.nbRation << "/" << distributeur._nbMax
                << " rations (min: " << distributeur._nbMin << ")";
        return os;
    }
};


inline auto croquette = MyDistributeur("Croquette", 30000, 1000, 50000, 10, 1000, 0, 0, 0, pubNbRationCroquette);
inline auto poissonRouge = MyDistributeur("Poisson Rouge", 30, 20, 100, 10, 3, 4, 30, 2000, pubNbRationPoissonRouge,
                                          &croquette);
inline auto achigan = MyDistributeur("Achigan", 10, 5, 20, 10, 1, 1, 15, 2, pubNbRationAchigan, &poissonRouge);
inline auto achiganResto = MyDistributeur("Achigan du Restaurant", 12, 5, 30, 20, 1, 1, 15, 1, pubNbRationResto,
                                          &achigan);


inline void setupDistributeur() {
    setupMQTT();

    // Vérification de la mémoire
    if (EspClass::getFreeHeap() < 4096) {
        MYDEBUG_PRINTLN("Mémoire insuffisante lors de l'initialisation");
        delay(1000);
        EspClass::restart();
    }

    // Vérification du WiFi
    if (WiFi.status() != WL_CONNECTED) {
        setupWiFi();
        delay(2000);
    }

    // Tentative de connexion initiale à Adafruit IO
    MYDEBUG_PRINTLN("Tentative de connexion initiale à Adafruit IO...");
    uint8_t retries = 5;
    bool connected = false;

    while (retries-- && !connected) {
        connectAdafruitIO();
        if (MyAdafruitMqtt.connected()) {
            connected = true;
            MYDEBUG_PRINTLN("Connexion initiale réussie !");
            break;
        }
        delay(3000);
    }

    if (!connected) {
        MYDEBUG_PRINTLN("Échec de la connexion initiale à Adafruit IO");
    }


    // Configuration des callbacks et souscription aux FEEDs
    subNbRationCroquette.setCallback([](char *data, uint16_t len) {
        croquette.setRation(atoi(data));
    });
    MyAdafruitMqtt.subscribe(&subNbRationCroquette);

    subNbRationPoissonRouge.setCallback([](char *data, uint16_t len) {
        poissonRouge.setRation(atoi(data));
    });
    MyAdafruitMqtt.subscribe(&subNbRationPoissonRouge);

    subNbRationAchigan.setCallback([](char *data, uint16_t len) {
        achigan.setRation(atoi(data));
    });
    MyAdafruitMqtt.subscribe(&subNbRationAchigan);

    subNbRationResto.setCallback([](char *data, uint16_t len) {
        achiganResto.setRation(atoi(data));
    });
    MyAdafruitMqtt.subscribe(&subNbRationResto);

    subCommande.setCallback([](char *data, uint16_t len) {
        achiganResto.commande(atoi(data));
    });
    MyAdafruitMqtt.subscribe(&subCommande);

    MyAdafruitMqtt.processPackets(2000);

    if (croquette.getCopulationSec() > 0) {
        croquetteTicker.attach(croquette.getCopulationSec(), []() {
            croquette.copulation();
        });
    }

    if (poissonRouge.getCopulationSec() > 0) {
        poissonRougeTicker.attach(poissonRouge.getCopulationSec(), []() {
            poissonRouge.copulation();
        });
    }

    if (achigan.getCopulationSec() > 0) {
        achiganTicker.attach(achigan.getCopulationSec(), []() {
            achigan.copulation();
        });
    }

    if (achiganResto.getCopulationSec() > 0) {
        achiganRestoTicker.attach(achiganResto.getCopulationSec(), []() {
            achiganResto.copulation();
        });
    }
}

inline void loopDistributeur() {
    static unsigned long lastProcess = 0;
    static unsigned long lastPing = 0;

    const unsigned long now = millis();

    // Gestion du ping
    if (constexpr unsigned long pingInterval = 5000; now - lastPing >= pingInterval) {
        lastPing = now;

        if (MyAdafruitMqtt.connected()) {
            // Tentative de ping
            if (!MyAdafruitMqtt.ping()) {
                MYDEBUG_PRINTLN("Ping échoué, tentative de reconnexion...");
                MyAdafruitMqtt.disconnect();
                delay(1000);
                connectAdafruitIO();
                return;
            }
        } else {
            connectAdafruitIO();
            return;
        }
    }

    // Traitement périodique des paquets
    if (constexpr unsigned long processInterval = 1000; now - lastProcess >= processInterval) {
        lastProcess = now;

        if (MyAdafruitMqtt.connected()) {
            MyAdafruitMqtt.processPackets(100);
        }
    }
}


#endif
