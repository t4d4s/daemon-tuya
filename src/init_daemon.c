#include "init_daemon.h"

// Example from geeks4geeks.com
void init_daemon() {
    pid_t pid, sid;

    // Fork the parent process
    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Failed to fork the parent process");
        exit(EXIT_FAILURE);
    }

    // If we got a valid PID, exit the parent process
    if (pid > 0) {
        syslog(LOG_INFO, "Valid PID found.");
        exit(EXIT_SUCCESS);
    }

    // Create a new session
    sid = setsid();
    if (sid < 0) {
        syslog(LOG_ERR, "Failed to create a new session");
        exit(EXIT_FAILURE);
    }

    // Ignore terminal I/O signals
    signal(SIGHUP, SIG_IGN);

    // Fork again to ensure the daemon can't regain a terminal
    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Failed to fork again");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        syslog(LOG_INFO, "Forked successfully.");
        exit(EXIT_SUCCESS);
    }

    // Set file permissions
    umask(0);

    // Change the working directory to root
    if (chdir("/") < 0) {
        syslog(LOG_ERR, "Failed to change directory to /");
        exit(EXIT_FAILURE);
    }

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Redirect standard files to /dev/null
    int fd = open("/dev/null", O_RDWR);
    if (fd >= 0) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
}