#include <Task.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#include "./modules/VmnSensors/water.h"
#include "./modules/VmnSensors/ec.h"
#include "./modules/VmnSensors/LoadCell.h"

const char *ssid = "vmnnetwork";
const char *password = "raspberry";
const char *host = "192.168.4.1";
int station = 0;
#include "./modules/wifi.h"

TaskManager taskManager;

unsigned long currentTime = 0;
void setup()
{
    // lcd.init();
    lcd.backlight();
    Serial.begin(115200);
    taskManager.StartTask(WaterSensor::instance());
    taskManager.StartTask(ECSensor::instance());
    taskManager.StartTask(LoadCell::instance());
    // taskManager.StartTask(VmnClient::instance());
    VmnClient::instance();
}
void loop()
{
    taskManager.Loop();
    unsigned long diff = millis() - currentTime;
    if (diff > 3000)
    {
        if ((WiFi.status() == WL_CONNECTED))
        {
            lcd.setCursor(0, 1);
            lcd.print("WIFI ERROR.");
            Serial.println("WIFI ERROR");
        }
        currentTime = millis();
        Serial.print("connecting to ");
        Serial.println(host);

        // Use WiFiClient class to create TCP connections
        WiFiClient client;
        const int httpPort = 80;
        if (!client.connect(host, httpPort))
        {
            lcd.setCursor(0, 1);
            lcd.print("CONNECTION ERROR.");
            Serial.println("connection failed");
            return;
        }

        // We now create a URI for the request
        float ec = ECSensor::instance()->GetEC();
        float vol = LoadCell::instance()->getVal();
        String url = "/vmndata?" + String(station) + "," + String(ec) + "," + String(vol);
        Serial.print("Requesting URL: ");
        Serial.println(url);

        // This will send the request to the server
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: close\r\n\r\n");
        // unsigned long timeout = millis();
        // while (client.available() == 0)
        // {
        //     if (millis() - timeout > 5000)
        //     {
        //         Serial.println(">>> Client Timeout !");
        //         client.stop();
        //         return;
        //     }
        // }

        // // Read all the lines of the reply from server and print them to Serial
        // while (client.available())
        // {
        //     String line = client.readStringUntil('\r');
        //     Serial.print(line);
        // }

        Serial.println();
        Serial.println("closing connection");
    }
}
