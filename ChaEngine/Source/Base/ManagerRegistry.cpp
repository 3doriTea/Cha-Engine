#include "ManagerRegistry.h"

std::map<std::type_index, std::shared_ptr<cha::IManager>> cha::ManagerRegistry::registry_{};
