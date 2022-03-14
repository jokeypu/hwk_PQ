//-----------------------
// This Class's Header --
//-----------------------
#include "EventFilter.h"

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <algorithm>
#include <string.h>
#include <unistd.h>

using std::endl;
using std::cout;

//----------------
// Constructors --
//----------------
EventFilter::EventFilter(EventArray *obj) 
:fEventArray(obj), init_flag(true), fIndexP(0)
{
    if (fEventArray != nullptr) {
        fIndexM = fEventArray->size();
    }
}

//--------------
// Destructor --
//--------------
EventFilter::~EventFilter()
{
}

/**
 * @brief Reset Multi-thread Task
 * 
 * @return int
 * @retval 0 success
 */
int EventFilter::Reset()
{
    mtx.lock();
    if (init_flag) {
        init_flag = false;
        mtx.unlock(); 
        fIndexP = 0;
        fIndexM = fEventArray->size();
        usleep(100000);
        init_flag = true;
        return 0;
    }
    mtx.unlock();
    return 0;
}

int EventFilter::AddEvents(const EventArray& _evts)
{
    unsigned int len = _evts.size();
    for ( ; ; ) {
        std::lock_guard<std::mutex> guard(mtx);
        if (fIndexP == len  || !init_flag) break;
        auto index = fIndexP++;
        fEventArray->push_back(_evts[index]);
    }
    return this->Reset();
}

void EventFilter::AddEvent(const Event& _evt)
{
    std::lock_guard<std::mutex> guard(mtx);
    fEventArray->push_back(_evt);
}

void EventFilter::AddEvent(uint64_t _id, uint32_t _timestamp)
{
    std::lock_guard<std::mutex> guard(mtx);
    struct Event evt;
    evt.id = _id;
    evt.timestamp = _timestamp;
    fEventArray->push_back(evt);
}

int EventFilter::Remove(uint64_t _min_id, uint64_t _max_id)
{
    unsigned int len = fEventArray->size();
    for ( ; ; ) {
        std::lock_guard<std::mutex> guard(mtx);
        if (fIndexM == 0 || !init_flag) break;
        auto index = --fIndexM;
        auto cul_id = fEventArray->at(index).id;
        if ( cul_id <= _max_id && cul_id >= _min_id) {
            fEventArray->erase(fEventArray->begin() + index);
        }
    }
    return this->Reset();
}

int EventFilter::Transform()
{
    unsigned int index;
    unsigned int len = fEventArray->size();
    for ( ; ; ) {
        {
            std::lock_guard<std::mutex> guard(mtx);
            if (fIndexP == len || !init_flag) break;
            index = fIndexP++;
        }
        (fEventArray->at(index).id) %= 1000;
    }
    
    return this->Reset();
}

void EventFilter::Sort()
{
    std::sort(fEventArray->begin(), fEventArray->end(), 
        [](const Event evt1, const Event evt2) -> bool { return evt1.id < evt2.id; });
}

void EventFilter::Print() 
{
    EventArray::const_iterator citer;
    cout << "{ ";
    for (citer = fEventArray->cbegin(); citer != fEventArray->cend()-1; ++citer) {
        cout << "{" << citer->id << "," << citer->timestamp << "}, ";
    }
    cout <<  "{" << citer->id << "," << citer->timestamp << "} }" << endl;
}