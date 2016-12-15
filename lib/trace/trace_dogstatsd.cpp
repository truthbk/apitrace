#include<arpa/inet.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<string.h>
#include <time.h>       /* time */

#include <iostream>
#include <sstream> // for ostringstream
#include <iomanip>

#include<cstdio>
#include<cstdlib>

#include "trace_dogstatsd.hpp"

namespace trace {

Dogstatsd::Dogstatsd() : Dogstatsd(false) { }
Dogstatsd::Dogstatsd(bool buffered) :
    Dogstatsd(default_dog_host, default_dog_port, buffered) { }
Dogstatsd::Dogstatsd(std::string host, uint32_t port, bool buffered){
    this->buffered = buffered;
    this->_sockfd = socket(AF_INET,SOCK_DGRAM,0); //IPv4(only)/UDP
    this->server.sin_family = AF_INET;
    this->server.sin_port = htons(port);
    this->server.sin_addr.s_addr = inet_addr(host);

    //plant a random seed
    srand(time(NULL));

}
Dogstatsd::~Dogstatsd(){
    if (this->buffered()) {
        flush();
    }
}

int Dogstatsd::gauge( std::string, double value, std::vector<std::string> tags, double rate){
    std::ostringstream statd;
    statd << std::setprecision(6) << value << "|g";
    send(name, statd.str(), tags, rate);
}
int Dogstatsd::count( std::string, int64_t value, std::vector<std::string> tags, double rate){
    std::ostringstream statd;
    statd << value << "|c";
    send(name, statd.str(), tags, rate);
}
int Dogstatsd::histogram( std::string, double value, std::vector<std::string> tags, double rate){
    std::ostringstream statd;
    statd << std::setprecision(6) << value << "|h";
    send(name, statd.str(), tags, rate);
}
int Dogstatsd::incr( std::string name, std::vector<std::string> tags, double rate){
    send(name, "1|c", tags, rate);
}
int Dogstatsd::decr( std::string name, std::vector<std::string> tags, double rate){
    send(name, "-1|c", tags, rate);
}
int Dogstatsd::set( std::string name, std::string value, std::vector<std::string> tags, double rate){
    std::ostringstream statd;
    statd << value << "|s";
    send(name, statd.str(), tags, rate);
}
int Dogstatsd::send( std::string name, std::string statd, std::vector<std::string> tags, double rate){
    int ret = 0;

    if (rate < 1 && (rate * 100) < (rand() % 100)) {
        return ret;
    }

    std::string cmd = std::string(format(name, statd, tags, rate));
    if (!buffered) {
        return sendto(_sockfd, cmd.c_str(), cmd.length(), 0, (struct sockaddr *)&_server, sizeof(m));
    }

    std::lock_guard<std::mutex> lock(_buff_mutex);
    if (cmd_buffer.size() < max_buff_cmds-1) {
        cmd_buffer.push_back(cmd);
    } else {
        cmd_buffer.push_back(cmd);
        ret = flush();
    }
    return ret;
}

//not thread-safe, call holding lock.
//optimize to be more optimal.
int Dogstatsd::flush(){
    std::ostringstream statd;
    if(!buffered) {
        return 0;
    }
    while (cmd_buffer.empty()) {
        statd << cmd_buffer.pop_back() << std::endl;
    }
    std::string buffer = std::string(statd.str());

    return sendto(_sockfd, buffer.c_str(), buffer.length(), 0, (struct sockaddr *)&_server, sizeof(m));
}
std::string Dogstatsd::format( std::string, std::string, std::vector<std::string> tags, double rate){
    std::ostringstream statd;

    statd << name << ":" << value;
    if (rate < 1) {
        statd << "|@" << rate;
    }

    if (tags.size()) {
        std::vector<string>::iterator it = tags.begin();

        statd << "|#" << *it;
        for( ; it != tags.end() ; ++it) {
            statd << "," << *it
        }
    }
	if rate < 1 {
		buf.WriteString(`|@`)
		buf.WriteString(strconv.FormatFloat(rate, 'f', -1, 64))
	}

	writeTagString(&buf, c.Tags, tags)
}

}
