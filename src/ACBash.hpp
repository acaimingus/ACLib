#pragma once

// C++ standard headers
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>

// Linux-specific C headers
#include <unistd.h>
#include <sys/wait.h>
#include <spawn.h>

extern char **environ;

namespace ACBash
{
    void fromString(std::string input)
    {
        // Arrays for storing the args (C++ and C-style)
        std::vector<std::string> args;
        std::vector<char *> c_args;

        // Check if the input is empty
        if (input.empty())
        {
            throw std::invalid_argument("Input cannot be empty");
        }

        // Put the shell in non-interactive mode as the beginning of the command and append the command
        args.push_back("sh");
        args.push_back("-c");
        args.push_back(input);

        // Convert the command and arguments to a C-style array (argv needs the command as the first element)
        for(auto &arg : args) {
            c_args.push_back(arg.data());
        }
        
        // Null-terminate the C-style array
        c_args.push_back(nullptr);

        // Execute the command using posix_spawnp
        pid_t pid;
        if(posix_spawnp(&pid, "sh", nullptr, nullptr, c_args.data(), environ) != 0){
            std::stringstream ss;
            ss << "Failed to execute command: " << strerror(errno);
            throw std::runtime_error(ss.str());
        }

        // Wait for the child to complete
        int childstat;
        waitpid(pid, &childstat, 0);
    }
}