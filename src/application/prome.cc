#include <application/prome.h>

Prome::Prome(Config c, Database *db) {
  config = std::move(c);
  database = db;
}

Prome::~Prome() {
  stopping = true;

  while (true) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // run loop
    if (semaphore == 0) {
      break;
    }
  }

  spdlog::info("Prometheus expoter stopped...");
}

int Prome::startup() {
  semaphore++;
  std::thread prome_thread([=, this]() {
    exposer = new prometheus::Exposer("0.0.0.0:8080");
    auto registry = std::make_shared<prometheus::Registry>();
    auto &packet_counter = prometheus::BuildCounter()
                               .Name("observed_packets_total")
                               .Help("Number of observed packets")
                               .Register(*registry);
    auto &tcp_rx_counter = packet_counter.Add({{"protocol", "tcp"}, {"direction", "rx"}});
    tcp_rx_counter.Increment();
    exposer->RegisterCollectable(registry);
    semaphore--;
  });
  prome_thread.detach();

  return EXIT_SUCCESS;
}
