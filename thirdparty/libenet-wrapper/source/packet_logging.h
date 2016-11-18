#pragma once
#ifndef PACKET_LOGGING_H
#define PACKET_LOGGING_H

#include <string>
#include "pandabase.h"

BEGIN_PUBLISH

uint64_t get_time();

void set_context(const std::string& context);
void set_simulation_time(float simulation_time);

void log_sent_packet(const std::string& bytes, size_t channel, bool reliable);
void log_recv_packet(const std::string& bytes, size_t channel);

void add_to_packet_log(const std::string& content);

END_PUBLISH

#endif
