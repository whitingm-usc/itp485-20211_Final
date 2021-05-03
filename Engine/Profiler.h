#pragma once

class Profiler
{
public:
    class Timer
    {
        friend class Profiler;
    public:
        void Start();
        void Stop();
        void Reset()
        {
            mTotalTimes_ms += mCurrent_ms;
            ++mNumSample;
            if (mCurrent_ms > mMax_ms)
                mMax_ms = mCurrent_ms;
            mCurrent_ms = 0.0;
        }
        const std::string &GetName() const { return mName; }
        double GetTime_ms() const { return mCurrent_ms; }
        double GetMax_ms() const { return mMax_ms; }
        double GetAvg_ms() const
        {
            if (mNumSample > 0)
                return mTotalTimes_ms / mNumSample;
            return 0.0;
        }

    private:
        Timer(std::string name)
            : mName(name)
            , mCurrent_ms(0.0)
            , mMax_ms(0.0)
            , mTotalTimes_ms(0.0)
            , mNumSample(0)
        {}
        ~Timer() {}

        std::string mName;
        double mCurrent_ms;
        double mMax_ms;
        double mTotalTimes_ms;
        int mNumSample;
        std::chrono::high_resolution_clock::time_point mStart;
    };

    class ScopedTimer
    {
    public:
        ScopedTimer(Timer* timer)
        {
            mTimer = timer;
            mTimer->Start();
        }
        ~ScopedTimer()
        {
            mTimer->Stop();
        }

    private:
        Timer *mTimer;
    };

    static Profiler *Get();
    Timer *GetTimer(const std::string &name);
    void ResetAll();

private:
    Profiler();
    ~Profiler();

    std::unordered_map<std::string, Timer*> mTimers;
};

#define PROFILE_SCOPE(name) \
Profiler::ScopedTimer name##_scope(Profiler::Get()->GetTimer(std::string(#name)))
