#include <WiFi.h>

/*
 * Afficher la MAC Address de la carte ESP32 via le moniteur série.
 * 
 *  Entrées : aucune
 *  Sortie : aucune
 */
void afficherMACAddress() {
  Serial.print("\nMAC Address: ");
  Serial.println(WiFi.macAddress());
}

/*
 *  Se connecte au réseau Wifi.
 *  
 *  Entrées : 
 *  - SSID, le nom du réseau Wifi
 *  - password, le mot de passe du réseau Wifi    
 *  Sortie : aucune
 */
void connecterAuWifi(const char* ssid, const char* password) {

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) { // Attendre que la connexion au Wifi s'effectue
    Serial.print('.');
    delay(500);
  }
  delay(100);
  Serial.print("\nWifi connecté.");
  Serial.print("\nAdresse IP locale: ");
  Serial.println(WiFi.localIP());
}

/*
 *  Structure et effectue une requête HTML de type GET.
 *  
 *  Entrées : 
 *  - serveur, le nom du serveur pour la requête (exemple : "perdu.com")
 *  - url, l'adresse de la page sur le serveur (exemple : "/" pour la page d'accueil);  
 *  Sortie : Une variable de type String qui contient la réponse demandée. La chaîne de texte sera vide si la requête échoue.
 */
String getURL(const char* serveur, const char* url) {

  //Connexion au serveur
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(serveur, httpPort)) {
    return "";
  }

  //Formatage et envoi de la requête
  String request = String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + serveur + "\r\n" +
                   "Connection: close\r\n\r\n";
  client.print(request);

  //Attente de la réponse
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      client.stop();
      return "";
    }
  }

  //Lecture de la réponse
  if (client.available()) {
    return client.readString();
  }
}
