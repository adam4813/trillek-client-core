#include "systems/transform-update-system.hpp"
#include "transform.hpp"
#include "ecs-state-system.hpp"

namespace trillek {

std::once_flag TransformUpdateSystem::only_one;
std::shared_ptr<TransformUpdateSystem> TransformUpdateSystem::instance = nullptr;

bool TransformUpdateSystem::Serialize() {
    for (auto entity_source : this->source_nodes) {
        auto entity_transform = ECSStateSystem<Transform>::GetState(entity_source.first);
        if (!entity_transform) {
            continue;
        }
        glm::vec3 translation = entity_transform->GetTranslation();
        glm::vec3 rotation = entity_transform->GetRotation();
        glm::vec3 scale = entity_transform->GetScale();

        // Check if the source node was saved.
        auto source = entity_source.second;
        if (!source) {
            // TODO get a new source/destination node from the JSON system
            continue;
        }
        auto& pos_element = (*source)["position"];
        pos_element["x"] = translation.x;
        pos_element["y"] = translation.y;
        pos_element["z"] = translation.z;

        auto& rot_element = (*source)["rotation"];
        rot_element["radians"] = true;
        rot_element["x"] = rotation.x;
        rot_element["y"] = rotation.y;
        rot_element["z"] = rotation.z;

        auto& scale_element = (*source)["scale"];
        scale_element["x"] = scale.x;
        scale_element["y"] = scale.y;
        scale_element["z"] = scale.z;
    }

    return true;
}

//  "transforms": {
//      "0": {
//          "position": {
//              "x": "0.0f",
//              "y" : "0.0f",
//              "z" : "0.0f"
//         },
//         "rotation" : {
//              "x": "0.0f",
//              "y" : "0.0f",
//              "z" : "0.0f"
//          },
//          "scale" : {
//              "x": "0.0f",
//              "y" : "0.0f",
//              "z" : "0.0f"
//          }
//      }
//  }
bool TransformUpdateSystem::Parse(rapidjson::Value& node) {
    if (node.IsObject()) {
        // Iterate over the entity ids.
        for (auto entity_itr = node.MemberBegin(); entity_itr != node.MemberEnd(); ++entity_itr) {
            if (entity_itr->value.IsObject()) {
                unsigned int entity_id = atoi(entity_itr->name.GetString());
                auto entity_transform = std::make_shared<Transform>(entity_id);

                if (entity_itr->value.HasMember("position")) {
                    auto& element = entity_itr->value["position"];

                    double x = 0.0f, y = 0.0f, z = 0.0f;
                    if (element.HasMember("x") && element["x"].IsNumber()) {
                        x = element["x"].GetDouble();
                    }
                    if (element.HasMember("y") && element["y"].IsNumber()) {
                        y = element["y"].GetDouble();
                    }
                    if (element.HasMember("z") && element["z"].IsNumber()) {
                        z = element["z"].GetDouble();
                    }

                    entity_transform->SetTranslation(glm::vec3(x, y, z));
                }
                if (entity_itr->value.HasMember("rotation")) {
                    auto& element = entity_itr->value["rotation"];

                    bool in_radians = false;

                    if (element.HasMember("radians") && element["radians"].IsBool()) {
                        in_radians = element["radians"].GetBool();
                    }

                    double x = 0.0f, y = 0.0f, z = 0.0f;
                    if (element.HasMember("x") && element["x"].IsNumber()) {
                        x = element["x"].GetDouble();
                        if (!in_radians) {
                            x = glm::radians(x);
                        }
                    }
                    if (element.HasMember("y") && element["y"].IsNumber()) {
                        y = element["y"].GetDouble();
                        if (!in_radians) {
                            y = glm::radians(y);
                        }
                    }
                    if (element.HasMember("z") && element["z"].IsNumber()) {
                        z = element["z"].GetDouble();
                        if (!in_radians) {
                            z = glm::radians(z);
                        }
                    }

                    entity_transform->SetRotation(glm::vec3(x, y, z));
                }
                if (entity_itr->value.HasMember("scale")) {
                    auto& element = entity_itr->value["scale"];

                    double x = 0.0f, y = 0.0f, z = 0.0f;
                    if (element.HasMember("x") && element["x"].IsNumber()) {
                        x = element["x"].GetDouble();
                    }
                    if (element.HasMember("y") && element["y"].IsNumber()) {
                        y = element["y"].GetDouble();
                    }
                    if (element.HasMember("z") && element["z"].IsNumber()) {
                        z = element["z"].GetDouble();
                    }

                    entity_transform->SetScale(glm::vec3(x, y, z));
                }
                this->source_nodes[entity_id] = &entity_itr->value;
                ECSStateSystem<Transform>::SetState(entity_id, entity_transform);
            }
        }

        return true;
    }

    return false;
}

} // End of trillek
