#include "../includes/ZmqService.hpp"
#include <iostream>

ZmqService::ZmqService()
    : running_(false) {}

ZmqService::~ZmqService() {
    stop();
    for (auto& [_, sock] : inputs_) delete sock;
    for (auto& [_, sock] : outputs_) delete sock;
}

void ZmqService::addInput(const std::string& endpoint) {
    auto* sock = new zmqpp::socket(context_, zmqpp::socket_type::pull);
    sock->connect(endpoint);
    inputs_[endpoint] = sock;
}

void ZmqService::addOutput(const std::string& endpoint) {
    auto* sock = new zmqpp::socket(context_, zmqpp::socket_type::push);
    sock->connect(endpoint);
    outputs_[endpoint] = sock;
}

void ZmqService::send(const std::string& endpoint, const std::string& message) {
    if (outputs_.count(endpoint)) {
        zmqpp::message msg;
        msg << message;
        outputs_[endpoint]->send(msg);
    }
}

void ZmqService::addCallback(const std::string& endpoint, std::function<void(const std::string&)> callback) {
    callbacks_[endpoint] = callback;
}

void ZmqService::run() {
    running_ = true;
    loopThread_ = std::thread(&ZmqService::pollingLoop, this);
}

void ZmqService::stop() {
    running_ = false;
    if (loopThread_.joinable()) loopThread_.join();
}

void ZmqService::pollingLoop() {
    while (running_) {
        for (auto& [endpoint, sock] : inputs_) {
            zmqpp::message msg;
            if (sock->receive(msg, true)) { 
                std::string data;
                msg >> data;
                if (callbacks_.count(endpoint)) {
                    callbacks_[endpoint](data);
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
