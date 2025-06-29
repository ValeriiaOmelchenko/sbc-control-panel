#include "../includes/ZmqService.hpp"
#include <iostream>
#include <cstring>

ZmqService::ZmqService()
    : running_(false) {
    context_ = zmq_ctx_new();
}

ZmqService::~ZmqService() {
    stop();

    for (auto& [_, sock] : inputs_) {
        zmq_close(sock);
    }

    for (auto& [_, sock] : outputs_) {
        zmq_close(sock);
    }

    zmq_ctx_term(context_);
}

void ZmqService::addInput(const std::string& endpoint) {
    void* sock = zmq_socket(context_, ZMQ_PULL);
    if (zmq_connect(sock, endpoint.c_str()) != 0) {
        std::cerr << "Failed to connect input socket to " << endpoint << ": " << zmq_strerror(zmq_errno()) << "\n";
        zmq_close(sock);
        return;
    }
    inputs_[endpoint] = sock;
}

void ZmqService::addOutput(const std::string& endpoint) {
    void* sock = zmq_socket(context_, ZMQ_PUSH);
    if (zmq_connect(sock, endpoint.c_str()) != 0) {
        std::cerr << "Failed to connect output socket to " << endpoint << ": " << zmq_strerror(zmq_errno()) << "\n";
        zmq_close(sock);
        return;
    }
    outputs_[endpoint] = sock;
}

void ZmqService::send(const std::string& endpoint, const std::string& message) {
    if (outputs_.count(endpoint)) {
        zmq_send(outputs_[endpoint], message.c_str(), message.size(), 0);
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
    if (loopThread_.joinable()) {
        loopThread_.join();
    }
}

void ZmqService::pollingLoop() {
    constexpr int timeout_ms = 10;

    while (running_) {
        for (auto& [endpoint, sock] : inputs_) {
            zmq_pollitem_t item = { sock, 0, ZMQ_POLLIN, 0 };
            int rc = zmq_poll(&item, 1, timeout_ms);

            if (rc > 0 && (item.revents & ZMQ_POLLIN)) {
                char buffer[1024] = {0};
                int size = zmq_recv(sock, buffer, sizeof(buffer) - 1, 0);
                if (size >= 0) {
                    buffer[size] = '\0';  // null-terminate
                    std::string data(buffer);
                    if (callbacks_.count(endpoint)) {
                        callbacks_[endpoint](data);
                    }
                }
            }
        }
    }
}
