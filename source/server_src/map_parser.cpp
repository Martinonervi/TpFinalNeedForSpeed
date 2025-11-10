#include "map_parser.h"

//testeado, lo que si tendria que definir las coordenadas de los edificios.
// Y luego pasarlo a archivo yaml

MapData MapParser::load(const std::string& path) {
    MapData data;
    data.city = "Liberty City";


    //struct BuildingConfig(x, y, w, h, angle)
    //data.buildings.push_back({15, 15, 5, 5, 0}); // -> ESTE FUE EL TESTEADO
    //data.buildings.push_back({500, 350, 100, 100, 0});

    //struct CheckpointConfig(id, kind, x1, y1, x2, y2)
    data.checkpoints.push_back({0, CheckpointKind::Start, 3,4, 6, 4});

    return data;
}

/*
#include <yaml-cpp/yaml.h>

MapData MapParser::load(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);

    MapData data;

    if (root["city"]) {
        data.city = root["city"].as<std::string>();
    }


    if (root["buildings"]) {
        for (auto b : root["buildings"]) {
            BuildingConfig cfg;
            cfg.x = b["x"].as<float>();
            cfg.y = b["y"].as<float>();
            cfg.w = b["w"].as<float>();
            cfg.h = b["h"].as<float>();
            cfg.angle = b["angle"].as<float>();
            data.buildings.push_back(cfg);
        }
    }


    if (root["route"] && root["route"]["checkpoints"]) {
        for (auto cpNode : root["route"]["checkpoints"]) {
            CheckpointConfig cp;
            cp.id = cpNode["id"].as<int>();

            std::string kindStr = cpNode["kind"].as<std::string>();
            if (kindStr == "start")      cp.kind = CheckpointKind::Start;
            else if (kindStr == "finish") cp.kind = CheckpointKind::Finish;
            else                          cp.kind = CheckpointKind::Normal;

            cp.x1 = cpNode["x1"].as<float>();
            cp.y1 = cpNode["y1"].as<float>();
            cp.x2 = cpNode["x2"].as<float>();
            cp.y2 = cpNode["y2"].as<float>();

            data.checkpoints.push_back(cp);
        }
    }

    return data;
}

para el cmake:
find_package(yaml-cpp REQUIRED)
target_link_libraries(server PRIVATE yaml-cpp)
*/