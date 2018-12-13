#include <Task.h>
extern TaskManager taskManager;

#define TBT 25

struct Page1
{
    String line1;
    String line2;
};

struct Page2
{
    String wifi_status;
    String requesting_status;
    float ec;
    int vol;
};

struct Page3
{
    String line1;
    String line2;
};

struct Page
{
    int LCD_PAGE;
    Page1 page1;
    Page2 page2;
    Page3 page3;
};

Page page;


class ButtonHandler : public Task
{
  public:
    static ButtonHandler *s_instance;
    ButtonHandler() : Task(MsToTaskTime(50)) {}
    static ButtonHandler *instance()
    {
        if (!s_instance)
            s_instance = new ButtonHandler;
        return s_instance;
    }

  private:
    unsigned long btnTimeout = 0;
    bool btnPressed = false;
    virtual bool OnStart() { return true; }
    virtual void OnUpdate(uint32_t delta_time) {
        int btn = digitalRead(TBT);
        if(btn == LOW){
            btnTimeout += delta_time;
            btnPressed = true;
            if(btnTimeout >= 1000){
                int dotnum = 16 * btnTimeout / 4000;
                String dots = "";
                for(int i = 0 ; i < dotnum ; i++){
                    dots+=">";
                }

                if(page.LCD_PAGE > 1){
                    lcd.setCursor(0,1);
                    lcd.print(dots);
                    if(btnTimeout >= 5000){
                        btnTimeout = 0;
                        lcd.clear();
                        lcd.setCursor(0,1);
                        checkAndRunOnPage();
                        return;
                    }
                }
                
            }
        }
        if(btnPressed && digitalRead(TBT) == HIGH){
            if(btnTimeout <= 1000){
                page.LCD_PAGE = (page.LCD_PAGE >=4)?1:page.LCD_PAGE + 1;
            }
            Serial.println("Button has been pressed for " + String(btnTimeout) + " ms");
            btnPressed = false;
            btnTimeout = 0;
        }
    }

    void checkAndRunOnPage(){
        if (page.LCD_PAGE == 2)
        {
            lcd.setCursor(0, 1);
            lcd.print("EC 1.4->Success!");
            ECSensor::instance()->calOnePointFour();
        }
        else if (page.LCD_PAGE == 3)
        {
            lcd.setCursor(0, 1);
            lcd.print("EC 12->Success!");
            ECSensor::instance()->calTwelve();

        }
        else if (page.LCD_PAGE == 4)
        {
            lcd.setCursor(0, 1);
            lcd.print("Set Zero Success");
            LoadCellSensor::instance()->tareZero();
        }
        delay(2000);
        page.LCD_PAGE = 1;
    }
};
ButtonHandler *ButtonHandler::s_instance = 0;


class LcdHandler : public Task
{
  public:
    static LcdHandler *s_instance;
    LcdHandler() : Task(MsToTaskTime(100))
    {
        lcd.init();
        lcd.backlight();
        lcd.clear();
        page = {0};
        currentPage = page.LCD_PAGE;
        lcd.setCursor(0, 0);
        lcd.print("LCD Init.");
        delay(1000);

        taskManager.StartTask(ButtonHandler::instance());
    }
    static LcdHandler *instance()
    {
        if (!s_instance)
            s_instance = new LcdHandler;
        return s_instance;
    }

  private:
    int currentPage;
    virtual bool OnStart() { return true; }
    virtual void OnUpdate(uint32_t delta_time)
    {

        if (currentPage != page.LCD_PAGE)
        {
            currentPage = page.LCD_PAGE;
            lcd.clear();
        }
        if (currentPage == 0)
        {
            Serial.println(page.page1.line1 + " " + page.page1.line2);
            lcd.setCursor(0, 0);
            lcd.print(page.page1.line1);

            lcd.setCursor(0, 1);
            lcd.print(page.page1.line2);
        }
        else if (currentPage == 1)
        {
            float vol = LoadCellSensor::instance()->getVal();
            float ec = ECSensor::instance()->GetEC();
            lcd.setCursor(0, 0);
            lcd.print("WiFi:" + CNT + " STA:" + String(station) + " " + RST);
            lcd.setCursor(0, 1);
            lcd.print("EC:" + String(ec) + " Vol:" + String(vol));
        }
        else if (currentPage == 2)
        {
            lcd.setCursor(0, 0);
            lcd.print("EC:" + String(ECSensor::instance()->GetEC()) + " CAL:1.4");
        }
        else if (currentPage == 3)
        {
            lcd.setCursor(0, 0);
            lcd.print("EC:" + String(ECSensor::instance()->GetEC()) + " CAL:12");
        }
        else if (currentPage == 4)
        {
            lcd.setCursor(0, 0);
            lcd.print("Set Zero Vol:" + String(LoadCellSensor::instance()->getVal()));
        }
    }
};
LcdHandler *LcdHandler::s_instance = 0;

