cmake_minimum_required(VERSION 3.16)
project(kwin-effects-yaml)

set(KF_MIN_VERSION "6.0")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${ECM_MODULE_PATH}
)

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMSetupQtPluginMacroNames)

# KWin registers these for its in-tree effects, but KWinConfig.cmake does not
# export them (it expands PACKAGE_SETUP_KWIN_AUTOMOC_VARIABLES while KWin sets
# PACKAGE_SETUP_KWINEFFECTS_AUTOMOC_VARIABLES). Declaring them here makes automoc
# treat metadata.json as a dependency of the plugin, so editing it triggers a
# rebuild instead of silently shipping stale metadata.
ecm_setup_qtplugin_macro_names(
    JSON_ARG2
        "KWIN_EFFECT_FACTORY"
    JSON_ARG3
        "KWIN_EFFECT_FACTORY_ENABLED"
        "KWIN_EFFECT_FACTORY_SUPPORTED"
    JSON_ARG4
        "KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED"
)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    DBus
    Gui
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    KCMUtils
    WindowSystem
)

find_package(KWin REQUIRED)

find_package(KWinDBusInterface CONFIG REQUIRED)

file(WRITE "${CMAKE_BINARY_DIR}/kwin_prepaint_probe.cc"
"#include <effect/effect.h>
#include <chrono>
struct Probe : KWin::Effect {
    void prePaintScreen(KWin::ScreenPrePaintData&, std::chrono::milliseconds) override {}
};
int main() { return 0; }
")
try_compile(KWIN_PREPAINT_HAS_PRESENT_TIME
    SOURCES "${CMAKE_BINARY_DIR}/kwin_prepaint_probe.cc"
    LINK_LIBRARIES KWin::kwin Qt6::Core Qt6::Gui KF6::CoreAddons
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

feature_summary(WHAT ALL)
