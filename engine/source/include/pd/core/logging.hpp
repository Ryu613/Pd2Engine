#pragma once

#include <iostream>

#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "fmt/format.h"

namespace pd {
namespace log {
static std::shared_ptr<spdlog::logger> logger = nullptr;
inline void init() {
  if (logger) {
    return;
  }
  try {
    // console
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::debug);
    console_sink->set_pattern(
        "[%Y-%m-%d %H:%M:%S.%e]\033[36m[p-%P][t-%t]\033[35m[%n]\033[0m%^[%l]%$ %v");

    // file
    auto max_size = 1048576 * 5;
    auto max_files = 3;
    auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        "logs/engine.log", max_size, max_files);
    file_sink->set_level(spdlog::level::debug);

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

// create logger
#ifndef NDEBUG
    logger = std::make_shared<spdlog::logger>("pd2", sinks.begin(), sinks.end());
    logger->set_level(spdlog::level::debug);
#else
    spdlog::init_thread_pool(8192, 1);
    logger = std::make_shared<spdlog::async_logger>("pd2", sinks.begin(), sinks.end(),
                                                    spdlog::thread_pool(),
                                                    spdlog::async_overflow_policy::block);
    logger->set_level(spdlog::level::info);
#endif
    spdlog::set_default_logger(logger);

    // err handler
    spdlog::set_error_handler(
        [](const std::string& msg) { std::cerr << "SPDLOG ERROR:" << msg << "\n"; });

  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << "Log initialization failed: " << ex.what() << "\n";
  }
}

inline std::shared_ptr<spdlog::logger> get() {
  if (!logger) {
    init();
  }
  return logger;
}

inline void shutdown() noexcept {
  try {
    logger.reset();
  } catch (const spdlog::spdlog_ex& ex) {
    std::cerr << " log destroy failed!" << ex.what() << "\n";
  }
}

// 对enum打印枚举名
template <typename Enum>
  requires std::is_enum_v<Enum>
struct fmt::formatter<Enum> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  auto format(Enum e, format_context& ctx) const {
    return fmt::format_to(ctx.out(), "{}", GetEnumName(e));
  }
};

template <typename Str, typename... Args>
inline void trace(Str&& str, Args&&... args) {
  get()->trace(fmt::runtime(std::forward<Str>(str)), std::forward<Args>(args)...);
}

template <typename Str, typename... Args>
inline void debug(Str&& str, Args&&... args) {
  get()->debug(fmt::runtime(std::forward<Str>(str)), std::forward<Args>(args)...);
}

template <typename Str, typename... Args>
inline void info(Str&& str, Args&&... args) {
  get()->info(fmt::runtime(std::forward<Str>(str)), std::forward<Args>(args)...);
}

template <typename Str, typename... Args>
inline void warn(Str&& str, Args&&... args) {
  get()->warn(fmt::runtime(std::forward<Str>(str)), std::forward<Args>(args)...);
}

template <typename Str, typename... Args>
inline void error(Str&& str, Args&&... args) {
  get()->error(fmt::runtime(std::forward<Str>(str)), std::forward<Args>(args)...);
}

inline void logo() {
  info(R"(
 ____         _                    _   ____                          
|  _ \  _ __ (_) _ __ ___    __ _ | | |  _ \   __ _ __      __ _ __  
| |_) || '__|| || '_ ` _ \  / _` || | | | | | / _` |\ \ /\ / /| '_ \ 
|  __/ | |   | || | | | | || (_| || | | |_| || (_| | \ V  V / | | | |
|_|    |_|   |_||_| |_| |_| \__,_||_| |____/  \__,_|  \_/\_/  |_| |_|
)");
}
}  // namespace log
}  // namespace pd