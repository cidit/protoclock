#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <TimeLib.h>
#include <ProtoTGP.h>

ProtoTGP proto;

long start_time, time_it_took;

void connect_to_wifi(const char *ssid, const char *password = "")
{
  Serial.print("Essai de connection avec ssid: " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(50);
  }
  Serial.println();
  Serial.println("Success.");
}

/**
 * Envoie une requète GET à l'endpoint passé en parramètre.
 * Nécéssite un que le wifi soit connecté à un réseau préalablement.
 * Le dernier paramètre permet de configurer le nombre de millisecondes alloué
 * à la requête. Lui donner la valeur 0 permet de déactiver cette fonctionnalitée.
 */
String make_request(WiFiClient client, const char *host, const char *tail, int TIMEOUT = 5000)
{
  auto timer_start = millis();
  Serial.println("Début de la requête.");
  Serial.print("Connection au serveur");
  while (!client.connect(host, 80))
  {
    auto now = millis();
    if (now % 1000 == 0)
    {
      Serial.print(".");
    }
    // Si on dépasse le temps qui nous est alloué, on retourne de la fonction.
    if (TIMEOUT > 0 && timer_start + TIMEOUT > now)
    {
      Serial.print("timeout");
      return "";
    }
  }
  Serial.println();

  client.printf("GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", tail, host);

  Serial.print("En attente de la requête.");
  while (!client.available())
  {
    auto now = millis();
    if (now % 1000 == 0)
    {
      Serial.print(".");
    }
    // Si on dépasse le temps qui nous est alloué, on retourne de la fonction.
    if (TIMEOUT > 0 && timer_start + TIMEOUT > now)
    {
      Serial.println("timeout");
      return "";
    }
  }
  Serial.println();
  // dès que le contenu de la requête est disponible, le retourner.
  return client.readString();
}

/**
 * Transforme un mois sous forme de chiffre tel que TimeLib nous le
 * donne en son équivalent en français pour permettre son affichage.
 */
String month_int_to_str(const int month)
{
  switch (month)
  {
  case 1:
    return "Janvier";
  case 2:
    return "Février";
  case 3:
    return "Mars";
  case 4:
    return "Avril";
  case 5:
    return "Mai";
  case 6:
    return "Juin";
  case 7:
    return "Juillet";
  case 8:
    return "Aout";
  case 9:
    return "Septembre";
  case 10:
    return "Octobre";
  case 11:
    return "Novembre";
  case 12:
    return "Décembre";
  default:
    return "Invalide";
  }
}

void setup()
{
  const char *SSID = "CAL-Techno";
  const char *SUPER_SECRET_PASS = "technophys123";
  const char *HOST = "worldtimeapi.org";
  const char *TAIL = "/api/timezone/America/Toronto";

  Serial.begin(115200);
  proto.begin();

  connect_to_wifi(SSID, SUPER_SECRET_PASS);

  WiFiClient client;
  // Je met 0 au timeout parce que je ne veut pas de timeout.
  // Je veux qu'il attende pour toujour sa reponse.
  auto response = make_request(client, HOST, TAIL, 0);

  // On tient en compte le temps que la réponse à prit pour
  // s'assurer que on utilise le temps le plus proche possible que celui
  // qu'on reçoit.
  time_it_took = millis();

  Serial.println("\nresponse:\n" + response);
  Serial.println("\nend response");

  // Aller chercher le corps de la réponse, le transformer en objet json...
  auto payload = response.substring(response.indexOf('{'));
  JsonDocument doc;
  deserializeJson(doc, payload);

  // L'offset est important à cause du fuseaux horaire.
  long unixtime = doc["unixtime"];
  long offset = doc["raw_offset"];
  start_time = unixtime + offset;
}

long calc_unix_now(long unix_start_time, long offset) {
  const long ONE_SECOND = 1000; // ms
  auto seconds_since_start = (millis() - time_it_took) / ONE_SECOND;
  return seconds_since_start;
}

/**
 * Construit un String bien formatté qui affiche:
 * - la date, en français
 * - l'heure, sous forme de phrase
 * - l'heure, sous forme de cadran
 * et retourne le String.
*/
const String formatted_display_output(const long now) {
  // Crée un buffer assez large pour contenir la string après formattage. 
  // (49 charactères de capacités + un charactère de fin \0)
  // un buffer de charactères est nécessaire parce que la fonction snprintf ne
  // prend pas de String en input.
  char buffer[100];
  snprintf(buffer,
           sizeof buffer,
           "%d %s %d à\n %d heures,\n %d minutes et\n %d secondes\n[%02d:%02d:%02d]",
           day(now),
           month_int_to_str(month(now)),
           year(now),
           hour(now),
           minute(now),
           second(now),
           hour(now),
           minute(now),
           second(now));

  return String(buffer);
}

void loop()
{
  proto.refresh();
  auto now = calc_unix_now(start_time, time_it_took);
  auto to_print = formatted_display_output(now);
  Serial.println(to_print);
  proto.ecran.ecrire(to_print);
}