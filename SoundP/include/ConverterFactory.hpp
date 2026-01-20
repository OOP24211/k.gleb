#pragma once
#include <string>
#include <vector>
#include <memory>
#include "IConverter.hpp"

std::unique_ptr<IConverter> createConverter(const std::string& name, const std::vector<std::string>& args,const std::vector<std::vector<int16_t>>& extra_streams);

