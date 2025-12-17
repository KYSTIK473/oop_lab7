#include <gtest/gtest.h>
#include "../src/async_game.hpp"

TEST(AsyncGameTest, Initialization) {
    AsyncGame game;
    SUCCEED();
}

TEST(AsyncGameTest, MovementRules) {
    AsyncGame game;
    
    EXPECT_EQ(game.get_move_distance("Rogue"), 10);
    EXPECT_EQ(game.get_move_distance("Orc"), 20);
    EXPECT_EQ(game.get_move_distance("Werewolf"), 40);
    EXPECT_EQ(game.get_move_distance("Pegasus"), 30);
}

TEST(AsyncGameTest, KillDistanceRules) {
    AsyncGame game;
    
    EXPECT_EQ(game.get_kill_distance("Rogue"), 10);
    EXPECT_EQ(game.get_kill_distance("Orc"), 10);
    EXPECT_EQ(game.get_kill_distance("Werewolf"), 5);
    EXPECT_EQ(game.get_kill_distance("Pegasus"), 10);
}

TEST(AsyncGameTest, BattleRules) {
    AsyncGame game;
    
    EXPECT_TRUE(game.can_kill("Rogue", "Werewolf"));
    EXPECT_TRUE(game.can_kill("Werewolf", "Rogue"));
    EXPECT_TRUE(game.can_kill("Orc", "Rogue"));
    
    EXPECT_FALSE(game.can_kill("Rogue", "Orc"));
    EXPECT_FALSE(game.can_kill("Orc", "Werewolf"));
    EXPECT_FALSE(game.can_kill("Werewolf", "Orc"));
    EXPECT_FALSE(game.can_kill("Pegasus", "Rogue"));
    EXPECT_FALSE(game.can_kill("Pegasus", "Orc"));
    EXPECT_FALSE(game.can_kill("Pegasus", "Werewolf"));
    EXPECT_FALSE(game.can_kill("Pegasus", "Pegasus"));
}

TEST(AsyncGameTest, DiceRollRange) {
    AsyncGame game;
    
    for (int i = 0; i < 100; i++) {
        int roll = game.roll_dice();
        EXPECT_GE(roll, 1);
        EXPECT_LE(roll, 6);
    }
}

TEST(AsyncNPCTest, Creation) {
    auto npc = std::make_shared<AsyncNPC>("Test", 100, 200);
    npc->type = "Rogue";
    
    EXPECT_EQ(npc->getName(), "Test");
    EXPECT_EQ(npc->getX(), 100);
    EXPECT_EQ(npc->getY(), 200);
    EXPECT_TRUE(npc->isAlive());
    EXPECT_EQ(npc->getType(), "Rogue");
}

TEST(AsyncNPCTest, DeathState) {
    auto npc = std::make_shared<AsyncNPC>("Test", 100, 100);
    npc->type = "Orc";
    
    EXPECT_TRUE(npc->isAlive());
    npc->die();
    EXPECT_FALSE(npc->isAlive());
}

TEST(AsyncNPCTest, SetPosition) {
    auto npc = std::make_shared<AsyncNPC>("Test", 100, 100);
    npc->type = "Werewolf";
    
    npc->setPosition(200, 300);
    EXPECT_EQ(npc->getX(), 200);
    EXPECT_EQ(npc->getY(), 300);
    
    npc->setPosition(600, 600);
    EXPECT_EQ(npc->getX(), 200);
    EXPECT_EQ(npc->getY(), 300);
}

TEST(AsyncNPCTest, DistanceCalculation) {
    auto npc1 = std::make_shared<AsyncNPC>("Test1", 0, 0);
    auto npc2 = std::make_shared<AsyncNPC>("Test2", 3, 4);
    
    npc1->type = "Rogue";
    npc2->type = "Orc";
    
    EXPECT_DOUBLE_EQ(npc1->distanceTo(*npc2), 5.0);
}

TEST(AsyncNPCTest, TypeAssignment) {
    auto npc = std::make_shared<AsyncNPC>("Test", 50, 50);
    
    npc->type = "Pegasus";
    EXPECT_EQ(npc->getType(), "Pegasus");
    
    npc->type = "Rogue";
    EXPECT_EQ(npc->getType(), "Rogue");
    
    npc->type = "Orc";
    EXPECT_EQ(npc->getType(), "Orc");
    
    npc->type = "Werewolf";
    EXPECT_EQ(npc->getType(), "Werewolf");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}