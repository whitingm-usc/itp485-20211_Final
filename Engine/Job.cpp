#include "stdafx.h"
#include "Job.h"
#include <mutex>
#include <queue>
#include <atomic>

static volatile bool s_shutdown = false;
static std::queue<JobManager::Job*> s_jobQueue;
static std::atomic<int> s_jobCount = 0;

static std::mutex* GetMutex()
{
    static std::mutex s_jobListMutex;
    return &s_jobListMutex;
}

void JobManager::Begin()
{
    for (int i = 0; i < NUM_WORKER; ++i)
        m_worker[i].Begin();
}

void JobManager::End()
{
    s_shutdown = true;
    for (int i = 0; i < NUM_WORKER; ++i)
        m_worker[i].End();
}

void JobManager::AddJob(Job* pJob)
{
    GetMutex()->lock();
    s_jobQueue.push(pJob);
    ++s_jobCount;
    GetMutex()->unlock();
}

void JobManager::WaitForJobs()
{
    while (s_jobCount > 0)
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

void JobManager::Worker::Begin()
{
    mThread = new std::thread(Loop);
}

void JobManager::Worker::End()
{
    mThread->join();
    delete mThread;
}

void JobManager::Worker::Loop()
{
    while (false == s_shutdown)
    {
        GetMutex()->lock();
        if (s_jobQueue.empty())
        {
            GetMutex()->unlock();
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
        }
        else
        {
            Job* pJob = s_jobQueue.front();
            s_jobQueue.pop();
            GetMutex()->unlock();
            pJob->DoIt();
            --s_jobCount;
        }
    }
}
