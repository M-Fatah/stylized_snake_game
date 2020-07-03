workspace "snake_game"
    architecture "x86"
	startproject "snake_game"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
	{
        "MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}_%{cfg.system}_%{cfg.architecture}"

project "snake_game"
--  location "snake_game"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++11"

    targetdir ("build/" .. outputdir .. "/%{prj.name}")
    objdir    ("build/" .. outputdir .. "/%{prj.name}")

    
    files
    {
        "src/**.h",
        "src/**.c",
        "src/**.cpp"
    }
    
    includedirs
    {
        "include"
        -- "include/glad",
        -- "include/glfw",
        -- "include/glm",
        -- "include/khr",
        -- "include/openal"
    }
    
    links
    {
        "lib/OpenAL32.lib",
        "lib/glfw3.lib",
        "opengl32.lib"
    }
    
    postbuildcommands
    {
        ("{COPY} lib/OpenAL32.dll build/" .. outputdir .. "/%{prj.name}")
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "On"
        linkoptions {"/NODEFAULTLIB:MSVCRT"}
        
    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        optimize "On"
        linkoptions { "/SAFESEH:NO" }

    