// Distributeur.h
#ifndef DISTRIBUTEUR_H
#define DISTRIBUTEUR_H

#include <ESP8266WiFi.h>
#include <iostream>
#include <utility>
#include <Ticker.h>
#include "MyDebug.h"
#include "Adafruit_MQTT_Client.h"
#include "MyMQTT.h"


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
        MYDEBUG_PRINTLN("========== Commande " + name + " ==============");
        MYDEBUG_PRINTLN("Demande de " + String(nombre) + " rations");
        MYDEBUG_PRINTLN("État actuel : " + String(nbRation) + " rations disponibles");


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
                    MYDEBUG_PRINTLN("=== Progression de l'envoi ===");
                    MYDEBUG_PRINTLN("Rations envoyées : " + String(envoi));
                    MYDEBUG_PRINTLN("Restant à envoyer : " + String(*pNombreRestant));
                    MYDEBUG_PRINTLN("Rations restantes dans " + name + " : " + String(nbRation));

                    const int ready = getReadyCount();
                    // Publier le nombre de poissons prêts
                    pubReady.publish(ready + envoi);
                    // Mettre à jour le nombre de rations restantes
                    this->adafruit_.publish(this->nbRation);
                    pubCommande.publish(std::max(*pNombreRestant, 0));

                    if (*pNombreRestant <= 0) {
                        MYDEBUG_PRINTLN("=== Envoi terminé ===");

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
            } else {
                MYDEBUG_PRINTLN("==========IL N'Y A PLUS ASSEZ DE " + _precedent->name + " =============");
            }
        }
        if (success) {
            MYDEBUG_PRINT(("Copulation réussie : " + std::to_string(nbRation)).data());
            MYDEBUG_PRINTLN(" " + name + " après opération");
        } else {
            MYDEBUG_PRINTLN("Échec de la copulation - Conditions non remplies");
        }

        return success;
    }

    void setRation(const int ration) { this->nbRation = ration; }
    void setPrecedent(MyDistributeur *precedent) { this->_precedent = precedent; }
    void setNbMin(const int nbMin) { _nbMin = nbMin; }
    void setNbMax(const int nbMax) { _nbMax = nbMax; }
    void setCopulation(const int copulation) { _copulation = copulation; }
    void setCopulationSec(const float copulationSec) { _copulationSec = copulationSec; }
    void setNbBySecSend(const float nbBySecSend) { _nbBySecSend = nbBySecSend; }
    void setNbSendRation(const int nbSendRation) { _nbSendRation = nbSendRation; }
    void setEat(const int eat) { _eat = eat; }
    void setName(const String &newName) { name = newName; }

    [[nodiscard]] float getCopulationSec() const { return this->_copulationSec; }
    [[nodiscard]] MyDistributeur *getPrecedent() const { return this->_precedent; }
};


inline auto croquette = MyDistributeur("Croquette", 30000, pubNbRationCroquette);
inline auto poissonRouge = MyDistributeur("Poisson Rouge", 30, pubNbRationPoissonRouge, &croquette);
inline auto achigan = MyDistributeur("Achigan", 10, pubNbRationAchigan, &poissonRouge);
inline auto achiganResto = MyDistributeur("Achigan du Restaurant", 12, pubNbRationResto, &achigan);

inline void loadDistributeurConfig() {
    if (SPIFFS.exists("/config.json")) {
        File configFile = SPIFFS.open("/config.json", "r");
        if (configFile) {
            DynamicJsonDocument doc(2048);
            DeserializationError error = deserializeJson(doc, configFile);

            if (!error) {
                JsonObject distributeurs = doc["distributeurs"];

                // Configuration de Croquette
                if (JsonObject cfgCroquette = distributeurs["croquette"]) {
                    croquette.setName(cfgCroquette["nom"].as<String>());
                    croquette.setRation(cfgCroquette["nbRation"].as<int>());
                    croquette.setNbMin(cfgCroquette["nbMin"].as<int>());
                    croquette.setNbMax(cfgCroquette["nbMax"].as<int>());
                    croquette.setNbBySecSend(cfgCroquette["nbBySecSend"].as<float>());
                    croquette.setNbSendRation(cfgCroquette["nbSendRation"].as<int>());
                    croquette.setCopulation(cfgCroquette["copulation"].as<int>());
                    croquette.setCopulationSec(cfgCroquette["copulationSec"].as<float>());
                    croquette.setEat(cfgCroquette["eat"].as<int>());
                }

                // Configuration de Poisson Rouge
                if (JsonObject cfgPoissonRouge = distributeurs["poissonRouge"]) {
                    poissonRouge.setName(cfgPoissonRouge["nom"].as<String>());
                    poissonRouge.setRation(cfgPoissonRouge["nbRation"].as<int>());
                    poissonRouge.setNbMin(cfgPoissonRouge["nbMin"].as<int>());
                    poissonRouge.setNbMax(cfgPoissonRouge["nbMax"].as<int>());
                    poissonRouge.setNbBySecSend(cfgPoissonRouge["nbBySecSend"].as<float>());
                    poissonRouge.setNbSendRation(cfgPoissonRouge["nbSendRation"].as<int>());
                    poissonRouge.setCopulation(cfgPoissonRouge["copulation"].as<int>());
                    poissonRouge.setCopulationSec(cfgPoissonRouge["copulationSec"].as<float>());
                    poissonRouge.setEat(cfgPoissonRouge["eat"].as<int>());
                }

                // Configuration d'Achigan
                if (JsonObject cfgAchigan = distributeurs["achigan"]) {
                    achigan.setName(cfgAchigan["nom"].as<String>());
                    achigan.setRation(cfgAchigan["nbRation"].as<int>());
                    achigan.setNbMin(cfgAchigan["nbMin"].as<int>());
                    achigan.setNbMax(cfgAchigan["nbMax"].as<int>());
                    achigan.setNbBySecSend(cfgAchigan["nbBySecSend"].as<float>());
                    achigan.setNbSendRation(cfgAchigan["nbSendRation"].as<int>());
                    achigan.setCopulation(cfgAchigan["copulation"].as<int>());
                    achigan.setCopulationSec(cfgAchigan["copulationSec"].as<float>());
                    achigan.setEat(cfgAchigan["eat"].as<int>());
                }

                // Configuration d'Achigan Restaurant
                if (JsonObject cfgAchiganResto = distributeurs["achiganResto"]) {
                    achiganResto.setName(cfgAchiganResto["nom"].as<String>());
                    achiganResto.setRation(cfgAchiganResto["nbRation"].as<int>());
                    achiganResto.setNbMin(cfgAchiganResto["nbMin"].as<int>());
                    achiganResto.setNbMax(cfgAchiganResto["nbMax"].as<int>());
                    achiganResto.setNbBySecSend(cfgAchiganResto["nbBySecSend"].as<float>());
                    achiganResto.setNbSendRation(cfgAchiganResto["nbSendRation"].as<int>());
                    achiganResto.setCopulation(cfgAchiganResto["copulation"].as<int>());
                    achiganResto.setCopulationSec(cfgAchiganResto["copulationSec"].as<float>());
                    achiganResto.setEat(cfgAchiganResto["eat"].as<int>());
                }

                MYDEBUG_PRINTLN("Configuration des distributeurs chargée avec succès");
            } else {
                MYDEBUG_PRINTLN("Erreur lors du parsing du fichier de configuration");
            }
            configFile.close();
        }
    } else {
        MYDEBUG_PRINTLN("Fichier de configuration des distributeurs non trouvé");
    }
}

inline void setupDistributeur() {
    setupMQTT();

    loadDistributeurConfig();
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
    delay(2000);

    subCommande.setCallback([](char *data, uint16_t len) {
        MYDEBUG_PRINTLN("Commande reçue : " + String(data));
        achiganResto.commande(atoi(data));
        MYDEBUG_PRINTLN("Commande traitée");
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
    static unsigned long lastDebug = 0;
    if (now - lastDebug >= 5000) {
        // Toutes les 5 secondes
        lastDebug = now;
        MYDEBUG_PRINTLN("Status MQTT : " + String(MyAdafruitMqtt.connected() ? "Connecté" : "Déconnecté"));
    }

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
