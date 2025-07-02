#include "../includes/ZmqService.hpp"
#include "../includes/Logger.hpp"
#include <cstring>

ZmqService::ZmqService()
    : running_(false) {
    context_ = zmq_ctx_new();
    Logger::get()->info("[ZmqService] Context created");
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
    Logger::get()->info("[ZmqService] Context destroyed");
}

void ZmqService::addInput(const std::string& endpoint) {
    void* sock = zmq_socket(context_, ZMQ_PULL);
    if (zmq_connect(sock, endpoint.c_str()) != 0) {
        Logger::get()->error("[ZmqService] Failed to connect input socket to {}: {}", endpoint, zmq_strerror(zmq_errno()));
        zmq_close(sock);
        return;
    }
    inputs_[endpoint] = sock;
    Logger::get()->info("[ZmqService] Input connected: {}", endpoint);
}

void ZmqService::addOutput(const std::string& endpoint) {
    void* sock = zmq_socket(context_, ZMQ_PUSH);
    if (zmq_connect(sock, endpoint.c_str()) != 0) {
        Logger::get()->error("[ZmqService] Failed to connect output socket to {}: {}", endpoint, zmq_strerror(zmq_errno()));
        zmq_close(sock);
        return;
    }
    outputs_[endpoint] = sock;
    Logger::get()->info("[ZmqService] Output connected: {}", endpoint);
}

void ZmqService::send(const std::string& endpoint, const std::string& message) {
    if (outputs_.count(endpoint)) {
        zmq_send(outputs_[endpoint], message.c_str(), message.size(), 0);
        Logger::get()->debug("[ZmqService] Sent message to {}: {}", endpoint, message);
    } else {
        Logger::get()->warn("[ZmqService] Tried to send to unknown endpoint: {}", endpoint);
    }
}

void ZmqService::addCallback(const std::string& endpoint, std::function<void(const std::string&)> callback) {
    callbacks_[endpoint] = callback;
    Logger::get()->info("[ZmqService] Callback added for endpoint: {}", endpoint);
}

void ZmqService::run() {
    running_ = true;
    loopThread_ = std::thread(&ZmqService::pollingLoop, this);
    Logger::get()->info("[ZmqService] Polling loop started");
}

void ZmqService::stop() {
    running_ = false;
    if (loopThread_.joinable()) {
        loopThread_.join();
        Logger::get()->info("[ZmqService] Polling loop stopped");
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
                    Logger::get()->debug("[ZmqService] Received message from {}: {}", endpoint, data);
                    if (callbacks_.count(endpoint)) {
                        callbacks_[endpoint](data);
                    }
                }
            }
        }
    }
}
