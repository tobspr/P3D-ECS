#pragma once
#ifndef PACKET_LOGGING_H
#define PACKET_LOGGING_H

#include <string>

void log_sent_packet(const std::string& context, const std::string& bytes, size_t channel, bool reliable);
void log_recv_packet(const std::string& context, const std::string& bytes, size_t channel);

#endif
