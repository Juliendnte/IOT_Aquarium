#ifndef WIFI_CREDENTIALS_H
#define WIFI_CREDENTIALS_H
#include "../lib/env.hpp"
inline EnvLoader env;

// pour le mode STATION
inline String station_ssid = String(env.get("WIFI_STATION_SSID").c_str());
inline String station_password = String(env.get("WIFI_STATION_PASSWORD").c_str());
// pour le mode ACCESS POINT
inline String ap_ssid = String(env.get("WIFI_AP_SSID").c_str());
inline String ap_password = String(env.get("WIFI_AP_PASSWORD").c_str());
inline String sstation_ssid;
inline String sstation_password;

#endif // WIFI_CREDENTIALS_H