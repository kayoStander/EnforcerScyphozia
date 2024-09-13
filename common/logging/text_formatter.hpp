#pragma once

#include <string>

#include "../assert.h"
#include "entry.hpp"

namespace Common::Logging {

std::string FormatLogMessage(const Entry &entry);

void PrintMessage(const Entry &entry);

void PrintColoredMessage(const Entry &entry);
} // namespace Common::Logging
