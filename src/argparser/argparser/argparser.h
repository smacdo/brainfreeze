// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>
#include <stdexcept>
#include <functional>
#include <optional>

namespace Brainfreeze
{
    /** A simple command line argument parser. */
    // TDOO: Support an argument builder interface.
    class ArgParser
    {
    public:
        using parameter_callback_t = std::function<void(void)>;
        using parameter_argument_callback_t = std::function<void(std::string_view)>;

    public:
        /** Parse the given command line arguments. */
        void parse(int argc, const char** argv);

        /** Parse the given command line arguments. */
        void parse(const std::vector<const std::string>& args);

        /** Add a command line flag with no parameters. */
        void addFlag(
            const std::string& longName,
            const std::string& description = "");

        /** Add a command line flag with no parameters. */
        void addFlag(
            char shortName,
            const std::string& longName,
            const std::string& description = "");

        /** Check if a flag was passd. */
        bool isFlagSet(const std::string& longName) const;

        /** Add a string parameter to the argument parser. */
        void addParameter(
            const std::string& longName,
            size_t expectedArgumentCount = 1,
            const std::string& description = "");

        /** Add a string parameter to the argument parser. */
        void addParameter(
            char shortName,
            const std::string& longName,
            size_t expectedArgumentCount = 1,
            const std::string& description = "");

        /** Get the arguments associated with a parameter. */
        const std::vector<std::string>& getArgumentsForParameter(const std::string& parameterName) const;

    public:
        /** Convert the traditional argc/argv parameters to a list of strings. */
        static std::vector<std::string> ArgcvToVector(int argc, const char** argv);

    private:
        void parseLongName(const std::string& parameterName);

    private:
        std::vector<std::string> argsToParse_;
        size_t nextArgIndex_ = 0;

    private:

        struct parameter_t
        {
            char shortName;
            std::string longName;
            std::string description;
            std::vector<std::string> arguments;
            size_t expectedArgumentCount = 0;
            bool isSet = false;
            parameter_callback_t onParam;
            parameter_argument_callback_t onArgument;
        };

        std::map<std::string, parameter_t> parameters_;
    };
    
    /** Argument parser exception. */
    class ArgParserException : public std::runtime_error
    {
    public:
        explicit ArgParserException(
            const std::string& message,
            const char* fileName = nullptr,
            size_t lineNumber = 0);

        const std::string& message() const noexcept { return message_; }
        const char* fileName() const noexcept { return fileName_; }
        const size_t lineNumber() const noexcept { return lineNumber_; }

    private:
        static std::string format(
            const std::string& message,
            const char* fileName,
            size_t lineNumber);

        std::string message_;
        const char* fileName_ = nullptr;
        size_t lineNumber_ = 0;
    };

    class UnknownParameterLongNameException : public ArgParserException
    {
    public:
        explicit UnknownParameterLongNameException(
            const std::string& longName,
            const char* fileName = nullptr,
            size_t lineNumber = 0);

    private:
        static std::string format(const std::string& longName);
    };

    class ExpectedParameterArgumentMissingException : public ArgParserException
    {
    public:
        explicit ExpectedParameterArgumentMissingException(
            const std::string& parameterName,
            size_t argumentIndex,
            size_t totalExpectedArguments,
            const char* fileName = nullptr,
            size_t lineNumber = 0);

    private:
        static std::string format(
            const std::string& longName,
            size_t argumentIndex,
            size_t totalExpectedArguments);
    };
}