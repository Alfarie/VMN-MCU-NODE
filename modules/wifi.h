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

        //LcdHandler::instance()->setPage(0);
        Serial.println();
        Serial.println("Start WiFi on Core: " + String(xPortGetCoreID()));
        Serial.print("Connecting to ");
        Serial.println(ssid);

        WiFi.begin(ssid, password);
        page.page1.line1 = "WiFi Init...";
        page.page1.line2 = "Connecting...";
        //LcdHandler::instance()->setLine1("WiFi Init...");
        //LcdHandler::instance()->setLine2("Connecting...");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        //LcdHandler::instance()->setLine2("Connected");
        page.page1.line2 = "Connected....";
        delay(1000);
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        
        page.LCD_PAGE = 1;
        //LcdHandler::instance()->setPage(1);
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