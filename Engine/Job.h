#pragma once
#include <thread>

class JobManager
{
public:
    class Job
    {
    public:
        virtual void DoIt() = 0;
    };

    void Begin();
    void End();
    void AddJob(Job* pJob);
    void WaitForJobs();

private:
    class Worker
    {
    public:
        Worker() 
            : mThread(nullptr) 
        {}
        void Begin();
        void End();
        static void Loop();
    private:
        std::thread* mThread;
    };

    enum { NUM_WORKER = 4 };

    Worker m_worker[NUM_WORKER];
};