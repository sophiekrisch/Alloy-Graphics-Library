#ifndef TINY_PROCESS_LIBRARY_HPP_
#define TINY_PROCESS_LIBRARY_HPP_

#include <string>
#include <functional>
#include <vector>
#include <mutex>
#include <thread>
#include <mutex>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/wait.h>
#endif

///Create a new process given command and run path.
///TODO: on Windows it is harder to specify which pipes to redirect.
///Thus, at the moment, if read_stdout==nullptr, read_stderr==nullptr and open_stdin==false,
///the stdout, stderr and stdin are sent to the parent process instead.
///Compile with -DMSYS_PROCESS_USE_SH to run command using "sh -c [command]" on Windows as well.
class Process {
public:
#ifdef _WIN32
  typedef DWORD id_type; //Process id type
  typedef HANDLE fd_type; //File descriptor type
#else
  typedef pid_t id_type;
  typedef int fd_type;
#endif
private:
  class Data {
  public:
    Data();
    id_type id;
#ifdef _WIN32
    HANDLE handle;
#endif
  };
public:
  Process(const std::string &command, const std::string &path=std::string(),
          std::function<void(const char *bytes, size_t n)> read_stdout=nullptr,
          std::function<void(const char *bytes, size_t n)> read_stderr=nullptr,
          bool open_stdin=false,
          size_t buffer_size=131072);
  ~Process();
  
  ///Get the process id of the started process.
  id_type get_id();
  ///Wait until process is finished, and return exit status.
  int get_exit_status();
  ///Write to stdin.
  bool write(const char *bytes, size_t n);
  ///Write to stdin. Convenience function using write(const char *, size_t).
  bool write(const std::string &data);
  ///Close stdin. If the process takes parameters from stdin, use this to notify that all parameters have been sent.
  void close_stdin();
  
  ///Kill the process.
  void kill(bool force=false);
  ///Kill a given process id. Use kill(bool force) instead if possible.
  static void kill(id_type id, bool force=false);
protected:
  Data data;
  bool closed;
  std::mutex close_mutex;
  std::function<void(const char* bytes, size_t n)> read_stdout;
  std::function<void(const char* bytes, size_t n)> read_stderr;
  std::thread stdout_thread, stderr_thread;
  bool open_stdin;
  std::mutex stdin_mutex;
  const size_t buffer_size;
  std::unique_ptr<fd_type> stdout_fd, stderr_fd, stdin_fd;
  id_type open(const std::string &command, const std::string &path);
  void async_read();
  void close_fds();
};

#endif  // TINY_PROCESS_LIBRARY_HPP_
