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
    class ParameterBuilder;

    /** A simple command line argument parser. */
    // TDOO: Support an argument builder interface.
    class ArgParser
    {
    public:
        /** Parse the given command line arguments. */
        // TODO: I think the results of parsing should be returned as a result object instead of stored in the
        //       ArgParser as state.
        void parse(int argc, const char** argv);

        /** Parse the given command line arguments. */
        void parse(const std::vector<const std::string>& args);

        /** Add a flag parameter that does not take any values and returns a builder for further configuration. */
        ParameterBuilder addFlagParameter(const std::string& flagName);

        /** Add a parameter that expects a value and returns a builder for further configuration. */
        ParameterBuilder addParameter(const std::string& name);

        /** Check if a parameter was parsed. */
        bool didParse(const std::string& parameterName) const;

        /** Check if a parameter flag was parsed. */
        bool isFlagSet(const std::string& flagName) const;

        /** Get the arguments associated with a parameter. */
        const std::vector<std::string>& parameterArguments(const std::string& parameterName) const;

    public:
        /** Convert the traditional argc/argv parameters to a list of strings. */
        static std::vector<std::string> ArgcvToVector(int argc, const char** argv);

    private:
        void parseLongName(const std::string& parameterName);

    private:
        std::vector<std::string> argsToParse_;
        size_t nextArgIndex_ = 0;

    public:

        struct parameter_t
        {
            using parameter_callback_t = std::function<void(void)>;
            using argument_callback_t = std::function<void(const std::string&)>;

            char shortName;
            std::string longName;
            std::string description;
            std::vector<std::string> arguments;
            size_t expectedArgumentCount = 0;
            bool isSet = false;
            bool isFlag = false;
            parameter_callback_t onParam;
            argument_callback_t onArgument;
        };

        // TODO: Make a map std longName => parameterName (or longName => parameter_t)
        //       to support parameter names with a different/reconfigured long name.
        std::map<std::string, std::unique_ptr<parameter_t>> parameters_;
    };

    /** Configures a command line parameter using the builder pattern. */
    class ParameterBuilder
    {
    public:
        ParameterBuilder(ArgParser::parameter_t* paramToBuild);

        ParameterBuilder& shortName(char name);
        ParameterBuilder& longName(const std::string& name);
        ParameterBuilder& description(const std::string& description);
        ParameterBuilder& expectedArgumentCount(size_t count);
        ParameterBuilder& onParam(ArgParser::parameter_t::parameter_callback_t&& callback);
        ParameterBuilder& onArgument(ArgParser::parameter_t::argument_callback_t&& callback);
        ParameterBuilder& isFlag(bool isFlag);

        ParameterBuilder& bindString(std::string* binding);
        ParameterBuilder& bindInt(int* binding);
        ParameterBuilder& bindSize(size_t* binding);

    private:
        ArgParser::parameter_t* param_ = nullptr;
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