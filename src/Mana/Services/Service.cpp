#include "Service.h"
#include <mutex>
#include <stop_token>
namespace Mana {
void Worker::WorkerLoop(std::stop_token stoken) {
    MC_TRACE("Thread {} started!", m_ID);

    while (!stoken.stop_requested()) {
        Job job;
        bool hasJob = false;

        // Create a wrapper with unique lock
        {
            std::unique_lock lock(m_JobsMutex);

            // Wait until job is ready, or its time to stop
            m_JobsCV.wait(lock, [&]() {
                return !m_Jobs.empty() || stoken.stop_requested();
            });

            // Stop if that is what is requested
            if (stoken.stop_requested())
                break;

            // Get a job
            if (!m_Jobs.empty()) {
                job = m_Jobs.top();
                m_Jobs.pop();
                hasJob = true;
            }
        }

        // Execute job if job is set
        if (hasJob) {
            job.task();
        }
    }

    MC_TRACE("Worker thread {} stopping", m_ID);
}

ServicesManager::ServicesManager(uint32_t threadCount) {
    // Initialize at least 2 threads, but not more than hardware_concurrency
    threadCount =
        std::max(uint32_t(2),
                 std::min(threadCount, std::thread::hardware_concurrency()));

    // Initialize worker threads
    MC_INFO("Starting ServicesManager with {} worker threads", --threadCount);
    m_Workers.reserve(threadCount);

    for (uint32_t i = 0; i < threadCount; i++) {
        m_Workers.emplace_back(i, m_Jobs, m_JobsMutex, m_JobsCV);
    }

    // Initialize Scheduler
    m_SchedulerThread =
        std::jthread([this](std::stop_token stoken) { SchedulerLoop(stoken); });
}

void ServicesManager::SchedulerLoop(std::stop_token stoken) {
    MC_INFO("Scheduler Thread Started");

    while (!stoken.stop_requested()) {
        auto now = Time::Now();
        {
            std::lock_guard sLock(m_ScheduleMutex);
            for (auto &[id, job] : m_Schedule) {
                if (now >= job.nextRun && !job.isRunning) {
                    // Create a wrapper job that updates the next run when
                    // finished
                    Job wrapperJob = job.job;
                    std::string jobId = id; // Copy the ID for the lambda

                    // Make new task
                    auto originalTask = job.job.task;
                    wrapperJob.task = [this, originalTask, jobId]() {
                        MC_TRACE("Running scheduled job '{}'", jobId);
                        try {
                            // Execute the original task
                            originalTask();
                        } catch (const std::exception &e) {
                            MC_ERROR("Exception in recurring job {}: {}", jobId,
                                     e.what());
                        } catch (...) {
                            MC_ERROR("Unknown exception in recurring job {}",
                                     jobId);
                        }

                        // Update schedule.nextRun
                        std::lock_guard recLock(m_ScheduleMutex);
                        auto it = m_Schedule.find(jobId);
                        if (it->second.runOnce)
                            return;
                        if (it != m_Schedule.end()) {
                            it->second.nextRun =
                                Time::Now() + it->second.interval;
                            it->second.isRunning = false;
                        }
                    };

                    // Queue the wrapper job
                    this->Queue(wrapperJob);

                    // Prevent the schedule frome being queued while the job is
                    // running
                    job.isRunning = true;
                }
            }
        }

        std::this_thread::sleep_for(Time::Millis(100)); // Check every 100ms
    }

    MC_INFO("Scheduler Thread Stopped!");
}

ServicesManager::~ServicesManager() {
    MC_TRACE("ServicesManager shutting down...");

    // Stop accepting new recurring jobs
    {
        std::lock_guard recLock(m_ScheduleMutex);
        m_Schedule.clear();
    }

    // Clear the job queue to prevent processing new jobs
    {
        std::lock_guard lock(m_JobsMutex);

        // Create an empty queue and swap with the existing one to clear it
        std::priority_queue<Job> empty;
        std::swap(m_Jobs, empty);
    }

    MC_TRACE("Waiting for scheduler thread to stop...");
    if (m_SchedulerThread.joinable()) {
        m_SchedulerThread.request_stop();
        m_SchedulerThread.join();
    }

    // Force all worker threads to stop
    MC_TRACE("Waiting for worker threads to stop...");
    for (auto &worker : m_Workers) {
        worker.EnsureStop();
    }

    MC_TRACE("ServicesManager shutdown complete");
}

void ServicesManager::Queue(Job job) {
    {
        std::lock_guard lock(m_JobsMutex);
        m_Jobs.push(std::move(job));
    }

    m_JobsCV.notify_one();
}

void ServicesManager::Schedule(std::string id, Job job,
                               Time::Duration interval) {
    MC_TRACE("Adding job '{}' to the job schedule: interval={}", id,
             interval.count());
    {
        std::lock_guard lock(m_ScheduleMutex);
        m_Schedule.try_emplace(id, std::move(job), interval);
    }
}

void ServicesManager::ScheduleOnce(std::string id, Job job,
                                   Time::Duration interval) {
    MC_TRACE("Adding job '{}' to the job schedule, to only run once: timer={}",
             id, interval.count());
    {
        std::lock_guard lock(m_ScheduleMutex);
        m_Schedule.try_emplace(id, std::move(job), interval, true);
    }
}

bool ServicesManager::CancelScheduled(const std::string &id) {
    MC_TRACE("Removing job '{}' from the job schedule", id);

    std::lock_guard lock(m_ScheduleMutex);
    auto it = m_Schedule.find(id);
    if (it != m_Schedule.end()) {
        m_Schedule.erase(it);
        return true;
    }
    return false;
}
} // namespace Mana
