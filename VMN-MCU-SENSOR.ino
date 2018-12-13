#include <Task.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x3d, 16, 2);

#include "./modules/eeprom.h"

#include "./modules/VmnSensors/water.h"
#include "./modules/VmnSensors/ec.h"
#include "./modules/VmnSensors/LoadCell.h"

const char *ssid = "vmnnetwork";
const char *password = "raspberry";
const char *host = "192.168.4.1";

int station = 1;
String CNT = "CNT";
String RST = "P";
#include "./modules/lcd.h"

#include "./modules/wifi.h"

TaskManager taskManager;


void coreTask(void *pvParameters)
{
    EEPROM_Manager::InitEEPROM();
    String taskMessage = "Task running on core ";
    taskMessage = taskMessage + xPortGetCoreID();
    VmnClient::instance();

    //LcdHandler::instance()->setLine("WiFi Init...");
    delay(2000);
    //LcdHandler::instance()->setLine("Initialized: " + String(station));

    while (true)
    {
        delay(3000);
        Serial.print("connecting to ");
        Serial.println(host);

        // Use WiFiClient class to create TCP connections
        WiFiClient client;
        const int httpPort = 80;
       
        //LcdHandler::instance()->setLine("WiFi:CNT STA:" + String(station) + " P");

        if (WiFi.status() != WL_CONNECTED){
            CNT = "DCT";
        }
        else{
            CNT = "CNT";
        }

        RST = "P";
        //connecting time ~ 4-5 ms
        if (!client.connect(host, httpPort))
        {
           //LcdHandler::instance()->setLine("WiFi:ERR STA:" + String(station) + " E");
            RST = "E";
            Serial.println("connection failed");
            continue;
        }
        
       //LcdHandler::instance()->setLine("WiFi:CNT STA:" + String(station) + " S");
         RST = "S";

        // We now create a URI for the request
        float ec = ECSensor::instance()->GetEC();
        float vol = LoadCellSensor::instance()->getVal();
        String url = "{" + String(station) + "," + String(ec) + "," + String(vol) + "}";
        Serial.print("Requesting URL: ");
        Serial.println(url);

        client.print(url);

        unsigned long timeout = millis();
        while (client.available() == 0)
        {
            if (millis() - timeout > 3000)
            {
                 RST = "T";
                Serial.println(">>> Client Timeout !");
                client.stop();
                break;
            }
        }

        // Read all the lines of the reply from server and print them to Serial
        while (client.available())
        {
            String line = client.readStringUntil('\r');
            Serial.print(line);
        }

        Serial.println();
        Serial.println("closing connection");
    }
}
void setup()
{
    taskManager.StartTask(LcdHandler::instance());
    Serial.begin(115200);
    taskManager.StartTask(WaterSensor::instance());
    taskManager.StartTask(ECSensor::instance());
    taskManager.StartTask(LoadCellSensor::instance());
    

    xTaskCreatePinnedToCore(
        coreTask,   /* Function to implement the task */
        "coreTask", /* Name of the task */
        10000,      /* Stack size in words */
        NULL,       /* Task input parameter */
        0,          /* Priority of the task */
        NULL,       /* Task handle. */
        0);
}
void loop()
{
    taskManager.Loop();
}
