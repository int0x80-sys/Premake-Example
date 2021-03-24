newoption {
    trigger = "arch",
    description = "Set the current target architecture",
    default = "x86",
    allowed = {
        { "x86", "32 bit target architecture" },
        { "x86_64", "64 bit target architecture" },
    }
}

workspace "premake-example"
    configurations { "Debug", "Release" }
    architecture (_OPTIONS["arch"])

    --Common Locations--
    --------------------

    CommonLocations = { }
    CommonLocations["TargetDir"] = "%{wks.location}/outputs/bin/%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}"
    CommonLocations["ObjDir"]    = "%{wks.location}/outputs/obj/%{prj.name}/%{cfg.architecture}/%{cfg.buildcfg}"

    include("utils/utils.lua")
    include("server/server.lua")
    include("client/client.lua")