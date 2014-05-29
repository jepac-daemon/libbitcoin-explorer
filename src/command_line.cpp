#include <iostream>
#include <string>
#include "config.hpp"
#include "command_line.hpp"

// TODO: deine and implement interface.
bool wallet_display_usage();
bool wallet_invoke(const int argc, const char* argv[]);

// localizable

void display_invalid_config(std::string& file)
{
    std::cerr << "sx: config file '" << file 
        << "' doesn't exist!" << std::endl;
}

void display_invalid_command(std::string& command)
{
    std::cerr << "sx: " << command
        << " is not a sx option or command. See 'sx --help'." << std::endl;
}

void display_usage()
{
    std::cerr << "Usage: sx COMMAND [ARGS]..." << std::endl;
    std::cerr << std::endl;
    std::cerr << "  -c, --config               "
        "Specify a config file" << std::endl;
    std::cerr << std::endl;
    std::cerr << "The sx commands are:" << std::endl;
    std::cerr << std::endl;

    // TODO: get category, subcategory and short description, sort categories.
    wallet_display_usage();

    std::cerr << std::endl;
    std::cerr << "See 'sx help COMMAND' for more information "
        "on a specific command." << std::endl;
    std::cerr << std::endl;
    std::cerr << "SX home page: <http://sx.dyne.org/>" << std::endl;
}

// not localizable

bool dispatch_invoke(const int argc, const char* argv[])
{
    auto command = std::string(argv[0]);

    if (command == "wallet")
        return wallet_invoke(argc, argv);

    display_invalid_command(command);

    return false;
}

bool dispatch_usage(std::string& command)
{
    if (command == "wallet")
        return wallet_display_usage();

    display_invalid_command(command);

    return false;
}

int invoke(const int argc, const char* argv[])
{
    // command line usage:
    // sx [-c|--config path] [[-h|--help|help command] | [command [args...]]]

    int position = 0;
    const int last = argc - 1;

    // sx
    if (position == last)
    {
        display_usage();
        return main_success;
    }

    // next token (skip process name)
    auto token = std::string(argv[++position]);

    // --config
    if (token == "-c" || token == "--config")
    {
        if (position == last)
        {
            // sx -c|--config <missing path>
            display_usage();
            return main_failure;
        }

        // next token (skip -c|--config)
        token = std::string(argv[++position]);

        if (!set_config_path(token))
        {
            display_invalid_config(token);
            return main_failure;
        }

        if (position == last)
        {
            // sx -c|--config path
            // std::cerr << "Using config file: " << token << std::endl;
            return main_success;
        }

        // next token (skip path)
        // config option can be combined with help|command
        token = std::string(argv[++position]);
    }

    // --help
    if (token == "-h" || token == "--help" || token == "help")
    {
        if (position == last)
            // sx [-c|--config path] -h|--help|help
            display_usage();
        else
        {
            // next token (skip -h|--help|help)
            token = std::string(argv[++position]);

            // sx [-c|--config path] -h|--help|help command
            if (!dispatch_usage(token))
                return main_failure;
        }

        // help option cannot be combined with command, ignore subsequent args
        return main_success;
    }

    // invoke: always set the command as the first argument
    if (!dispatch_invoke(argc - position, &argv[position]))
        return main_failure;

    return main_success;
}