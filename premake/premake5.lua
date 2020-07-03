workspace "snake_game"
    architecture "x86"
	startproject "snake_game"

    configurations
    {
        "Debug",
        "Release"
    }

outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"

project "snake_game"
--  location "snake_game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++11"
    staticruntime "On"


    targetdir ("build/" .. outputdir .. "/%{prj.name}")
    objdir    ("build/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "include"
    }

    links
    {
        "OpenAL32.lib",
        "glfw3.lib",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "On"
    
    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        optimize "On"

    