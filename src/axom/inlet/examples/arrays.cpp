#include <iostream>
#include <unordered_map>

#include "axom/inlet.hpp"

void example() {
  auto lr = std::make_shared<axom::inlet::LuaReader>();
  lr->parseString("values = { [1] = 'start', [2] = 'stop', [3] = 'pause' }");
  axom::sidre::DataStore ds;
  auto inlet = std::make_shared<axom::inlet::Inlet>(lr, ds.getRoot());
  auto vals = inlet->getGlobalTable()->addStringArray("values");
  vals->registerVerifier([&]() -> bool {
    std::unordered_map<int,std::string> map;
    if (!vals->getStringArray(map)) {
      std::cout << "Error: Array not found\n";
    }
    return map.size() == 3;
  });

  // We expect verfication to pass since values array has 3 elements
  inlet->verify() ? std::cout << "Verification passed\n"
                  : std::cout << "Verification failed\n";

}

int main() {
  example();
}
