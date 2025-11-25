#ifndef TPFINALNEEDFORSPEED_PLAYER_MANAGER_H
#define TPFINALNEEDFORSPEED_PLAYER_MANAGER_H

#include <unordered_map>
#include <unordered_set>

#include "../server_types.h"
#include "../world/entities/car.h"
#include "../world/world_manager.h"
#include "../server_client_registry.h"
#include "../world/map_parser.h"   // por SpawnPointConfig
#include "../../common_src/cli_msg/move_Info.h"
#include "../../common_src/cli_msg/init_player.h"
#include "../../common_src/srv_msg/player_state.h"
#include "../../common_src/srv_msg/send_player.h"
#include "../../common_src/srv_msg/playerstats.h"
#include "../../common_src/srv_msg/new_player.h"


class PlayerManager {
public:
    PlayerManager(WorldManager& world,
                  ClientsRegistry& registry,
                  std::unordered_map<ID, Car>& playerCars,
                  const std::vector<SpawnPointConfig>& spawnPoints, bool& raceStarted);

    // Devuelve true si se pudo crear el jugador (si había spawn)
    bool initPlayer(Cmd& cmd);

    // Aplica comandos de movimiento
    void handleMovement(Cmd& cmd, float dt);

    // Borra auto + cuerpo físico + libera spawn
    void disconnectPlayer(ID id);

    // Envía snapshot de todos los jugadores
    void broadcastSnapshots();

    // Envía stats de todos los jugadores
    void sendPlayerStats();

    // Entre carreras: limpiar info de spawns usados, etc.
    void resetForNewRace();

private:
    WorldManager& world;
    ClientsRegistry& registry;
    std::unordered_map<ID, Car>& playerCars;
    const std::vector<SpawnPointConfig>& spawnPoints;

    // manejo de spawns
    std::unordered_set<ID> usedSpawnIds;       // spawnId ocupados
    std::unordered_map<ID, ID> carToSpawnId;   // carId -> spawnId

    bool& raceStarted;
};


#endif
