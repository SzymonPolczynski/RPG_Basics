#include <array>
#include <cassert>
#include "monsters_db.hpp"


static const MonsterDef DB[] = {
    { MonsterKind::Rat,    "Rat",    3, 1, 'r' },
    { MonsterKind::Goblin, "Goblin", 6, 2, 'g' }
};

std::span<const MonsterDef> allMonsterDefs() {
    return { DB, std::size(DB) };
}

const MonsterDef& monsterDef(MonsterKind kind) {
    const auto idx = static_cast<std::size_t>(kind);
    assert(idx < std::size(DB));
    return DB[idx];
}