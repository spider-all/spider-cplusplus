// #include <iostream>

// #include <prometheus/counter.h>
// #include <prometheus/exposer.h>
// #include <prometheus/registry.h>

// #include <config.h>

// #include <application.h>
// #include <database.h>

// #pragma once

// class Prome : public Application {
// private:
//   Config config;
//   Database *database;

//   int semaphore = 0; // 执行过程中的信号量
//   bool stopping = false;

//   prometheus::Exposer *exposer{};

// public:
//   Prome(Config, Database *);
//   ~Prome() override;
//   int startup() override;
// };
