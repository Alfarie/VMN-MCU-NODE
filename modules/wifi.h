#include <Task.h>
#include <WiFi.h>
#include <HTTPClient.h>

// const char *ssid = "ABDULKARIM";
// const char *password = "0810980364";

const char *ssid = "vmnnetwork";
const char *password = "raspberry";

extern TaskManager taskManager;

class VmnClient : public Task
{
  public:
    static VmnClient *s_instance;
    VmnClient() : Task(MsToTaskTime(500))
    {
        // We start by connecting to a WiFi network

        Serial.println();
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);
        
        lcd.setCursor(0,0);
        lcd.print("Connecting...");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }

        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());

        lcd.setCursor(0,0);
        lcd.print("WiFi Connected");
    }

    static VmnClient *instance()
    {
        if (!s_instance)
            s_instance = new VmnClient;
        return s_instance;
    }

  private:
    float currentTime = 0;
    int state = 0;
    virtual bool OnStart() { return true; }
    virtual void OnUpdate(uint32_t delta_time)
    {

        requestData(delta_time);
    }

    void requestData(uint32_t delta_time)
    {

        if ((WiFi.status() == WL_CONNECTED))
        { //Check the current connection status

            float ec = ECSensor::instance()->GetEC();
            float vol = LoadCell::instance()->getVal();

            Serial.println(ec);
            Serial.println(station);
            Serial.println(vol);
            // String url = "http://192.168.4.1/vmndata";
            String url = "http://192.168.4.1/vmndata?" + String(station) + "," + String(ec) + "," + String(vol);
            // String url = "http://192.168.4.1/vmndata?st=" + String(station) + "&ec=" + String(ec) + "&vol=" + String(vol);
            Serial.println(url);
            HTTPClient http;

            http.begin(url); //Specify the URL
            int httpCode = http.GET();                                        //Make the request

            if (httpCode > 0)
            { //Check for the returning code
                String payload = http.getString();
                Serial.println(httpCode);
                Serial.println(payload);
            }

            else
            {
                lcd.setCursor(0,1);
                lcd.print("Error HTTP");
                Serial.println("Error on HTTP request");
            }

            http.end(); //Free the resources
        }

        /*Serial.print("connecting to ");
        Serial.println(host);

        WiFiClient client;
        const int httpPort = 3000;
        if (!client.connect(host, httpPort))
        {
            Serial.println("connection failed");
            return;
        }
        float ec = ECSensor::instance()->GetEC();
        float vol = LoadCell::instance()->getVal();
        String url = "/vmndata?" + String(STA) + "," + String(ec) + "," + String(vol);

        // This will send the request to the server
        if (state == 0)
        {
            currentTime += delta_time;
            if (currentTime > 1000)
            {
                Serial.print("Requesting URL: ");
                Serial.println(url);
                client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                             "Host: " + host + "\r\n" +
                             "Connection: close\r\n\r\n");
                state = 1;
                currentTime = 0;
            }
        }
        else if (state == 1)
        {
            currentTime += delta_time;
            if (currentTime > 1000)
            {
                 Serial.print("close connection");
                state = 0;
                currentTime = 0;
                client.stop();
            }
        }
        // Read all the lines of the reply from server and print them to Serial*/
    }
};
VmnClient *VmnClient::s_instance = 0;