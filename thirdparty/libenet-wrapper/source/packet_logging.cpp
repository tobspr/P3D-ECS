
#include "packet_logging.h"

#include <stdint.h>
#include <chrono>
#include <iomanip>

#include <iostream>
#include <fstream>

using namespace std::chrono;

uint64_t get_time() {
    nanoseconds ns = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());
    return ns.count();
}

std::string get_output_name(const std::string& context) {
    return "packets-" + context + ".log";
}

void do_log_packet(const std::string& prefix, const std::string& context, const std::string& bytes, size_t channel, bool reliable) {
    std::cout << prefix << ": Packet[size=]" << bytes.size() << ", data='"
                  << bytes << "', channel=" << channel << "]" << std::endl;

    ::std::ofstream handle(get_output_name(context), ::std::ios_base::app);
    handle << get_time() << ": " << prefix << " packet, reliable=" << reliable << ", size=" << bytes.size() << ", data=";
    for (char c : bytes) {
        handle << c; //        handle << std::hex << std::setw(2) << (int)c << std::dec << " ";
    }
    handle << "\n";
}

void log_sent_packet(const std::string& context, const std::string& bytes, size_t channel, bool reliable) {
    do_log_packet("sent", context, bytes, channel, reliable);
}

void log_recv_packet(const std::string& context, const std::string& bytes, size_t channel) {
    do_log_packet("recv", context, bytes, channel, true);
}

