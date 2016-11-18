
#include "packet_logging.h"

#include <stdint.h>
#include <chrono>
#include <iomanip>

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std::chrono;

// Globals
struct globals {
  static float simulation_time;
  static std::string context;
  static std::ofstream log_file_handle;
};

float globals::simulation_time = 0.0;
std::string globals::context = "nocontext";
std::ofstream globals::log_file_handle;

uint64_t
get_time() {
  nanoseconds ns = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());
  return ns.count();
}

std::string
get_output_name() {
  return "packets-" + globals::context + ".log";
}

void
set_context(const std::string& context) {
  globals::context = context;
  globals::log_file_handle = ofstream(get_output_name());
}

void
set_simulation_time(float simulation_time) {
  ::globals::simulation_time = simulation_time;
}

std::string
get_log_prefix() {
  size_t ms = (size_t)(globals::simulation_time * 1000.0);
  std::string ms_str = to_string(ms);
  return "[" + std::string(6 - std::min<size_t>(6, ms_str.size()), ' ') + ms_str + "] ";
}

void
add_to_packet_log(const std::string& content) {
  std::string prefix = get_log_prefix();
  globals::log_file_handle << get_time() << " " << prefix << content << "\n";
}

void
do_log_packet(const std::string& sent_or_recv, const std::string& bytes, size_t channel,
              bool reliable) {
  add_to_packet_log(sent_or_recv + " packet, data=" + bytes + ", channel=" + to_string(channel));
}

void
log_sent_packet(const std::string& bytes, size_t channel, bool reliable) {
  do_log_packet("sent", bytes, channel, reliable);
}

void
log_recv_packet(const std::string& bytes, size_t channel) {
  do_log_packet("recv", bytes, channel, true);
}
