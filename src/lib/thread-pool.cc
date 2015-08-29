#include <lader/thread-pool.h>

#include <lader/util.h>

using namespace lader;
using namespace std;
using namespace boost;

ThreadPool::ThreadPool(int num_threads, int queue_limit) :
        stopped_(false), stopping_(false), queue_limit_(queue_limit) {
    for(int i = 0; i < num_threads; i++)
        threads_.create_thread(bind(&ThreadPool::Run, this));
}

void ThreadPool::Run() {
    while(!stopped_) {
        Task* task = NULL;
        {
            boost::mutex::scoped_lock lock(mutex_);
            if(tasks_.empty() && !stopped_)
                thread_needed_.wait(lock);
            if(!stopped_ && !tasks_.empty()) {
                task = tasks_.front();
                tasks_.pop();
            }
        }
        if(task) {
            task->Run();
            delete task;
        }
        thread_available_.notify_all();
    }
}

void ThreadPool::Submit(Task* task) {
    boost::mutex::scoped_lock lock(mutex_);
    if(stopping_)
        THROW_ERROR("Cannot accept new jobs while ThreadPool is stopping");
    while(queue_limit_ && (int)tasks_.size() >= queue_limit_)
        thread_available_.wait(lock);
    tasks_.push(task);
    thread_needed_.notify_all();
}

void ThreadPool::Stop(bool process_remaining) {
    {
        boost::mutex::scoped_lock lock(mutex_);
        if(stopped_) return;
        stopping_ = true;
    }
    if(process_remaining) {
        boost::mutex::scoped_lock lock(mutex_);
        while(!tasks_.empty() && !stopped_)
            thread_available_.wait(lock);
    }
    {
        boost::mutex::scoped_lock lock(mutex_);
        stopped_ = true;
    }
    thread_needed_.notify_all();
    threads_.join_all();
}
