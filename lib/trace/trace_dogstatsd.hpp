#pragma once

#include <string>
#include <vector>
#include <mutex>

namespace trace {

const std::string default_dog_host = "localhost";
const uint32_t default_dog_port = 8125;
const uint32_t max_udp_payload = 65467;
const uint32_t max_buff_cmds = 100;

class Dogstatsd {
public:
    Dogstatsd();
    Dogstatsd(bool);
    Dogstatsd(std::string, uint32_t, bool);
    ~Dogstatsd();

    int gauge( std::string, double, std::vector<std::string>, double );
    int count( std::string, int64_t, std::vector<std::string>, double );
    int histogram( std::string, double, std::vector<std::string>, double );
    int incr( std::string, std::vector<std::string>, double );
    int decr( std::string, std::vector<std::string>, double );
    int set( std::string, std::string, std::vector<std::string>, double );
protected:
    std::string host;
    uint32_t port;
    bool buffered;
    std::vector<std::string> cmd_buffer;
    int send( std::string, std::string, std::vector<std::string>, double );
    int flush();
private:
    int _sockfd;
    std::mutex _buff_mutex;
    struct sockaddr_in _server;

    std::string format( std::string, std::string, std::vector<std::string>, double );

};

/**
* Singleton.
*/
extern Dogstatsd dogstatsd;

}
