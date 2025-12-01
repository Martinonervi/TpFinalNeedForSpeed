// tests/tests.cpp
#define private public
#define protected public
#include "common_src/socket.h"
#undef private
#undef protected


#include <gtest/gtest.h>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>
#include <future>

#include "common_src/opcodes.h"
#include "common_src/constants.h"
#include "common_src/socket.h"

#include "client_src/client_protocol.h"
#include "server_src/server_protocol.h"

struct TcpPair {
    std::unique_ptr<Socket> a;
    std::unique_ptr<Socket> b;
};

static TcpPair make_tcp_pair() {
    int fds[2]{-1, -1};
    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0) {
        throw std::runtime_error("socketpair() failed");
    }
    try {
        auto A = std::make_unique<Socket>(fds[0]);
        auto B = std::make_unique<Socket>(fds[1]);
        A->closed = false;
        B->closed = false;
        return {std::move(A), std::move(B)};
    } catch (...) {
        if (fds[0] != -1) ::close(fds[0]);
        if (fds[1] != -1) ::close(fds[1]);
        throw;
    }
}

TEST(Protocol_ClientToServer, InitPlayer_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    const CarType car = static_cast<CarType>(2);
    const InitPlayer in("pepe", car);
    ASSERT_GT(cli.sendInitPlayer(in), 0);

    const InitPlayer out = srv.recvInitPlayer();
    EXPECT_EQ(out.getCarType(), car);
}

TEST(Protocol_ClientToServer, Move_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    const MoveMsg in(1, 0, -1, 1);
    ASSERT_GT(cli.sendClientMove(in), 0);

    ASSERT_EQ(srv.recvOpcode(), Opcode::Movement);
    const MoveMsg out = srv.recvMoveInfo();

    EXPECT_EQ(out.getAccelerate(), 1);
    EXPECT_EQ(out.getBrake(), 0);
    EXPECT_EQ(out.getSteer(), -1);
    EXPECT_EQ(out.getNitro(), 1);
}

TEST(Protocol_ClientToServer, JoinGame_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    RequestGame in(42);
    ASSERT_GT(cli.sendRequestGame(in), 0);

    ASSERT_EQ(srv.recvOpcode(), Opcode::JOIN_GAME);
    RequestGame out = srv.recvGameInfo();

    EXPECT_EQ(out.getGameID(), 42);
}

TEST(Protocol_ClientToServer, Disconnect_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    DisconnectReq in(777);
    cli.sendDisconnectReq(in);

    ASSERT_EQ(srv.recvOpcode(), Opcode::CLIENT_DISCONNECT);
    const DisconnectReq out = srv.recvDisconnectReq();

    EXPECT_EQ(out.getGameID(), 777);
}

TEST(Protocol_ClientToServer, RequestUpgrade_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    RequestUpgrade in(Upgrade::ENGINE_FORCE);
    cli.sendRequestUpgrade(in);

    ASSERT_EQ(srv.recvOpcode(), Opcode::UPGRADE_REQUEST);
    const RequestUpgrade out = srv.recvUpgradeReq();

    EXPECT_EQ(out.getUpgrade(), Upgrade::ENGINE_FORCE);
}

TEST(Protocol_ServerToClient, SendPlayerInit_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    NewPlayer sp(5, static_cast<CarType>(1), 12.34f, -56.78f, 1.57f);
    ASSERT_GT(srv.sendPlayerInit(sp), 0);

    ASSERT_EQ(cli.readActionByte(), Opcode::NEW_PLAYER);
    const SendPlayer out = cli.recvSendPlayer();

    EXPECT_EQ(out.getPlayerId(), 5);
    EXPECT_EQ(out.getCarType(), static_cast<CarType>(1));
    EXPECT_NEAR(out.getX(), 12.34f, 0.01f);
    EXPECT_NEAR(out.getY(), -56.78f, 0.01f);
    EXPECT_NEAR(out.getAngleRad(), 1.57f, 0.01f);
}

TEST(Protocol_ServerToClient, PlayerState_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    PlayerState in(9, 1.25f, 2.50f, 3.75f);
    ASSERT_GT(srv.sendPlayerState(in), 0);

    const Op opcode = cli.readActionByte();
    ASSERT_EQ(opcode, Opcode::Movement);

    const PlayerState out = cli.recvSrvMsg();
    EXPECT_EQ(out.getPlayerId(), 9);
    EXPECT_NEAR(out.getX(), 1.25f, 0.01f);
    EXPECT_NEAR(out.getY(), 2.50f, 0.01f);
    EXPECT_NEAR(out.getAngleRad(), 3.75f, 0.01f);
}

TEST(Protocol_ServerToClient, JoinGame_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    JoinGame in(true, SUCCES, 1234);
    ASSERT_GT(srv.sendGameInfo(in), 0);

    ASSERT_EQ(cli.readActionByte(), Opcode::JOIN_GAME);
    const JoinGame out = cli.recvGameInfo();

    EXPECT_TRUE(out.couldJoin());
    EXPECT_EQ(out.getExitStatus(), SUCCES);
    EXPECT_EQ(out.getGameID(), 1234);
}

TEST(Protocol_ServerToClient, GamesMetadata_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    std::vector<GameMetadata> v = {{10, 3, false}, {20, 5, true}};
    MetadataGames in(v);
    ASSERT_GT(srv.sendGames(in), 0);

    (void)cli.readActionByte();

    const MetadataGames out = cli.getMetadata();
    ASSERT_EQ(out.size(), 2u);
    EXPECT_EQ(out.getMetadata()[0].game_id, 10);
    EXPECT_EQ(out.getMetadata()[0].players, 3);
    EXPECT_FALSE(out.getMetadata()[0].started);
    EXPECT_EQ(out.getMetadata()[1].game_id, 20);
    EXPECT_EQ(out.getMetadata()[1].players, 5);
    EXPECT_TRUE(out.getMetadata()[1].started);
}

TEST(Protocol_ServerToClient, Collision_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    SrvCarHitMsg in(7, 85.5f, 14.f);
    ASSERT_GT(srv.sendCollisionEvent(in), 0);

    (void)cli.readActionByte();
    const SrvCarHitMsg out = cli.recvCollisionEvent();
    EXPECT_EQ(out.getPlayerId(), 7);
    EXPECT_NEAR(out.getCarHealth(), 85.5f, 0.01f);
    EXPECT_NEAR(out.getTotalHealth(), 14.f, 0.01f);
}

TEST(Protocol_ServerToClient, CheckpointHit_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    SrvCheckpointHitMsg in(2, 9);
    ASSERT_GT(srv.sendCheckpointHit(in), 0);

    (void)cli.readActionByte();
    const SrvCheckpointHitMsg out = cli.recvCheckpointHitEvent();
    EXPECT_EQ(out.getPlayerId(), 2);
    EXPECT_EQ(out.getCheckpointId(), 9);
}

TEST(Protocol_ServerToClient, ClientDisconnect_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    ClientDisconnect in(99);
    ASSERT_GT(srv.sendClientDisconnect(in), 0);

    (void)cli.readActionByte();
    const ClientDisconnect out = cli.recvClientDisconnect();
    EXPECT_EQ(out.getPlayerId(), 99);
}

TEST(Protocol_ServerToClient, CurrentInfo_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    SrvCurrentInfo in(5,1.5f,2.5f,0.75f,10.0f,12.34f,2,45.6f,3, 4, 6);
    ASSERT_GT(srv.sendCurrentInfo(in), 0);

    (void)cli.readActionByte();
    const SrvCurrentInfo out = cli.recvCurrentInfo();
    EXPECT_EQ(out.getNextCheckpointId(), 5);
    EXPECT_NEAR(out.getCheckX(), 1.5f, 0.01f);
    EXPECT_NEAR(out.getCheckY(), 2.5f, 0.01f);
    EXPECT_NEAR(out.getAngleHint(), 0.75f, 0.01f);
    EXPECT_NEAR(out.getDistanceToCheckpoint(), 10.0f, 0.01f);
    EXPECT_NEAR(out.getRaceTimeSeconds(), 12.34f, 0.01f);
    EXPECT_EQ(out.getRaceNumber(), 2);
    EXPECT_NEAR(out.getSpeed(), 45.6f, 0.01f);
    EXPECT_EQ(out.getTotalRaces(), 3);
    EXPECT_EQ(out.getTotalCheckpoints(), 4);
    EXPECT_EQ(out.getRanking(), 6);
}

TEST(Protocol_ServerToClient, PlayerStats_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    PlayerStats in(1, 98.76f);
    ASSERT_GT(srv.sendPlayerStats(in), 0);

    (void)cli.readActionByte();
    const PlayerStats out = cli.recvStats();
    EXPECT_EQ(out.getRacePosition(), 1);
    EXPECT_NEAR(out.getTimeSecToComplete(), 98.76f, 0.01f);
}

TEST(Protocol_ServerToClient, TimeLeft_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    TimeLeft in(7, false);
    ASSERT_GT(srv.sendTimeLeft(in), 0);

    (void)cli.readActionByte();
    const TimeLeft out = cli.recvTimeLeft();
    EXPECT_EQ(out.getTimeLeft(), 7);
    EXPECT_EQ(out.getUpgradesEnabled(), false);
}

TEST(Protocol_ServerToClient, Upgrade_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    SendUpgrade in(HEALTH, true);
    ASSERT_GT(srv.sendUpgrade(in), 0);

    (void)cli.readActionByte();
    const SendUpgrade out = cli.recvUpgrade();
    EXPECT_EQ(out.getUpgrade(), Upgrade::HEALTH);
    EXPECT_TRUE(out.couldBuy());
}

TEST(Protocol_ServerToClient, UpgradeLogic_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    std::vector<UpgradeDef> defs = {
        {Upgrade::ENGINE_FORCE, 1.5f, 0.2f},
        {Upgrade::HEALTH,       5.0f, 1.0f}
    };
    UpgradeLogic in(defs);
    ASSERT_GT(srv.sendUpgradeLogic(in), 0);

    (void)cli.readActionByte();
    const UpgradeLogic out = cli.recvUpgradeLogic();
    const auto& uds = out.getUpgrades();
    ASSERT_EQ(uds.size(), 2u);
    EXPECT_EQ(uds[0].type, Upgrade::ENGINE_FORCE);
    EXPECT_NEAR(uds[0].value, 1.5f, 0.01f);
    EXPECT_NEAR(uds[0].penaltySec, 0.2f, 0.01f);
    EXPECT_EQ(uds[1].type, Upgrade::HEALTH);
    EXPECT_NEAR(uds[1].value, 5.0f, 0.01f);
    EXPECT_NEAR(uds[1].penaltySec, 1.0f, 0.01f);
}

TEST(Protocol_ServerToClient, RecommendedPath_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    std::vector<RecommendedPoint> pts = {{1.25f, 2.5f}, {3.75f, 4.0f}};
    RecommendedPath in(pts);
    ASSERT_GT(srv.sendRecommendedPath(in), 0);

    (void)cli.readActionByte();
    const RecommendedPath out = cli.recvRecommendedPath();
    const auto& rps = out.getPath();
    ASSERT_EQ(rps.size(), 2u);
    EXPECT_NEAR(rps[0].x, 1.25f, 0.01f);
    EXPECT_NEAR(rps[0].y, 2.50f, 0.01f);
    EXPECT_NEAR(rps[1].x, 3.75f, 0.01f);
    EXPECT_NEAR(rps[1].y, 4.00f, 0.01f);
}

TEST(Protocol_ServerToClient, CarConfirmation_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    CarSelect in(true);
    ASSERT_GT(srv.sendCarConfirmation(in), 0);

    (void)cli.readActionByte();
    const CarSelect out = cli.recvCarConfirmation();
    EXPECT_TRUE(out.isSelected());
}
TEST(Protocol_ServerToClient, SendNPC_roundtrip) {
    auto pp = make_tcp_pair();
    ClientProtocol cli(*pp.a);
    ServerProtocol srv(*pp.b);

    SrvNpcSpawn sp(5, static_cast<CarType>(1), 12.34f, -56.78f, 1.57f);
    ASSERT_GT(srv.sendNpcSpawn(sp), 0);

    ASSERT_EQ(cli.readActionByte(), Opcode::NPC_SPAWN);
    const SrvNpcSpawn out = cli.recvNpcSpawn();

    EXPECT_EQ(out.getId(), 5);
    EXPECT_EQ(out.getCarType(), static_cast<CarType>(1));
    EXPECT_NEAR(out.getX(), 12.34f, 0.01f);
    EXPECT_NEAR(out.getY(), -56.78f, 0.01f);
    EXPECT_NEAR(out.getAngleRad(), 1.57f, 0.01f);
}