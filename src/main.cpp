#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include "YBot.hpp"
#include <fstream>

using namespace geode::prelude;

static bool g_botEnabled = false;

namespace YBot {
    void toggleBotMode(bool enabled) {
        g_botEnabled = enabled;
    }

    bool isBotEnabled() {
        return g_botEnabled;
    }

    void logDeath(float x, float y) {
        std::ofstream logFile(Mod::get()->getSaveDir() / "YBot.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << "Death at X: " << x << ", Y: " << y << std::endl;
            logFile.close();
        }
    }
}

class $modify(YBotPlayerObject, PlayerObject) {
    void destroyPlayer() {
        if (YBot::isBotEnabled()) {
            auto pos = this->getPosition();
            log::info("YBot - Died at X: {}, Y: {}", pos.x, pos.y);
            YBot::logDeath(pos.x, pos.y);

            if (auto playLayer = GameManager::sharedState()->getPlayLayer()) {
                playLayer->resetLevel();
                return;
            }
        }

        PlayerObject::destroyPlayer();
    }
};

$on_mod(Loaded) {
    Mod::get()->addCustomUI([] {
        auto menu = MenuLayer::create();
        auto toggle = CCMenuItemToggler::createWithStandardSprites(
            [](CCObject*) {
                YBot::toggleBotMode(!YBot::isBotEnabled());
                FLAlertLayer::create("YBot", YBot::isBotEnabled() ? "Bot Mode Enabled" : "Bot Mode Disabled", "OK")->show();
            },
            [] { return false; }
        );
        toggle->setPosition(100, 100);
        menu->addChild(toggle);
        return menu;
    });
}
