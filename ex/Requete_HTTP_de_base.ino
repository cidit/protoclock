/*  --- Entête principale -- information sur le programme
 *   
 *  Programme:        Requête HTTP de base

 *  Date:             Hiver 2024
 *  Auteur:          Claude Bouchard


 *  Plateforme matérielle: ESP32 DOIT KIT V1 - protoTPhys

 *  Plateforme de développement : Arduino 2.2.1
 *  Description:      Ce programme se connecte au wifi et effectue une requête HTTP
                      à l'URL demandé.
 *  Fonctionnalités:  - Se connecter au wifi
 *                    - Afficher l'adresse MAC
 *                    - Envoyer une requête HTTP
 *                    - Afficher le contenu de la requête HTTP
 */

void setup()
{

  Serial.begin(115200);

  // La prochaine ligne affiche l'adresse MAC
  afficherMACAddress();
  
  // La prochaine ligne connecte l'appareil au Wifi
  connecterAuWifi("CAL-Techno","technophys123");
  
  // La prochaine ligne effectue une requête HTTP
  String contenu = getURL("perdu.com", "/");
  
  Serial.println(contenu);

}

void loop()
{
  
}
