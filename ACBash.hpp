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

// Environment variables
extern char **environ;

namespace ACBash
{
    std::string fromString(std::string input)
    {
        // Streams for output and error messages
        std::stringstream err;
        std::stringstream ss;

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
        for (auto &arg : args)
        {
            c_args.push_back(arg.data());
        }

        // Null-terminate the C-style array
        c_args.push_back(nullptr);

        // Create pipes for output redirection
        int out_pipe[2], err_pipe[2];
        if (pipe(out_pipe) || pipe(err_pipe))
        {
            throw std::runtime_error("Failed to create pipes");
        }

        // Create and initialize file actions
        posix_spawn_file_actions_t actions;
        posix_spawn_file_actions_init(&actions);

        // Redirect stdout and stderr to the pipes
        posix_spawn_file_actions_adddup2(&actions, out_pipe[1], STDOUT_FILENO);
        posix_spawn_file_actions_adddup2(&actions, err_pipe[1], STDERR_FILENO);

        // Close unused pipe ends
        posix_spawn_file_actions_addclose(&actions, out_pipe[0]);
        posix_spawn_file_actions_addclose(&actions, err_pipe[0]);

        // Execute the command using posix_spawnp
        pid_t pid;
        if (posix_spawnp(&pid, "sh", &actions, nullptr, c_args.data(), environ) != 0)
        {
            err << "Failed to execute command: " << strerror(errno);
            throw std::runtime_error(err.str());
        }

        // Close unused pipe ends
        close(out_pipe[1]);
        close(err_pipe[1]);

        // Read stdout
        char buffer[256];
            ssize_t bytesRead;
        while ((bytesRead = read(out_pipe[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytesRead] = '\0';
            ss << buffer;
        }
        close(out_pipe[0]);

        // Read stderr
        while ((bytesRead = read(err_pipe[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            // Null-terminate safely
            buffer[bytesRead] = '\0';
            err << buffer;
        }
        close(err_pipe[0]);

        // Clean up file actions
        posix_spawn_file_actions_destroy(&actions);

        // Wait for the child to complete
        int childstat;
        waitpid(pid, &childstat, 0);

        // Check exit status
        if (!WIFEXITED(childstat) || WEXITSTATUS(childstat))
        {
            err << "Command failed with exit code " << WEXITSTATUS(childstat) << ": " << err.str();
            throw std::runtime_error(err.str());
        }

        return ss.str();
    }
}