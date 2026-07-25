add_subdirectory(kcm)

set(effect_SRCS
    Model.cc
    YetAnotherMagicLampEffect.cc
    plugin.cc
)

kconfig_add_kcfg_files(effect_SRCS
    YetAnotherMagicLampConfig.kcfgc
)

# MODULE, not SHARED: KDECMakeSettings clears CMAKE_SHARED_MODULE_PREFIX, so the
# plugin is installed as kwin4_effect_yetanothermagiclamp.so. KPluginMetaData
# derives the plugin id from that file name, and the id is what ends up in the
# kwinrc key and in the D-Bus reconfigureEffect() call. A SHARED library would be
# named libkwin4_effect_yetanothermagiclamp.so and shift the id accordingly.
add_library(kwin4_effect_yetanothermagiclamp MODULE ${effect_SRCS})

if(KWIN_PREPAINT_HAS_PRESENT_TIME)
    target_compile_definitions(kwin4_effect_yetanothermagiclamp
        PRIVATE KWIN_PREPAINT_HAS_PRESENT_TIME)
endif()

target_link_libraries(kwin4_effect_yetanothermagiclamp
    Qt6::Core
    Qt6::Gui
    KF6::ConfigCore
    KF6::ConfigGui
    KF6::CoreAddons
    KF6::WindowSystem
    KWin::kwin
)

install(
    TARGETS
        kwin4_effect_yetanothermagiclamp

    DESTINATION
        ${KDE_INSTALL_QTPLUGINDIR}/kwin/effects/plugins/
)
