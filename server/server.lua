project "server"
    kind "ConsoleApp"
    language "C"
    cdialect "GNU11"

    targetdir "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"
    objdir    "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/${cfg.buildcfg}"

    includedirs {
        "%{wks.location}/utils/include"
    }

    files {
        "%{wks.location}/server/src/**.c"
    }

    links {
        "utils"
    }

    filter "configurations:Debug"
        symbols "on"
        warnings "extra"
        buildoptions "-g3"

    filter "configurations:Release"
        optimize "speed"
        warnings "extra"
        fatalwarnings "all"