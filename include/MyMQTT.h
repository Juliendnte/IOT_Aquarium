/**
 * \file MyMQTT.h
 * \page mqtt MQTT
 * 
 * Le protocole Message Queue Telemetry Transport (MQTT) est conçu pour l'échange de messages entre des petits 
 * périphériques sur une faible bande passante, ou pour les connexions coûteuses et envoyer des messages 
 * de façon fiable. Il utilise TCP/IP. http://mqtt.org/
 * MQTT est un protocole ouvert, simple, léger et facile à mettre en œuvre. Ce protocole est idéal pour
 * répondre aux besoins suivants :
 * - Utilisation d’une très faible bande passante,
 * - Utilisation sur les réseaux sans fils,
 * - Faible consommation en énergie,
 * - Rapidité avec un temps de réponse supérieur aux autres standards du web actuel,
 * - Fiabilité,
 * - Utilisation de faibles ressources processeurs et de mémoire.
 * 
 * <H2>Publish/Subscribe</H2>
 * 
 * MQTT est un service de publication/abonnement en contraste avec le protocole HTTP et son architecture 
 * « request/response ». MQTT se base sur TCP/IP et est extrêmement léger. Il fonctionne 
 * sur le principe client/serveur.
 * 
 * <H2>MQTT Broker</H2>
 * 
 * Le point central de la communication est le broker MQTT en charge de relayer les messages des émetteurs
 * vers les clients. Chaque client s’abonne via un message vers le broker : le « topic » (sorte d’information
 * de routage pour le broker) qui permettra au broker de réémettre les messages reçus des producteurs
 * de données vers les clients. Les clients et les producteurs n’ont ainsi pas à se connaître, ne communiquant
 * qu’au travers des topics. Cette architecture permet des solutions multi-échelles.
 * 
 * Le principe d’échange est très proche de celui de Twitter. Les messages sont envoyés par les publishers 
 * sur un canal appelé topic. Ces messages peuvent être lus par les subscribers (abonnés). Les topics 
 * (ou canaux d’informations) peuvent avoir une hiérarchie qui permet de sélectionner finement les 
 * informations que l’on désire.
 * 
 * Chaque client MQTT a une connexion ouverte en permanence avec le broker. Si la connexion s’arrête, le
 * broker bufférise les messages et les émet dès que la reconnexion avec le client est effectuée.
 * 
 * Voici quelques brokers :
 * - Eclipse Mosquitto 
 * - CloudMQTT 
 * - Thingsboard
 * - ThingMQ  
 * - ThingStudio 
 * - CloudMQTT 
 * - IBM Bluemix 
 * - Heroku 
 * - Hivemq 
 * - Microsoft Azure IoT 
 * - MaQiaTTo
 * - AWS IoT Core
 * 
 * <H2>Topics</H2>
 * 
 * Un « topic MQTT » est une chaîne de caractères qui peut posséder une hiérarchie de niveaux séparés
 * par le caractère « / ». Par exemple, une information de température du salon pourrait être envoyée sur
 * le topic « maison/salon/temperature » et la température de la cuisine sur « maison/cuisine/temperature ».
 * Le signe « + » est un caractère « wildcard » qui permet des valeurs arbitraires pour une hiérarchie
 * particulière et le signe « # » pour plus d’un niveau. 
 * 
 * Par exemple le "topic" ‘/home/salon/temperature’ communiquera les températures du salon si un 
 * quelqu'un (object connecté ou application cliente) s’y abonne. La sonde de température présente dans 
 * le salon publiera régulièrement la température relevée sur ce topic.
 * Si un publisher s’abonne au topic ‘/home/salon/#’ il recevra toutes les données du salon (on peut 
 * imaginer : luminosité, humidité, température ...).
 * S’il s’abonne au topic ‘/home/#’, il collectera toutes les données des sondes de la maison.
 * 
 * <H2>Sécurité</H2>
 * 
 * MQTT permet de sécuriser les échanges à plusieurs niveaux :
 * - Transport en SSL/TLS,
 * - Authentification par certificats SSL/TLS,
 * - Authentification par login/mot de passe.
 * 
 * <H2>Qualité de Service (QoS)</H2>
 * 
 * MQTT intègre en natif la notion de QoS. En effet le publisher à la possibilité de définir la 
 * qualité de son message. Trois niveaux sont possibles :
 * - Un message de QoS niveau 0 « At most once » sera délivré tout au plus une fois. Ce qui 
 *   signifie que le message est envoyé sans garantie de réception, (le broker n’informe pas 
 *   l’expéditeur qu’il l’a reçu et le message)
 * - Un message de QoS niveau 1 « At least once » sera livré au moins une fois. Le client 
 *   transmettra plusieurs fois s’il le faut jusqu’à ce que le Broker lui confirme qu’il a 
 *   était transmis sur le réseau.
 * - Un message de QoS niveau 2 « exactly once » sera obligatoirement sauvegardé par l’émetteur 
 *   et le transmettra toujours tant que le récepteur ne confirme pas son envoi sur le réseau. 
 *   La principale différence étant que l’émetteur utilise une phase de reconnaissance plus 
 *   sophistiquée avec le broker pour éviter une duplication des messages (plus lent mais plus sûr).
 * 
 * <H2>Bibliothèques MQTT</H2>
 * 
 * Il existe de très nombreuses bibliothèques pour programmer des clients MQTT, et ce 
 * pour la plupart des langages (C, C++, Java, JavaScript, PHP, Python ...) et sur la 
 * plupart des plates-formes (GNU/Linux, Windows, iOS, Android, Arduino ...).
 * - Eclipse Paho : https://www.eclipse.org/paho/
 * - HiveMQ
 * - Eclipse Mosquitto
 * 
 * \note Même si on ne sait pas exactement comment, il semblerait que Facebook utilise 
 * des aspects de MQTT dans Facebook Messenger !
 */
