#pragma once
#include "types.hpp"
#include <span>


std::span<const MonsterDef> allMonstersDefs();
const MonsterDef& monsterDef(MonsterKind kind);
