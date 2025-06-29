#pragma once

#include <zmq.h>
#include <unordered_map>
#include <functional>
#include <thread>
#include <atomic>
#include <string>

class ZmqService {
public:
    ZmqService();
    ~ZmqService();

    void addInput(const std::string& endpoint);
    void addOutput(const std::string& endpoint);

    void send(const std::string& endpoint, const std::string& message);
    void addCallback(const std::string& endpoint, std::function<void(const std::string&)> callback);

    void run();
    void stop();

private:
    void* context_;
    std::unordered_map<std::string, void*> inputs_;
    std::unordered_map<std::string, void*> outputs_;
    std::unordered_map<std::string, std::function<void(const std::string&)>> callbacks_;

    std::thread loopThread_;
    std::atomic<bool> running_;

    void pollingLoop();
};
