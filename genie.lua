-- build script

solution "StaticVector"
	language "C++"

	flags {
		"NoManifest",
		"ExtraWarnings",
		"FloatFast",
		"StaticRuntime",
	}

	debugdir( "." )

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"_HAS_EXCEPTIONS=0",
		"_SCL_SECURE=0",
		"_SECURE_SCL=0",
	}

	location ( "Build/" .. _ACTION )

	local config_list = {
		"Debug",
		"Release",
	}

	local platform_list = {
		"x32",
		"x64",
	}

	configurations(config_list)
	platforms(platform_list)

configuration "Debug"
	defines {
		"_DEBUG",
		"_CRTDBG_MAP_ALLOC"
	}
	flags { 
		"Symbols"
	}


configuration "Release"
	defines	{
		"NDEBUG",
	}
	flags {
		"Symbols",
		"OptimizeSpeed"
	}


configuration "x32"
	flags {
		"EnableSSE2",
	}
	defines {
		"WIN32",
	}

configuration "x64"
	defines {
		"WIN32",
	}

--  give each configuration/platform a unique output/target directory
for _, config in ipairs(config_list) do
	for _, plat in ipairs(platform_list) do
		configuration { config, plat }
		objdir( "Build/" .. _ACTION .. "/tmp/"  .. config  .. "-" .. plat )
		targetdir( "Bin/" .. _ACTION .. "/" .. config .. "-" .. plat )
	end
end


project "Test"
	kind "ConsoleApp"

	flags {
		"NoPCH",
	}

	files {
		"Src/**.*", 
	}

