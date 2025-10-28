#include "server_gameloop.h"

#include <chrono>
#include <thread>

GameLoop::GameLoop(serv_types::gameLoopQueue& queue, ClientsRegistry& registry):
        queue(queue), registry(registry) {}

void GameLoop::run() {
    try {
        while (should_keep_running()) {
            processTrun();
            std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
        }
    } catch (const std::exception& e) {
        std::cerr << "[GameLoop] fatal: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[GameLoop] fatal: unknown\n";
    }
}


void GameLoop::processTrun() {
    std::list<constants::Cmd> to_process = emptyQueue();
    constants::CliMsg msg;
    for (constants::Cmd& cmd: to_process) {  // paso 1, proceso comandos activando nitros
        if (cmd.type == constants::Opcode::Movement){
            std::cout << "aca en el gameloop movement\n";
        }
        if (cmd.type != constants::Opcode::Nitro)
            continue;  // siempre false pero podría hacer un switch acá por ej

        auto it = nitros.find(cmd.client_id);
        if (it == nitros.end()) {
            nitros.emplace(cmd.client_id, NITRO_TICKS);
            msg.event_type = constants::Opcode::NitroON;
            msg.cars_with_nitro = static_cast<constants::Cars_W_Nitro>(nitros.size());
            registry.broadcast(msg);
            printer.printNitroON();
        }
    }
    for (auto it = nitros.begin();
         it != nitros.end();) {  // paso 2, simulo iteracion desactivando nitros
        it->second -= 1;
        if (it->second == 0) {
            it = nitros.erase(it);
            msg.event_type = constants::Opcode::NitroOFF;
            msg.cars_with_nitro = static_cast<constants::Cars_W_Nitro>(nitros.size());
            registry.broadcast(msg);
            printer.printNitroOFF();
        } else {
            ++it;
        }
    }
}

std::list<constants::Cmd> GameLoop::emptyQueue() {
    std::list<constants::Cmd> cmd_list;
    constants::Cmd cmd_aux;

    while (queue.try_pop(cmd_aux)) {
        cmd_list.push_back(cmd_aux);
    }

    return cmd_list;
}

void GameLoop::stop() {
    Thread::stop();
    try {
        queue.close();
    } catch (...) {}
}
