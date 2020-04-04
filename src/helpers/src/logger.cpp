#include "logger.h"
#include <fstream>
#include <cassert>

namespace helpers{
    //! Convert log level into string
    static std::string string_from_log_level(log_level level){
        const char *names[] = {
            "OFF", "ERROR", "WARNING", "INFO", "DEBUG"
        };
        auto ilevel = static_cast<int>(level);
        if (ilevel < 0 || level >= log_level::_last)
            ilevel = 0; // avoid overrun
        return names[ilevel];
    }

    //! Get current time, for printing into the log
    static std::string get_now_string(){
        using system_clock = std::chrono::system_clock;
        auto now = system_clock::now();
        auto cnow = system_clock::to_time_t(now);
        auto result = std::string{std::ctime(&cnow)};
        // ctime result always ends with '\n', we don't want it in our logs
        if (!result.empty())
            result.pop_back();
        return result;
    }
}

using namespace helpers;

//! Logger implementation
struct logger::logger_impl{
    log_level level = log_level::default_level;
    std::ofstream stream;
};

//! Get logger singleton. Throws if log cannot be accessed
logger logger::get_logger(){
    static logger singleton;
    if (!singleton.impl_){
        auto impl = std::make_shared<logger::logger_impl>();
        impl->stream.open("log.txt", std::ios_base::app);
        if (!impl->stream.is_open()){
            throw std::runtime_error{"Cannot open a log"};
        }
        singleton.impl_ = impl;
    }
    return singleton;
}

// Function is const:
//      Writing to log does not change logger state
// Message is const&:
//      Caller gets a guarantee that the string won't change

void logger::write(log_level level, const std::string &message) const {

    assert(impl_);

    if (level > impl_->level)
        return;

    impl_->stream
        << "["
        << get_now_string()
        << "]\t"
        << string_from_log_level(level) 
        << '\t'
        << message
        << std::endl;
}

log_level logger::get_level() const {
    assert(impl_);
    return impl_->level;
}

void logger::set_level(log_level level) {
    assert(impl_);
    impl_->level = level;
}