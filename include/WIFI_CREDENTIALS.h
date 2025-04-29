#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H

// pour le mode STATION
inline auto station_ssid     = "";       // Nom du réseau WiFi pour accéder à internet en mode Station
inline auto station_password = "";   // Mot de passe du réseau WiFi pour accéder à internet en mode Station
// pour le mode ACCESS POINT
inline auto ap_ssid     = "";      // Nom du réseau WiFi pour accéder à mon serveur web
inline auto ap_password = "";     // Mot de passe du réseau WiFi pour accéder à l'Access Point
inline String sstation_ssid;
inline String sstation_password;

#endif // WIFI_CREDENTIALS_H