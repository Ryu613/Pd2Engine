#pragma once

#include <filesystem>

namespace pd {
/**
 * @brief 文件状态
 *
 */
struct FileStat {
  bool isFile;
  bool isDirectory;
  size_t size;
};

using Path = std::filesystem::path;

/**
 * @brief 文件系统抽象，提供统一的接口进行文件的读写等操作，实现类可按平台扩展
 *
 */
class FileSystem {
 public:
  FileSystem() noexcept = default;
  virtual ~FileSystem() = default;

  MOVABLE_ONLY(FileSystem);

  /**
   * @brief 获取文件状态信息
   *
   * @param path 文件路径
   * @return FileStat 文件状态信息
   */
  virtual FileStat statOfFile(const Path& path) noexcept = 0;
  /**
   * @brief 检查path代表的是文件而不是路径
   *
   * @param path
   * @return true
   * @return false
   */
  virtual bool isFile(const Path& path) noexcept = 0;
  /**
   * @brief 检查path是否是一个路径而不是文件
   *
   * @param path
   * @return true
   * @return false
   */
  virtual bool isDirectory(const Path& path) noexcept = 0;
  /**
   * @brief 检查path对应的路径或文件存在
   *
   * @param path
   * @return true
   * @return false
   */
  virtual bool exists(const Path& path) noexcept = 0;
  /**
   * @brief 根据path创建文件夹
   *
   * @param path
   * @return true
   * @return false
   */
  virtual bool createDirectory(const Path& path) noexcept = 0;

  /**
   * @brief 按块读取文件
   *
   * @param path 文件路径
   * @param offset 文件字节偏移量
   * @param count 读取长度
   * @return std::vector<uint8_t> 字符形成的列表
   */
  virtual std::vector<uint8_t> readChunk(const Path& path, size_t offset,
                                         size_t count) noexcept = 0;
  virtual bool writeFile(const Path& path, const std::vector<uint8_t>& data) noexcept = 0;
  /**
   * @brief 根据path删除文件
   *
   * @param path
   * @return true
   * @return false
   */
  virtual bool remove(const Path& path) = 0;
  /**
   * @brief 写入文件
   *
   * @param path 要写入的路径
   * @param data 要写入的字符串
   */
  void writeFile(const Path& path, const std::string& data) noexcept;

  /**
   * @brief 读取文件并转为string
   *
   * @param path
   * @return std::string
   */
  std::string readFileString(const Path& path) noexcept;

  /**
   * @brief 以二进制形式读取文件
   *
   * @param path
   * @return std::vector<uint8_t>
   */
  std::vector<uint8_t> readFileBinary(const Path& path) noexcept;
};

}  // namespace pd