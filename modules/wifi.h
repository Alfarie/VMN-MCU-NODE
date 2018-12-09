#include <Task.h>
#include <WiFi.h>
#include <HTTPClient.h>

// const char *ssid = "ABDULKARIM";
// const char *password = "0810980364";


WiFiClient client;
extern TaskManager taskManager;

class VmnClient : public Task
{
  public:
    static VmnClient *s_instance;
    VmnClient() : Task(MsToTaskTime(2000))
    {
        // We start by connecting to a WiFi network

        Serial.println();
        Serial.println("Start WiFi on Core: " + String(xPortGetCoreID()));
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);

        lcd.setCursor(0, 0);
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

        lcd.setCursor(0, 0);
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

    }
};
VmnClient *VmnClient::s_instance = 0;