#pragma once
#include "mpch.h"
#include <condition_variable>
#include <queue>
#include <stop_token>

#include "Mana/Core/Platform.h"

namespace Mana {
// Define job priorities
enum class JobPriority { Low, Normal, High };

// Job structure
struct Job {
    std::function<void()> task;
    JobPriority priority;

    // Comparison operator for priority queue
    bool operator<(const Job &other) const { return priority < other.priority; }
};

class ScheduledJob {
  public:
    Job job;
    Time::Duration interval; // Time between executions
    Time::Moment nextRun;    // When to run next

    bool isRunning = false;
    bool runOnce;

    ScheduledJob(Job job, Time::Duration interval, bool runOnce = false)
        : job(std::move(job)), interval(interval),
          nextRun(runOnce ? (Time::Now() + interval) : Time::Now()),
          runOnce(runOnce) {}

    // Comparison operator for priority queue (if needed)
    bool operator<(const ScheduledJob &other) const {
        Time::Moment a = Time::Now() + interval;
        return nextRun > other.nextRun; // Earlier time has higher priority
    }
};

class Worker {
  public:
    Worker(uint32_t ID, std::priority_queue<Job> &jobsQueue,
           std::mutex &jobsMutex, std::condition_variable &jobsCV)
        : m_ID(ID), m_Jobs(jobsQueue), m_JobsMutex(jobsMutex),
          m_JobsCV(jobsCV) {
        thread = std::jthread(
            [this](std::stop_token stoken) { WorkerLoop(stoken); });
    }

    void EnsureStop() {
        thread.request_stop();
        m_JobsCV.notify_all();
        thread.join();
    }

  private:
    void WorkerLoop(std::stop_token stoken);

  private:
    uint32_t m_ID;
    std::priority_queue<Job> &m_Jobs;
    std::mutex &m_JobsMutex;
    std::condition_variable &m_JobsCV;

    std::jthread thread;
};

class ServicesManager {
  public:
    ServicesManager(uint32_t threadCount = 2);
    ~ServicesManager();
    void Queue(Job job);
    void Schedule(std::string id, Job job, Time::Duration interval);
    void ScheduleOnce(std::string id, Job job, Time::Duration interval);
    bool CancelScheduled(const std::string &id);

  private:
    void SchedulerLoop(std::stop_token stoken);

  private:
    std::vector<Worker> m_Workers;
    std::jthread m_SchedulerThread;

    std::priority_queue<Job> m_Jobs;
    std::mutex m_JobsMutex;
    std::condition_variable m_JobsCV;

    std::unordered_map<std::string, ScheduledJob> m_Schedule;
    std::mutex m_ScheduleMutex;
};
} // namespace Mana
