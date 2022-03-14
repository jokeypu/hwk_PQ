#ifndef EVENTFILTER_HH
#define EVENTFILTER_HH

#include <vector>
#include <mutex>
#include <thread>

struct Event {
    uint64_t id = 0;
    uint32_t timestamp = 0;
};

typedef std::vector<Event> EventArray;

class EventFilter {
    public:
    //Constructors
    explicit EventFilter(EventArray* _obj= nullptr);
    //Destructor
    ~EventFilter();

    inline void SetEventCollection(EventArray *_obj) {  //Set the pointer of EventArray
        fEventArray = _obj;
        fIndexM = fEventArray->size();
    }
    int Reset();

    int AddEvents(const EventArray&);                   //Add Events to fEventArray 
    void AddEvent(const Event&);
    void AddEvent(uint64_t _id, uint32_t _timestamp);
    int Remove(uint64_t _min_id, uint64_t _max_id);     //Remove Events with id between _min_id and _max_id
    int Transform();                                    //id = id % 1000
    void Sort();                                        //Sort by id

    void Print();

    private:
    EventArray *fEventArray;
    unsigned int fIndexP;
    unsigned int fIndexM;
    std::mutex mtx;
    bool init_flag;
};

#define RunMultiThread(numThread, ptrNonStaticFun, ptrFilter, ...) \
{ \
        std::thread *th = new std::thread[numThread];\
        ptrFilter->Reset();\
        for (int i = 0; i < numThread; i++) th[i] = std::thread(ptrNonStaticFun, ptrFilter, ##__VA_ARGS__);\
        for (int i = 0; i < numThread; i++) th[i].join();\
        delete[] th, th = NULL;\
}   //Multi-thread

#endif /*EVENTFILTER_HH*/