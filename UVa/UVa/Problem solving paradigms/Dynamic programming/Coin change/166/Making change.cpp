//
// Created by Gautham Banasandra on 2018-12-23.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <string>
#include <cassert>

const auto infinity = std::numeric_limits<int>::max();

struct Denomination {
  Denomination(int value, long count) : value(value), count(count) {}

  int value;
  long count;
};

enum Denominations {
  k5,
  k10,
  k20,
  k50,
  k100,
  k200
};

class ChangeMaker {
 public:
  explicit ChangeMaker(std::size_t max_value, std::vector<Denomination> denominations);

  int Count(int change);

 protected:
  std::vector<Denomination> denominations_;

 private:
  std::vector<int> memo_;
};

ChangeMaker::ChangeMaker(std::size_t max_value, std::vector<Denomination> denominations) : denominations_(std::move(
    denominations)) {
  memo_.resize(max_value + 1, -1);
}

int ChangeMaker::Count(int change) {
  if (change < 0) {
    return infinity;
  }

  if (change == 0) {
    return 0;
  }

  auto &memo = memo_[change];
  if (memo != -1) {
    return memo;
  }

  auto min_change = infinity;
  for (auto &denomination : denominations_) {
    if (denomination.count == 0) {
      continue;
    }

    --denomination.count;
    auto b = Count(change - denomination.value);
    min_change = std::min(min_change, b);
    ++denomination.count;
  }
  return memo = min_change == infinity ? infinity : 1 + min_change;
}

struct CoinIterator {
 public:
  CoinIterator(const std::vector<Denomination> &denominations)
      : i_denomination(0), i_count(0), denominations(denominations) {}

  int Next(int offset);

 private:
  std::size_t i_denomination;
  std::size_t i_count;
  const std::vector<Denomination> &denominations;
};

int CoinIterator::Next(int offset) {
  if (i_denomination >= denominations.size()) {
    return -1;
  }

  const auto &denomination = denominations[i_denomination];
  if (i_count >= denomination.count) {
    i_count = 0;
    ++i_denomination;
    return Next(offset);
  }
  ++i_count;
  return denomination.value + offset;
}

class Customer : public ChangeMaker {
 public:
  Customer(std::size_t max_value, std::vector<Denomination> &denominations);

  CoinIterator GetCoinIterator() const { return {denominations_}; }
  int GetBudget() const { return budget_; }

 private:
  int budget_;
};

Customer::Customer(std::size_t max_value, std::vector<Denomination> &denominations) : ChangeMaker(max_value,
                                                                                                  denominations),
                                                                                      budget_(0) {
  for (const auto &denomination : denominations_) {
    budget_ += denomination.value * denomination.count;
  }
}

class Shop {
 public:
  explicit Shop(std::size_t max_value) : shop_keeper_(max_value, {
      {5, infinity},
      {10, infinity},
      {20, infinity},
      {50, infinity},
      {100, infinity},
      {200, infinity}
  }) {}

  int Buy(Customer &customer, int value);

 private:
  ChangeMaker shop_keeper_;
};

int Shop::Buy(Customer &customer, int value) {
  auto min_coins = infinity;
  for (auto tender = value, budget = customer.GetBudget(); tender <= budget; tender += 5) {
    auto coins_tendered = customer.Count(tender);
    if (coins_tendered == infinity) {
      continue;
    }

    auto receive = tender - value;
    auto coins_received = shop_keeper_.Count(receive);
    if (coins_received == infinity) {
      continue;
    }
    min_coins = std::min(min_coins, coins_tendered + coins_received);
  }
  return min_coins;
}

int main() {
  auto value = 400;
//  auto value = 315;
  std::string value_str;
  Shop shop(1000000);
  std::vector<Denomination> denominations{
//      {5, 1},
//      {10, 7},
//      {20, 7},
//      {50, 4},
      {100, 2},
      {200, 1},

/*//      {5, 1},
//      {10, 7},
      {20, 6},
      {50, 7},
      {100, 8},
//      {200, 1},*/
  };

  /*auto &num_k5 = denominations[k5].count;
  auto &num_k10 = denominations[k10].count;
  auto &num_k20 = denominations[k20].count;
  auto &num_k50 = denominations[k50].count;
  auto &num_k100 = denominations[k100].count;
  auto &num_k200 = denominations[k200].count;

  while (std::cin >> num_k5 >> num_k10 >> num_k20 >> num_k50 >> num_k100 >> num_k200,
      num_k5 || num_k10 || num_k20 || num_k50 || num_k100 || num_k200) {
    std::cin >> value_str;
    value_str.erase(value_str.begin() + value_str.find('.'));
    value = std::stoi(value_str);

    auto max_value = static_cast<std::size_t>(5 * num_k5 + 10 * num_k10 + 20 * num_k20 + 50 * num_k50 + 100 * num_k100
        + 200 * num_k200);
    Customer customer(1000000, denominations);
    std::cout.width(3);
    std::cout << std::right << shop.Buy(customer, value) << std::endl;
  }*/

  Customer customer(1000000, denominations);
  std::cout.width(3);
  std::cout << std::right << shop.Buy(customer, value) << std::endl;
  return 0;
}