#include "EventFilter.h"

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <string.h>
#include <unistd.h>

//#define nthread 1
const int vertif = 0x1234abcd;

using namespace std;

int Write(const string filePath, const EventArray &Data)
{
    ofstream ofile(filePath.c_str(), ios::binary | ios::trunc | ios::out);
    if (ofile.is_open() == false)
    {
        cout << "Open file fail!" << endl;
        exit(1);
    }
    ofile.write((char *)&vertif, sizeof(int));

    int length = Data.size();
    ofile.write((char *)&length, sizeof(int));

    int totalSize = Data.size() * sizeof(Event);
    ofile.write((char *)&totalSize, sizeof(int));

    ofile.write((char *)&Data[0], totalSize);
    ofile.write((char *)&vertif, sizeof(int));

    ofile.close();
    return 0;
}

int Read(const string filePath, EventArray &Data)
{
    ifstream ifile(filePath.c_str(), ios::binary | ios::in);
    int tmpVertif, length, totalSize;
    ifile.read((char *)&tmpVertif, sizeof(int));
    if (tmpVertif != vertif)
    {
        cout << "Unknow format at the begin of file...";
        exit(1);
    }
    ifile.read((char *)&length, sizeof(int));
    ifile.read((char *)&totalSize, sizeof(int));
    Data.resize(length);
    ifile.read((char *)&Data[0], totalSize);
    ifile.read((char *)&tmpVertif, sizeof(int));
    if (tmpVertif != vertif)
    {
        cout << "Unknow format at the end of file..." << endl;
        exit(1);
    }
    return 0;
}

void Product(const unsigned int N, EventArray &evts)
{
    EventFilter *producer = new EventFilter;
    producer->SetEventCollection(&evts);
    for (unsigned int i = 0; i < N; i++)
    {
        producer->AddEvent(1000 + (rand() % 1000), time(0));
    }
    cout << "-INFO evts : \t"; producer->Print();
    delete producer;
}

int main(int argc, char *argv[])
{
    clock_t start, end;
    EventArray evts1, evts2, evts3;
    EventFilter *filter = new EventFilter;

    int nthread = 1;
    bool is_save = true;
    if (argc > 1) nthread = atoi(argv[1]);
    if (argc > 2) is_save = atoi(argv[2]);
    if (is_save) {
        cout << "-INFO thread: " << nthread << "   Is Save: true" << endl;
    } else cout << "-INFO thread: " << nthread << "   Is Save: false" << endl;

    if (is_save)
    {
        Product(3, evts1);
        Product(4, evts2);

        Write("data1.txt", evts1);
        Write("data2.txt", evts2);
    }
    else
    {
        Read("data1.txt", evts1);
        Read("data2.txt", evts2);
    }

    cout << "\n[Add...]";
    filter->SetEventCollection(&evts3);
    start = clock();
    RunMultiThread(nthread, &EventFilter::AddEvents, filter, evts1);
    RunMultiThread(nthread, &EventFilter::AddEvents, filter, evts2);
    end = clock();
    cout << "\ttime = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "-INFO evts3 -> Merge(evts1,evts2) : \t"; filter->Print();

    cout << "\n[Sort...]";
    start = clock();
    filter->Sort();
    end = clock();
    cout << "\ttime = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "-INFO evts3 -> Sort() : \t"; filter->Print();

    cout << "\n[Remove...]";
    start = clock();
    filter->Remove(1000, 1500);
    end = clock();
    cout << "\ttime = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "-INFO evts3 -> Remove() : \t"; filter->Print();

    cout << "\n[Transform...]";
    start = clock();
    RunMultiThread(nthread, &EventFilter::Transform, filter);
    end = clock();
    cout << "\ttime = " << double(end - start) / CLOCKS_PER_SEC << "s" << endl;
    cout << "-INFO evts3 -> Transform() :\t"; filter->Print();

    delete filter;
    return 0;
}