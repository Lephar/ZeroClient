#include "server.hpp"

namespace zero::network {
    void optimizeSocket(int32_t socket) {
        int32_t nodelayValue = 1;
        uint8_t tosValue = IPTOS_LOWDELAY;

        setsockopt(socket, IPPROTO_IP, IP_TOS, &tosValue, sizeof(uint8_t));
        setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &nodelayValue, sizeof(int32_t));
    }

    Server::Server(const char *ip, uint32_t port) : address(), lock(), receiver(), sender() {
        active = false;
        pthread_rwlock_init(&lock, nullptr);

        server = socket(AF_INET, SOCK_STREAM, 0);
        optimizeSocket(server);

        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_pton(AF_INET, ip, &address.sin_addr);
    }

    bool Server::isActive() {
        pthread_rwlock_rdlock(&lock);
        bool value = active;
        pthread_rwlock_unlock(&lock);

        return value;
    }

    void Server::setActive(bool value) {
        pthread_rwlock_wrlock(&lock);
        active = value;
        pthread_rwlock_unlock(&lock);
    }

    void Server::receiveLoop() {
        while (isActive()) {

        }
    }

    void Server::sendLoop() {
        while (isActive()) {

        }
    }

    void *receiverProxy(void *context) {
        auto server = reinterpret_cast<Server *>(context);
        server->receiveLoop();
        return nullptr;
    }

    void *senderProxy(void *context) {
        auto server = reinterpret_cast<Server *>(context);
        server->sendLoop();
        return nullptr;
    }

    void Server::connect() {
        static_cast<void>(::connect(server, reinterpret_cast<sockaddr *>(&address), sizeof(sockaddr_in)));

        setActive(true);

        pthread_create(&receiver, nullptr, receiverProxy, this);
        pthread_create(&sender, nullptr, senderProxy, this);
    }

    void Server::disconnect() {
        if(!isActive())
            return;

        setActive(false);

        pthread_join(receiver, nullptr);
        pthread_join(sender, nullptr);

        close(server);
    }

    Server::~Server() {
        disconnect();
    }
}