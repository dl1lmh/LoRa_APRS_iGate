#include <ETH.h>

#include "configuration.h"
#include "display.h"

extern Configuration    Config;
extern bool             backUpDigiMode;
extern bool             backUpDigiModeEth;
extern uint32_t         lastBackupDigiTime;

bool EthLink            = false;
bool EthGotIP           = false;
bool EthConnected       = false;


namespace ETH_Utils {

    void EthEvent(WiFiEvent_t event) {
        switch (event) {
            case ARDUINO_EVENT_ETH_START:
                break;
                
            case ARDUINO_EVENT_ETH_CONNECTED:
                EthLink = true;
                break;

            case ARDUINO_EVENT_ETH_GOT_IP:
                EthGotIP = true;
                break;

            case ARDUINO_EVENT_ETH_DISCONNECTED:
                EthLink = false;
                EthGotIP = false;
                EthConnected = false;
                break;

            case ARDUINO_EVENT_ETH_STOP:
                EthLink = false;
                EthGotIP = false;
                EthConnected = false;
                break;

            default:
                break;
        }

    }

    void checkETH() {
        if (!Config.digi.ecoMode) {
            if (backUpDigiMode) {
                if (EthConnected) {
                    Serial.println("LAN Reconnected...");
                    backUpDigiModeEth = false;
                }
            } else {
                if (!EthConnected) {
                    Serial.println("Lost LAN Connection!");
                    backUpDigiModeEth = true;
                }
            }
        }
    }

    void startETH() {
        unsigned long start = millis();

        String hostname = "iGate-" + Config.callsign;
        ETH.setHostname(hostname.c_str());
        WiFi.onEvent(ETH_Utils::EthEvent);
        displayShow("", "Connecting to LAN:", "", "...", 0);
        Serial.print("Connecting to LAN: ");
        ETH.begin();
        while (((!EthConnected) && (!EthGotIP)) || ((millis() - start) > 10000))
        {
            delay(500);
            #ifdef INTERNAL_LED_PIN
                digitalWrite(INTERNAL_LED_PIN,HIGH);
            #endif
            Serial.print('.');
            delay(500);
            #ifdef INTERNAL_LED_PIN
                digitalWrite(INTERNAL_LED_PIN,LOW);
            #endif
        }
        if ((millis() - start) > 10000) {
            displayShow("", "LAN TIMEOUT!", "", "...", 0);
            Serial.println("\nLAN Timeout!");
            EthConnected = false;
            delay(5000);
        }
        if (EthConnected && EthGotIP) EthConnected = true;
        #ifdef INTERNAL_LED_PIN
                digitalWrite(INTERNAL_LED_PIN,LOW);
        #endif
        if (EthConnected) {
            Serial.print("\nConnected as ");
            Serial.print(ETH.localIP());
            Serial.print(" / MAC: ");
            Serial.println(ETH.macAddress());
            displayShow("", " LAN connected!!", "" , "     loading ...", 1000);
        } else {
            Serial.println("\nNot connected to LAN!");
            displayShow("", " LAN not connected!", "" , "     loading ...", 1000);
        }        
    }

    void setup() {
        if (Config.ethernet.ethernet_enable) startETH();
    }
}