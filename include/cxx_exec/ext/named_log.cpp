#include "log"
#include "name_provider.cpp"
#include "on_startup.hpp"

on_startup __name_log() {
    log_formatter = [](const string& message) { return "["+string{name()}+"] " + message; };
}
