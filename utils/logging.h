#pragma once

#include <string>

#ifdef ENABLE_LOGGING
#define LOG(...) log_strings(__VA_ARGS__);
#define LOG_HLINE() std::cout << std::string(80, '-') << std::endl;
#define LOG_DOUBLE_HLINE() std::cout << std::string(80, '=') << std::endl;
#define LOG_START()                                                                                \
    LOG_DOUBLE_HLINE();                                                                            \
    LOG("Starting Program");                                                                       \
    LOG_HLINE();
#define LOG_DONE()                                                                                 \
    LOG("\n\nDone.");                                                                              \
    LOG_DOUBLE_HLINE();
#define LOG_TITLE(...)                                                                             \
    LOG_HLINE();                                                                                   \
    LOG(__VA_ARGS__);                                                                              \
    LOG_HLINE();

#define WARN(...) LOG("[WARNING]: ", __VA_ARGS__);
#define ERROR(...) LOG("[ERROR]: ", __VA_ARGS__);

#else
#define LOG(...)
#define LOG_HLINE()
#define LOG_DOUBLE_HLINE()
#define LOG_START()
#define LOG_DONE()
#define LOG_TITLE(...)

#define WARN(...)
#define ERROR(...)

#endif

template <typename T> void log_strings(T&& str) { std::cout << str << std::endl; }
template <typename T, typename... Args> void log_strings(T&& str, Args&&... args)
{
    std::cout << str;
    log_strings(std::forward<Args>(args)...);
}
