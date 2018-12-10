#include <Task.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3d, 16, 2);

#include "./modules/VmnSensors/water.h"
#include "./modules/VmnSensors/ec.h"
#include "./modules/VmnSensors/LoadCell.h"

const char *ssid = "vmnnetwork";
const char *password = "raspberry";
const char *host = "192.168.4.1";
int station = 1;
#include "./modules/wifi.h"

TaskManager taskManager;

void coreTask(void *pvParameters)
{

    lcd.init();
    lcd.backlight();

    String taskMessage = "Task running on core ";
    taskMessage = taskMessage + xPortGetCoreID();
    VmnClient::instance();

    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    lcd.clear();
    delay(2000);
    lcd.print("Initialized: " + String(station));

    while (true)
    {
        delay(3000);
        Serial.print("connecting to ");
        Serial.println(host);

        // Use WiFiClient class to create TCP connections
        WiFiClient client;
        const int httpPort = 80;

        //connecting time ~ 4-5 ms
        if (!client.connect(host, httpPort))
        {
            lcd.setCursor(0, 1);
            lcd.print("CONNECTION ERROR.");
            Serial.println("connection failed");
            return;
        }

        lcd.setCursor(0, 0);
        lcd.print("WiFi: CNT STA:" + String(station));

        // We now create a URI for the request
        float ec = ECSensor::instance()->GetEC();
        float vol = LoadCellSensor::instance()->getVal();
        String url = "{" + String(station) + "," + String(ec) + "," + String(vol) + "}";
        Serial.print("Requesting URL: ");
        Serial.println(url);

        lcd.setCursor(0, 1);
        lcd.print("EC:" + String(ec) + " Vol:" + String(vol));

        client.print(url);

        unsigned long timeout = millis();
        while (client.available() == 0)
        {
            if (millis() - timeout > 3000)
            {
                Serial.println(">>> Client Timeout !");
                client.stop();
                return;
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

    lcd.init();
    lcd.backlight();
    Serial.begin(115200);
    taskManager.StartTask(WaterSensor::instance());
    taskManager.StartTask(ECSensor::instance());
    taskManager.StartTask(LoadCellSensor::instance());
    // taskManager.StartTask(VmnClient::instance());

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
    // Serial.println(xPortGetCoreID());
    taskManager.Loop();
}
