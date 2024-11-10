#include <chrono>
#include <iostream>
#include <random>
#include <regex>

int64_t getRandomNumber(int64_t min, int64_t max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(min, max - 1);
  return dis(gen);
}

int main() {
  int minValue = 10;
  int maxValue = 50;

  int randomNumber = getRandomNumber(minValue, maxValue);

  std::cout << "Random number in [" << minValue << ", " << maxValue << "): " << randomNumber << std::endl;

  std::string input = "Hello,World,This,Is,C++";
  std::string delimiter = ",";

  std::regex re(delimiter);

  // Construct the range using std::sregex_token_iterator
  std::vector<std::string> result(std::sregex_token_iterator(input.begin(), input.end(), re, -1),
                                  std::sregex_token_iterator());

  //   for (const auto &token : result) {
  //     std::cout << token << std::endl;
  //   }

  for (auto it = result.begin(); it != result.end(); ++it) {
    std::cout << *it << std::endl;
  }

  return 0;

  return 0;
}
