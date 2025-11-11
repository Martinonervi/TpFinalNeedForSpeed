#include "map_parser.h"
#include <yaml-cpp/yaml.h>

constexpr float PIXEL_TO_METER = 1.0f / 10.0f;   // 10 px = 1 m

MapData MapParser::load(const std::string& path) {
    YAML::Node root = YAML::LoadFile(path);

    MapData data;

    if (root["city"]) {
        data.city = root["city"].as<std::string>();
    }


    if (root["buildings"]) {
        for (auto b : root["buildings"]) { // recorre la lista
            BuildingConfig cfg;
            cfg.x = b["x"].as<float>()* PIXEL_TO_METER;
            cfg.y = b["y"].as<float>()* PIXEL_TO_METER;
            cfg.w = b["w"].as<float>()* PIXEL_TO_METER;
            cfg.h = b["h"].as<float>()* PIXEL_TO_METER;
            cfg.angle = b["angle"].as<float>();
            data.buildings.push_back(cfg);
        }
    }


    if (root["route"] && root["route"]["checkpoints"]) {
        for (auto cpNode : root["route"]["checkpoints"]) { //tmb recorre la lista
            CheckpointConfig cp;
            cp.id = cpNode["id"].as<int>();

            std::string kindStr = cpNode["kind"].as<std::string>();
            if (kindStr == "start")      cp.kind = CheckpointKind::Start;
            else if (kindStr == "finish") cp.kind = CheckpointKind::Finish;
            else                          cp.kind = CheckpointKind::Normal;

            cp.x1 = cpNode["x1"].as<float>()* PIXEL_TO_METER;
            cp.y1 = cpNode["y1"].as<float>()* PIXEL_TO_METER;
            cp.x2 = cpNode["x2"].as<float>()* PIXEL_TO_METER;
            cp.y2 = cpNode["y2"].as<float>()* PIXEL_TO_METER;

            data.checkpoints.push_back(cp);
        }
    }

    return data;
}

//testeado, lo que si tendria que definir las coordenadas de los edificios.
// Y luego pasarlo a archivo yaml

MapData MapParser::loadd(const std::string& path) {
    MapData data;
    data.city = "Liberty City";


    //struct BuildingConfig(x, y, w, h, angle)
    //data.buildings.push_back({15, 15, 5, 5, 0}); // -> ESTE FUE EL TESTEADO
    //data.buildings.push_back({500, 350, 100, 100, 0});

    //struct CheckpointConfig(id, kind, x1, y1, x2, y2)
    data.checkpoints.push_back({0, CheckpointKind::Start, 3,4, 6, 4});

    return data;
}