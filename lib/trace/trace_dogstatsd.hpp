#pragma once

#include <string>
#include <vector>
#include <mutex>

namespace trace {

const string default_dog_host = "localhost";
const uint32_t default_dog_port = 8125;
const uint32_t max_udp_payload = 65467;
const uint32_t max_buff_cmds = 100;

class Dogstatsd {
public:
    Dogstatsd();
    Dogstatsd(bool);
    Dogstatsd(string, uint32_t, bool);
    ~Dogstatsd();

    int gauge( string, double, std::vector<string>, double );
    int count( string, int64_t, std::vector<string>, double );
    int histogram( string, double, std::vector<string>, double );
    int incr( string, std::vector<string>, double );
    int decr( string, std::vector<string>, double );
    int set( string, string, std::vector<string>, double );
protected:
    string host;
    uint32_t port;
    bool buffered;
    std::vector<string> cmd_buffer;
    int send( string, string, std::vector<string>, double );
    int flush();
private:
    int _sockfd;
    std::mutex _buff_mutex
    struct sockaddr_in _client;
    struct sockaddr_in _server;

    string format( string, string, std::vector<string>, double );

}


}
