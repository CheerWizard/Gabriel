#include <ecs/component.h>

namespace gl {

    std::unordered_map<ComponentID, ComponentMeta>* ComponentMetaTable::sTable = null;

    const ComponentMeta& ComponentMetaTable::get(ComponentID componentId) {
        if (!sTable) {
            sTable = new std::unordered_map<ComponentID, ComponentMeta>();
        }
        return sTable->at(componentId);
    }

    std::unordered_map<ComponentID, ComponentMeta>& ComponentMetaTable::getTable() {
        if (!sTable) {
            sTable = new std::unordered_map<ComponentID, ComponentMeta>();
        }
        return *sTable;
    }

    std::unordered_map<ComponentID, SerializableMeta>* SerializableMetaTable::sTable = null;

    SerializableMeta* SerializableMetaTable::get(ComponentID componentId) {
        if (!sTable) {
            sTable = new std::unordered_map<ComponentID, SerializableMeta>();
        }

        if (sTable->find(componentId) == sTable->end()) {
            return null;
        }

        return &sTable->operator[](componentId);
    }

    std::unordered_map<ComponentID, SerializableMeta>& SerializableMetaTable::getTable() {
        if (!sTable) {
            sTable = new std::unordered_map<ComponentID, SerializableMeta>();
        }
        return *sTable;
    }

}