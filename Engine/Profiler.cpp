#include "stdafx.h"
#include "Profiler.h"
#include <stdio.h>


static FILE* s_jsonFile = nullptr;
static bool s_doComma = false;

void Profiler::Timer::Start()
{
    mStart = std::chrono::high_resolution_clock::now();

    if (nullptr != s_jsonFile)
    {
        if (s_doComma)
        {
            fprintf(s_jsonFile, ",\n");
        }
        uint64_t startTime = mStart.time_since_epoch().count() / 1000;
        fprintf(s_jsonFile, "\t{\"name\": \"%s\", \"cat\": \"PERF\", \"ph\": \"B\", \"pid\": 1, \"tid\": 1, \"ts\": %llu}",
            this->GetName().c_str(), startTime);
    }
    s_doComma = true;
}

void Profiler::Timer::Stop()
{
    auto end = std::chrono::high_resolution_clock::now();
    double time_ns = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - mStart).count();
    mCurrent_ms += 0.000001 * time_ns;

    if (nullptr != s_jsonFile)
    {
        fprintf(s_jsonFile, ",\n");
        uint64_t endTime = end.time_since_epoch().count() / 1000;
        fprintf(s_jsonFile, "\t{\"ph\": \"E\", \"pid\": 1, \"tid\": 1, \"ts\": %llu}",
            endTime);
    }
    s_doComma = true;
}

/*static*/ Profiler* Profiler::Get()
{
    static Profiler s_theProfiler;
    return &s_theProfiler;
}

Profiler::Timer *Profiler::GetTimer(const std::string &name)
{
    auto find = mTimers.find(name);
    if (find != mTimers.end())
        return find->second;
    Timer* pTimer = new Timer(name);
    mTimers[name] = pTimer;
    return pTimer;
}

void Profiler::ResetAll()
{
    for (auto& timer : mTimers)
    {
        timer.second->Reset();
    }
}

Profiler::Profiler()
{
    fopen_s(&s_jsonFile, "profile.json", "wt");
    if (nullptr != s_jsonFile)
    {
        fprintf(s_jsonFile, "[\n");
    }
}

Profiler::~Profiler()
{
    if (nullptr != s_jsonFile)
    {
        fprintf(s_jsonFile, "]\n");
        fclose(s_jsonFile);
    }

    FILE* pFile = nullptr;
    fopen_s(&pFile, "profile.txt", "wt");
    if (nullptr != pFile)
    {
        fprintf(pFile, "name,\tavg,\tmax\n");
        for (const auto& iter : mTimers)
        {
            const Timer *pTimer = iter.second;
            fprintf(pFile, "%s,\t%lf,\t%lf\n", pTimer->GetName().c_str(), pTimer->GetAvg_ms(), pTimer->GetMax_ms());
        }
        fclose(pFile);
    }
}