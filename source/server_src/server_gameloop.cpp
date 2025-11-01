#include "server_gameloop.h"

#include <chrono>
#include <thread>

GameLoop::GameLoop(gameLoopQueue& queue, ClientsRegistry& registry):
        queue(queue), registry(registry) {

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};   // sin gravedad
    this->world = b2CreateWorld(&worldDef);

}
const float timeStep = 1.0f / 60.0f; //cuánto tiempo avanza el mundo en esa llamada.
const int subStepCount = 4; //por cada timeStep resuelve problemas 4 veces mas rapido o algo asi
//sobre todo para hacer calculos de colisiones

void GameLoop::run() {
    try {
        while (should_keep_running()) {
            processCmds();

            b2World_Step(this->world, timeStep,  subStepCount);

            broadcastCarSnapshots();

            std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
        }
    } catch (const std::exception& e) {
        std::cerr << "[GameLoop] fatal: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[GameLoop] fatal: unknown\n";
    }
}


void GameLoop::processCmds() {
    std::list<Cmd> to_process = emptyQueue();
    for (Cmd& cmd: to_process) {
        if (cmd.msg->type() == Opcode::Movement){
            std::cout << "aca en el gameloop movement\n";
            movementHandler(cmd);
        }
    }
}

void GameLoop::movementHandler(Cmd& cmd) {
    auto it = cars.find(cmd.client_id);
    // creo auto si no existe -> PARA TESTEAR
    if (it == cars.end()) {
        // separo autos 3m en X para que no spawneen superpuestos
        b2Vec2 spawn = { 3.0f * static_cast<float>(cars.size()), 0.0f };
        float angle  = 0.0f;

        // try_emplace construye el Car in-place
        auto [insIt, inserted] =
                cars.try_emplace(cmd.client_id, cmd.client_id, this->world, spawn, angle);
        it = insIt;
        if (!inserted) {
            return;
        }
    }
    const auto& mv = dynamic_cast<const MoveMsg&>(*cmd.msg);
    it->second.applyControlsToBody(mv, timeStep);

}


void GameLoop::broadcastCarSnapshots() {
    for (auto& [id, car] : cars) {
        PlayerState ps = car.snapshotState();
        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerState>(std::move(ps)));
        registry.broadcast(base); // todos los jugadores quieren saber tu posicion
    }
}


std::list<Cmd> GameLoop::emptyQueue() {
    std::list<Cmd> cmd_list;
    Cmd cmd_aux;

    while (queue.try_pop(cmd_aux)) {
        cmd_list.push_back(std::move(cmd_aux));
    }

    return cmd_list;
}

void GameLoop::stop() {
    Thread::stop();
    try {
        queue.close();
    } catch (...) {}
}

GameLoop::~GameLoop() {
    if (world.index1) {
        b2DestroyWorld(this->world);
        this->world = {0};
    }
}