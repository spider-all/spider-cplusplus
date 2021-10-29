#include <application/request.h>

int Request::startup_info() {
  semaphore++;
  std::thread info_thread([=]() {
    spdlog::info("Info thread is starting...");
    int checker = 0;
    while (!stopping) {
      checker++;
      if (checker == 30 * 60) {
        checker = 0;
        int64_t user_count = database->count_user();
        int64_t org_count = database->count_org();
        spdlog::info("Database have users: {}, orgs: {}", user_count, org_count);

        fort::char_table table;
        table.set_border_style(FT_DOUBLE2_STYLE);
        table << fort::header
              << "Catalog"
              << "Count" << fort::endr
              << "users"
              << user_count << fort::endr
              << "orgs"
              << org_count << fort::endr;

        table.column(1).set_cell_text_align(fort::text_align::center);

        std::cout << std::endl
                  << table.to_string() << std::endl;
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    spdlog::info("Info thread stopped");
    this->semaphore--;
  });
  info_thread.detach();
  return EXIT_SUCCESS;
}
