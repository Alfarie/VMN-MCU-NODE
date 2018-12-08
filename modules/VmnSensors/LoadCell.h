#include <Task.h>
extern TaskManager taskManager;
class LoadCell : public Task
{
  public:
    static LoadCell *s_instance;
    LoadCell() : Task(MsToTaskTime(1000)) {
        volume = 0;
    }
    static LoadCell *instance()
    {
        if (!s_instance)
            s_instance = new LoadCell;
        return s_instance;
    }

    float getVal(){
        return volume;
    }

  private:
    float volume;
    virtual bool OnStart() { return true; }
    virtual void OnUpdate(uint32_t delta_time) {
        volume = getLoadCell();
    }

    float getLoadCell(){
        return 0;
    }
};
LoadCell *LoadCell::s_instance = 0;