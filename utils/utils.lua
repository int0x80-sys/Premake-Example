project "utils"
    kind "StaticLib"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/utils/include/**.h",
        "%{wks.location}/utils/src/**.c",
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"