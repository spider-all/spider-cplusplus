#include <application/request.h>

int Request::startup_info() {
  semaphore++;
  std::thread info_thread([=, this]() {
    spdlog::info("info thread is starting...");
    int checker = 0;
    while (!stopping) {
      checker++;
      if (checker == 30 * 60) {
        checker = 0;
        int64_t user_count = database->count_user();
        int64_t org_count = database->count_org();
        spdlog::info("database users count: {}", user_count);
        spdlog::info("database organizations count: {}", org_count);
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    spdlog::info("info thread stopped");
    this->semaphore--;
  });
  info_thread.detach();
  return EXIT_SUCCESS;
}
