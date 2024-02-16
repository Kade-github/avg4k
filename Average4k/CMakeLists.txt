cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

project(Average4k CXX)

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
     OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

################################################################################
# Global configuration types
################################################################################
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

################################################################################
# Global compiler options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_CXX_FLAGS "")
    set(CMAKE_CXX_FLAGS_DEBUG "")
    set(CMAKE_CXX_FLAGS_RELEASE "")
endif()

################################################################################
# Global linker options
################################################################################
if(MSVC)
    # remove default flags provided with CMake for MSVC
    set(CMAKE_EXE_LINKER_FLAGS "")
    set(CMAKE_MODULE_LINKER_FLAGS "")
    set(CMAKE_SHARED_LINKER_FLAGS "")
    set(CMAKE_STATIC_LINKER_FLAGS "")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

################################################################################
# Nuget packages function stub.
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

################################################################################
# Common utils
################################################################################
include(CMake/Utils.cmake)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Sub-projects
################################################################################



























set(PROJECT_NAME Average4k)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "resource.h"
    "SourceFiles/Average4k/A4kGame.h"
    "SourceFiles/Average4k/Api/AvgLuaFile.h"
    "SourceFiles/Average4k/Api/Functions/FCharts.h"
    "SourceFiles/Average4k/Api/Functions/FData.h"
    "SourceFiles/Average4k/Api/Functions/FFiles.h"
    "SourceFiles/Average4k/Api/Functions/FGame.h"
    "SourceFiles/Average4k/Api/Functions/FGameplay.h"
    "SourceFiles/Average4k/Api/Functions/FMenu.h"
    "SourceFiles/Average4k/Api/Functions/FSkins.h"
    "SourceFiles/Average4k/Api/Functions/FSteam.h"
    "SourceFiles/Average4k/Api/Stubs/LuaMenu.h"
    "SourceFiles/Average4k/Api/Stubs/LuaNote.h"
    "SourceFiles/Average4k/Api/Stubs/LuaObject.h"
    "SourceFiles/Average4k/Api/Stubs/LuaSong.h"
    "SourceFiles/Average4k/Api/Stubs/LuaSprite.h"
    "SourceFiles/Average4k/Api/Stubs/LuaText.h"
    "SourceFiles/Average4k/Console/CmdHandler.h"
    "SourceFiles/Average4k/Data/Chart/AsyncChartLoader.h"
    "SourceFiles/Average4k/Data/Chart/ChartFile.h"
    "SourceFiles/Average4k/Data/Chart/ChartFinder.h"
    "SourceFiles/Average4k/Data/Chart/Difficulty.h"
    "SourceFiles/Average4k/Data/Chart/Metadata.h"
    "SourceFiles/Average4k/Data/Chart/Providers/StepFile.h"
    "SourceFiles/Average4k/Data/Chart/ScrollPoint.h"
    "SourceFiles/Average4k/Data/Chart/SpeedPoints.h"
    "SourceFiles/Average4k/Data/Chart/StopPoint.h"
    "SourceFiles/Average4k/Data/Chart/TimingPoint.h"
    "SourceFiles/Average4k/Data/SaveData.h"
    "SourceFiles/Average4k/Data/Types/AudioData.h"
    "SourceFiles/Average4k/Data/Types/GameplayData.h"
    "SourceFiles/Average4k/Data/Types/SkinData.h"
    "SourceFiles/Average4k/Data/Types/VideoData.h"
    "SourceFiles/Average4k/Defines.h"
    "SourceFiles/Average4k/Helpers/FileDiag.h"
    "SourceFiles/Average4k/Helpers/JudgementHelper.h"
    "SourceFiles/Average4k/Helpers/QuickAdd.h"
    "SourceFiles/Average4k/Helpers/SteamHelper.h"
    "SourceFiles/Average4k/Helpers/StringTools.h"
    "SourceFiles/Average4k/Helpers/TimeToScreen.h"
    "SourceFiles/Average4k/Helpers/WaveformData.h"
    "SourceFiles/Average4k/Multiplayer/Connection.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketChartAcquired.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketHello.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketHostChangeChart.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketHostEndChart.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketHostServer.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketHostStartGame.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketHostUpdateLobby.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketJoinServer.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketLeaderboardRequest.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketLeave.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketNoteHit.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketRequestAvatar.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketSendMessage.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketServerList.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketSongFinished.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketStatus.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketSubmitScore.h"
    "SourceFiles/Average4k/Multiplayer/Packets/CPacketWtfAmIn.h"
    "SourceFiles/Average4k/Multiplayer/Packets/Lobby.h"
    "SourceFiles/Average4k/Multiplayer/Packets/Packet.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketAvatarRequestResponse.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketFinalizeChart.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketFuckYou.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketHello.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketHostServerReply.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketInvalidPacket.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketJoinServerReply.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketLeaderboardResponse.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketOnChat.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketPlayerSongFinished.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketScoreResult.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketServerListReply.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketStartLobbyGame.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketStatus.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketUpdateEncryptionParameters.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketUpdateLeaderboard.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketUpdateLobbyChart.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketUpdateLobbyData.h"
    "SourceFiles/Average4k/Multiplayer/Packets/SPacketWtfAmInReply.h"
    "SourceFiles/Average4k/Objects/BaseNote.h"
    "SourceFiles/Average4k/Objects/Notes/Hold.h"
    "SourceFiles/Average4k/Objects/Playfield.h"
    "SourceFiles/Average4k/Objects/RenderTexture.h"
    "SourceFiles/Average4k/Objects/UnicodeText.h"
    "SourceFiles/Average4k/Screens/Menu/Gameplay.h"
    "SourceFiles/Average4k/Screens/Menu/MainMenu.h"
    "SourceFiles/Average4k/Skin/Skin.h"
    "SourceFiles/Average4k/Steam/UGCHandler.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "Include/ImGui/imgui.cpp"
    "Include/ImGui/imgui_demo.cpp"
    "Include/ImGui/imgui_draw.cpp"
    "Include/ImGui/imgui_impl_glfw.cpp"
    "Include/ImGui/imgui_impl_opengl3.cpp"
    "Include/ImGui/imgui_tables.cpp"
    "Include/ImGui/imgui_widgets.cpp"
    "Include/ImGui/implot.cpp"
    "Include/ImGui/implot_demo.cpp"
    "Include/ImGui/implot_items.cpp"
    "SourceFiles/Average4k/A4kGame.cpp"
    "SourceFiles/Average4k/Api/AvgLuaFile.cpp"
    "SourceFiles/Average4k/Api/Functions/FCharts.cpp"
    "SourceFiles/Average4k/Api/Functions/FData.cpp"
    "SourceFiles/Average4k/Api/Functions/FFiles.cpp"
    "SourceFiles/Average4k/Api/Functions/FGame.cpp"
    "SourceFiles/Average4k/Api/Functions/FGameplay.cpp"
    "SourceFiles/Average4k/Api/Functions/FMenu.cpp"
    "SourceFiles/Average4k/Api/Functions/FSkins.cpp"
    "SourceFiles/Average4k/Api/Functions/FSteam.cpp"
    "SourceFiles/Average4k/Console/CmdHandler.cpp"
    "SourceFiles/Average4k/Data/Chart/AsyncChartLoader.cpp"
    "SourceFiles/Average4k/Data/Chart/ChartFile.cpp"
    "SourceFiles/Average4k/Data/Chart/ChartFinder.cpp"
    "SourceFiles/Average4k/Data/Chart/Providers/StepFile.cpp"
    "SourceFiles/Average4k/Helpers/Base64.cpp"
    "SourceFiles/Average4k/Helpers/JudgementHelper.cpp"
    "SourceFiles/Average4k/Helpers/SteamHelper.cpp"
    "SourceFiles/Average4k/Multiplayer/Connection.cpp"
    "SourceFiles/Average4k/Objects/BaseNote.cpp"
    "SourceFiles/Average4k/Objects/Notes/Hold.cpp"
    "SourceFiles/Average4k/Objects/UnicodeText.cpp"
    "SourceFiles/Average4k/Screens/Menu/Gameplay.cpp"
    "SourceFiles/Average4k/Screens/Menu/MainMenu.cpp"
    "SourceFiles/Average4k/Skin/Skin.cpp"
    "SourceFiles/Average4k/Steam/UGCHandler.cpp"
    "SourceFiles/External/ImGui/imgui.cpp"
    "SourceFiles/External/ImGui/imgui_demo.cpp"
    "SourceFiles/External/ImGui/imgui_draw.cpp"
    "SourceFiles/External/ImGui/imgui_impl_glfw.cpp"
    "SourceFiles/External/ImGui/imgui_impl_opengl3.cpp"
    "SourceFiles/External/ImGui/imgui_tables.cpp"
    "SourceFiles/External/ImGui/imgui_widgets.cpp"
    "SourceFiles/External/ImGui/implot.cpp"
    "SourceFiles/External/ImGui/implot_demo.cpp"
    "SourceFiles/External/ImGui/implot_items.cpp"
    "SourceFiles/Main.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Demo)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
################################################################################
# Target name
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}-$<CONFIG>"
        TARGET_NAME_RELEASE "${PROJECT_NAME}-$<CONFIG>"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        TARGET_NAME_DEBUG   "${PROJECT_NAME}-$<CONFIG>"
        TARGET_NAME_RELEASE "${PROJECT_NAME}-$<CONFIG>"
    )
endif()
################################################################################
# Output directory
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Builds/Binaries/$<CONFIG>"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Builds/Binaries/$<CONFIG>"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Builds/Binaries/$<CONFIG>"
        OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Builds/Binaries/$<CONFIG>"
    )
endif()
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
            MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
            MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
    )
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Include directories
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/Include"
    )
endif()

################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "_CRT_SECURE_NO_WARNINGS;"
        "SILENCE_ALL_CXX17_DEPRECATION_WARNINGS;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "_CRT_SECURE_NO_WARNINGS;"
        "SILENCE_ALL_CXX17_DEPRECATION_WARNINGS;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /O1;
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++latest;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /D;
            _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /O1;
                /Oi;
                /Gy
            >
            /permissive-;
            /std:c++latest;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            /D;
            _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS;
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /DEBUG;
            /SUBSYSTEM:WINDOWS
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "dbghelp"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "dbghelp"
    )
endif()
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Library"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Library"
    )
endif()

