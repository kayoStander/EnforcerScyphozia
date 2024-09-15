#pragma once

#include <string>

#include "../assert.h"
#include "entry.hpp"

namespace Common::Logging {

std::string FormatLogMessage(const Entry &entry);

void PrintMessage(const Entry &entry) noexcept;

void PrintColoredMessage(const Entry &entry);
} // namespace Common::Logging
