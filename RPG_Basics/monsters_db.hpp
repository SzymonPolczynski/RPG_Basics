#pragma once
#include "types.hpp"
#include <span>


std::span<const MonsterDef> allMonsterDefs();
const MonsterDef& monsterDef(MonsterKind kind);
