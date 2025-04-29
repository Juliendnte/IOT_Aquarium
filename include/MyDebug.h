#ifndef MYDEBUG_H
#define MYDEBUG_H

#include <Arduino.h>

// Buffer circulaire pour les logs
constexpr int LOG_BUFFER_SIZE = 50; // Nombre de lignes à conserver
inline String logBuffer[LOG_BUFFER_SIZE];
inline int logIndex = 0;

// Fonction pour ajouter un log au buffer
inline void addToLogBuffer(const String &message) {
    logBuffer[logIndex] = message;
    logIndex = (logIndex + 1) % LOG_BUFFER_SIZE;
}

#ifdef MYDEBUG
// Fonction spéciale pour une nouvelle ligne sans argument
inline void debugPrintln() {
    Serial.println();
    addToLogBuffer("");
}

// Surcharge pour IPAddress
inline void debugPrint(const IPAddress &ip) {
    String ipStr = String(ip[0]) + "." + String(ip[1]) + "." +
                   String(ip[2]) + "." + String(ip[3]);
    Serial.print(ipStr);
    addToLogBuffer(ipStr);
}

inline void debugPrintln(const IPAddress &ip) {
    String ipStr = String(ip[0]) + "." + String(ip[1]) + "." +
                   String(ip[2]) + "." + String(ip[3]);
    Serial.println(ipStr);
    addToLogBuffer(ipStr);
}

// Surcharge pour les autres types
template<typename T>
void debugPrint(const T &x) {
    Serial.print(x);
    addToLogBuffer(String(x));
}

template<typename T>
void debugPrintln(const T &x) {
    Serial.println(x);
    addToLogBuffer(String(x));
}

#define MYDEBUG_PRINT(x)     debugPrint(x)
#define MYDEBUG_PRINTDEC(x)  { Serial.print(x, DEC); addToLogBuffer(String(x)); }
#define MYDEBUG_PRINTHEX(x)  { Serial.print(x, HEX); addToLogBuffer(String(x, HEX)); }
#define MYDEBUG_PRINTLN(...)   debugPrintln(__VA_ARGS__)
#define MYDEBUG_PRINTF(a,b,c,d,e) { \
    Serial.printf(a,b,c,d,e); \
    char buffer[256]; \
    snprintf(buffer, sizeof(buffer), a,b,c,d,e); \
    addToLogBuffer(String(buffer)); \
    }
#else
#define MYDEBUG_PRINT(x)
#define MYDEBUG_PRINTDEC(x)
#define MYDEBUG_PRINTHEX(x)
#define MYDEBUG_PRINTLN(...)
#define MYDEBUG_PRINTF(x)
#endif

inline void setupDebug() {
#ifdef MYDEBUG
    Serial.begin(115200);
    MYDEBUG_PRINTLN("Ouverture du port série");
#endif
}

#endif
