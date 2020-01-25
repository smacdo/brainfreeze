// Copyright 2009-2020, Scott MacDonald.
#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <stdexcept>
#include <functional>
#include <optional>

namespace Brainfreeze::ArgParsing
{
    using parsed_callback_t = std::function<void(void)>;
    using argument_callback_t = std::function<void(const std::string&)>;

    class OptionBuilder;
    class OptionDesc;
    class OptionState;

    /** A simple command line argument parser. */
    class ArgParser
    {
    public:
        /** Parse the given command line arguments. */
        // TODO: I think the results of parsing should be returned as a result object instead of stored in the
        //       ArgParser as state.
        void parse(int argc, const char** argv);

        /** Parse the given command line arguments. */
        void parse(const std::vector<const std::string>& args);

        /** Adds an option to the parser and returns a builder to further configure the option. */
        OptionBuilder addOption(const std::string& optionName);

        void registerOption(const std::string& name, const OptionDesc& desc);

    public:

        /** Get an option by name. */
        // TODO: Move this to a results object returned by parse.
        const OptionState& findOption(const std::string& optionName) const;

    public:
        /** Convert the traditional argc/argv parameters to a list of strings. */
        static std::vector<std::string> ArgcvToVector(int argc, const char** argv);

    private:
        void parseLongName(const std::string& longName);

        /** Try to find an option by name. */
        bool tryFindOption(
            const std::string& optionName,
            const OptionState** optionState) const;

        /** Try to find an option by name. */
        bool tryFindOption(
            const std::string& optionName,
            OptionState** optionState);

        /** Try to find option state by long name. */
        bool tryFindOptionByLongName(
            const std::string& longName,
            const OptionState** optionState) const;

        /** Try to find option state by long name. */
        bool tryFindOptionByLongName(
            const std::string& longName,
            OptionState** optionState);

        /** Get option state by long name. */
        const OptionState& findOptionByLongName(const std::string& longName) const;
        
        /** Get option state by long name. */
        OptionState& findOptionByLongName(const std::string& longName);

    private:
        // Table of configured options, keyed by name.
        std::unordered_map<std::string, std::unique_ptr<OptionState>> options_;
        std::unordered_map<std::string, std::string> longNameToOptionLUT_;
        std::unordered_map<char, std::string> shortNameToOptionLUT_;

        std::vector<std::string> argsToParse_;
        size_t nextArgIndex_ = 0;

    };

    /** Holds information for an option supported by the argparser. */
    class OptionDesc
    {
    public:

        /**
         * Get the shortname for this option. If no short name was assigned this method will
         * return '\0'.
         */
        char shortName() const { return shortName_; }

        /** Set the shortname for this option. */
        void setShortName(char c) { shortName_ = c; }

        /** Test if short name is set. */
        bool hasShortName() const { return shortName_ != '\0'; }

        /** Get the longname for this option. */
        const std::string& longName() const { return longName_; }

        /** Set the longname for this option. */
        void setLongName(const std::string& longName) { longName_ = longName; }

        /** Test if longname is set. */
        bool hasLongName() const { return longName_.size() > 0; }

        /** Get the description for this option. */
        const std::string& description() const { return description_; }

        /** Set the description for this option. */
        void setDescription(const std::string& description) { description_ = description; }

        /** Get the expected argument count for this option. */
        size_t expectedArgumentCount() const;

        /** Set the expected argument count for this option. */
        void setExpectedArgumentCount(size_t count);

        /** Get if the option expects arguments. */
        bool expectsArguments() const;

        /** Get if the number of expected arguments has been set. */
        bool didSetExpectedArgumentCount() const;

        /** Get the parsed callback for this option. */
        const parsed_callback_t& onParsed() const { return onParsed_; }

        /** Set the parsed callback for this option. */
        void setOnParsed(parsed_callback_t&& callback) { onParsed_ = std::move(callback); }

        /** Get if the parsed callback for this option is set. */
        bool hasOnParsed() const { return onParsed_ != nullptr; }

        /** Get the argument callback for this option. */
        const argument_callback_t& onArgument() const { return onArgument_; }

        /** Get if on argument callback is set. */
        bool hasOnArgument() const { return onArgument_ != nullptr; }

        /** Set the argument callback for this option. */
        void setOnArgument(argument_callback_t&& callback) { onArgument_ = std::move(callback); }

    private:
        char shortName_ = '\0';
        std::string longName_;
        std::string description_;

        std::optional<std::size_t> expectedArgumentCount_;

        parsed_callback_t onParsed_;
        argument_callback_t onArgument_;
    };

    /** State of an option added to the argparser. */
    class OptionState
    {
    public:
        OptionState(OptionDesc desc);

        /** Get option description. */
        const OptionDesc& desc() const { return desc_; }

        /** Get if the option was marked as set. */
        bool wasSet() const { return wasSet_; }

        /** Mark that the option was set. */
        void markSet(bool isSet) { wasSet_ = isSet; }

        /** Invokes the onParsed callback (if set). */
        void invokeOnParsed();

        /** Invokes the onArgument callback (if set). */
        void invokeOnArgument(const std::string& value);

        /** Get the number of arguments added to this option. */
        size_t argumentCount() const;

        /** Get how many more arguments are expected. */
        bool expectsMoreArguments() const;

        /** Get how many expected arguments are remaining. */
        size_t expectedArgumentsRemaining() const;

        /** Add an argument to the option. */
        void addArgument(const std::string& argument);

        /** Get the arguments associated with this argument. */
        const std::vector<std::string>& arguments() const;

    private:
        OptionDesc desc_;
        bool wasSet_ = false;
        std::vector<std::string> arguments_;
    };

    /** Configures a command line option using the builder pattern. */
    class OptionBuilder
    {
    public:
        OptionBuilder(std::string name, std::string longNameIn, ArgParser& parent);
        ~OptionBuilder();

        OptionBuilder& shortName(char shortName);
        OptionBuilder& longName(const std::string& longName);
        OptionBuilder& description(const std::string& description);
        OptionBuilder& expectsArguments(size_t count);
        OptionBuilder& onParsed(parsed_callback_t&& callback);
        OptionBuilder& onArgument(argument_callback_t&& callback);

        OptionBuilder& bindString(std::string* binding);
        OptionBuilder& bindInt(int* binding);
        OptionBuilder& bindSize(size_t* binding);

    private:
        ArgParser& parent_;
        OptionDesc desc_;
        std::string name_;
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
}
