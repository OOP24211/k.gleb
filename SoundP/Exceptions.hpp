#pragma once
#include <stdexcept>

class SoundProcessorException : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class ConfigError : public SoundProcessorException {
public:
    using SoundProcessorException::SoundProcessorException;
};

class ArgumentError : public SoundProcessorException {
public:
    using SoundProcessorException::SoundProcessorException;
};
