//
// Created by gautham on 13/8/18.
//

#include <iostream>
#include <queue>
#include <string>
#include <cassert>

struct Alphabet {
  Alphabet() : name('\0'), value(-1) {}
  Alphabet(char name, int value) : name(name), value(value) {}

  bool operator<(const Alphabet &other) const {
    return value == other.value ? name > other.name : value > other.value;
  }

  char name;
  int value;
};

class Numerology {
 public:
  Numerology();
  std::string GetName(std::size_t max_length);

 private:
  void HandleAlphabet(std::priority_queue<Alphabet> &alphabets,
                      std::size_t i,
                      std::string &name,
                      int &num_left,
                      int max_num_left);

  const int max_vowels_;
  const int max_consonants_;
  std::priority_queue<Alphabet> vowels_;
  std::priority_queue<Alphabet> consonants_;
};

Numerology::Numerology() : max_consonants_(5), max_vowels_(21) {
  for (int c = 'A', i = 0; c < 'A' + 26; ++c, ++i) {
    switch (static_cast<char>(c)) {
      case 'A':
      case 'E':
      case 'I':
      case 'O':
      case 'U':vowels_.emplace(static_cast<char>(c), (i % 9) + 1);
        break;

      default:consonants_.emplace(static_cast<char>(c), (i % 9) + 1);
    }
  }
}

std::string Numerology::GetName(std::size_t max_length) {
  int num_vowels_left = max_vowels_;
  int num_consonants_left = max_consonants_;
  std::string name(max_length, 'x');

  for (std::size_t i = 0; i < max_length; ++i) {
    if ((i + 1) & 1) {
      HandleAlphabet(vowels_, i, name, num_vowels_left, max_vowels_);
    } else {
      HandleAlphabet(consonants_, i, name, num_consonants_left, max_consonants_);
    }
  }

  return name;
}

void Numerology::HandleAlphabet(std::priority_queue<Alphabet> &alphabets,
                                std::size_t i,
                                std::string &name,
                                int &num_left,
                                const int max_num_left) {
  assert(i < name.length());
  assert(!alphabets.empty());
  assert(num_left > 0);

  name[i] = alphabets.top().name;
  --num_left;

  if (num_left == 0) {
    num_left = max_num_left;
    alphabets.pop();
  }
}

int main() {
  int n = 0;
  std::size_t max_length = 0;

  std::cin >> n;
  for (int i = 1; i <= n; ++i) {
    std::cin >> max_length;
    std::cout << "Case " << i << ": " << Numerology().GetName(max_length) << std::endl;
  }

  return 0;
}