#include "ThreadPool.h"


ThreadPool::ThreadPool(int _size) : stop(false) {
    for(int i=0;i<_size;++i){
        threads.emplace_back(std::thread([this](){
            while(1){
                std::function<void()> task;
                if(1){
                    std::unique_lock<std::mutex> lock(this->tasks_mtx);
                    cv.wait(lock,[this](){
                        return stop || !tasks.empty();
                    });
                    if(stop && tasks.empty()) return;
                    task = tasks.front();
                    tasks.pop();
                }
                task();
            }
        }));
    }
}

ThreadPool::~ThreadPool(){
    if(1){
        std::unique_lock<std::mutex> lock(this->tasks_mtx);
        stop=1;
    }
    cv.notify_all();
    for(auto& th:threads){
        if(th.joinable()) th.join();
    }
}

// void ThreadPool::add(std::function<void()> func){
//     if(1){
//         std::unique_lock<std::mutex> lock(this->tasks_mtx);
//         if(stop)
//             throw(std::runtime_error("ThreadPool already stopped, can't add task any more"));
//         tasks.push(func);
//     }
//     cv.notify_one();
// }