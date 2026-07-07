#pragma once

#include <iostream>

#include "spdlog/async.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "fmt/format.h"

namespace pd {
namespace log {

inline std::shared_ptr<spdlog::logger>& logger_storage() {
  static std::shared_ptr<spdlog::logger> instance;
  return instance;
}

inline void init() {
  static std::once_flag flag;
  std::call_once(flag, []() {
    try {
      auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e]\033[36m[p-%P][t-%t]\033[35m[%n]\033[0m%^[%l]%$ [%s:%#] %v");

      auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
          "logs/engine.log", 1048576 * 5, 3);

      std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};

#ifndef NDEBUG
      auto logger = std::make_shared<spdlog::logger>("pd2", sinks.begin(), sinks.end());
      logger->set_level(spdlog::level::debug);
#else
            spdlog::init_thread_pool(8192, 1);
            auto logger = std::make_shared<spdlog::async_logger>("pd2", sinks.begin(), sinks.end(),
                                                                 spdlog::thread_pool(),
                                                                 spdlog::async_overflow_policy::block);
            logger->set_level(spdlog::level::info);
#endif
      logger_storage() = logger;
      spdlog::set_default_logger(logger);
      spdlog::set_error_handler([](const std::string& msg) {
        // 输出到 stderr 或系统调试器
        std::cerr << "SPDLOG ERROR: " << msg << "\n";
      });
    } catch (const spdlog::spdlog_ex& ex) {
      std::cerr << "Log init failed: " << ex.what() << "\n";
    }
  });
}

inline std::shared_ptr<spdlog::logger> get() {
  init();
  return logger_storage();
}

inline void shutdown() noexcept {
  spdlog::shutdown();
  logger_storage().reset();
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

template <typename... Args>
inline void trace(fmt::format_string<Args...> fmt, Args&&... args) {
  get()->trace(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args&&... args) {
  get()->debug(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void info(fmt::format_string<Args...> fmt, Args&&... args) {
  get()->info(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args&&... args) {
  get()->warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(fmt::format_string<Args...> fmt, Args&&... args) {
  get()->error(fmt, std::forward<Args>(args)...);
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
#define LOG_TRACE(...) SPDLOG_LOGGER_TRACE(log::get().get(), __VA_ARGS__)
#define LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(log::get().get(), __VA_ARGS__)
#define LOG_INFO(...) SPDLOG_LOGGER_INFO(log::get().get(), __VA_ARGS__)
#define LOG_WARN(...) SPDLOG_LOGGER_WARN(log::get().get(), __VA_ARGS__)
#define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(log::get().get(), __VA_ARGS__)
}  // namespace pd
