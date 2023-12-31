#pragma once

#include <string>
#include <sstream>
#include <Eigen/Core>

#ifndef DISABLE_LOGGING

#define LOG(...) log_strings(__VA_ARGS__);
#define LOG_HLINE() std::cout << std::string(80, '-') << std::endl;
#define LOG_DOUBLE_HLINE() std::cout << std::string(80, '=') << std::endl;
#define LOG_START()                                                                                \
    LOG_DOUBLE_HLINE();                                                                            \
    LOG("Starting main function in ", __FILE__);                                                                       \
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

// Function to log any number of strings to stdout

template <typename T> void log_strings(T&& str) { std::cout << str << std::endl; }
template <typename T, typename... Args> void log_strings(T&& str, Args&&... args)
{
    std::cout << str;
    log_strings(std::forward<Args>(args)...);
}



// Function to convert a vector to a string
template<typename T>
std::string vectorToString(const std::vector<T>& myVector) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < myVector.size(); ++i) {
        ss << myVector[i];
        if (i < myVector.size() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}

// Overload for Eigen vectors
template<typename Derived>
std::string vectorToString(const Eigen::MatrixBase<Derived>& eigenVector) {
    static_assert(Derived::ColsAtCompileTime == 1,
                  "vectorToString only works with column vectors (1 column).");

    std::stringstream ss;
    ss << "[";
    for (Eigen::Index i = 0; i < eigenVector.rows(); ++i) {
        ss << eigenVector(i);
        if (i < eigenVector.rows() - 1) {
            ss << ", ";
        }
    }
    ss << "]";
    return ss.str();
}
