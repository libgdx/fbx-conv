-- TODO: Because there are a few remaining things ...
--    1. x86/x64 switching
--    2. clean this file up because I'm sure it could be organized better
--    3. consider maybe switching to CMake because of the ugly hack below
--
-- NOTE: I am intentionally leaving out a "windows+gmake" configuration
--       as trying to compile against the FBX SDK using MinGW results in
--       compile errors. Some quick googling seems to indicate MinGW is
--       not supported by the FBX SDK?
--       If you try to use this script to build with MinGW you will end
--       up with a Makefile that has god knows what in it

FBX_SDK_ROOT = os.getenv("FBX_SDK_ROOT")
if not FBX_SDK_ROOT then
	printf("ERROR: Environment variable FBX_SDK_ROOT is not set.")
	printf("Set it to something like: C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2016.1")
	os.exit()
end
BUILD_NUMBER = os.getenv("BUILD_NUMBER")
if not BUILD_NUMBER then
	BUILD_NUMBER = 0
end

-- avert your eyes children!
if string.find(_ACTION, "xcode") then
	-- TODO: i'm sure we could do some string search+replace trickery to make 
	--       this more general-purpose
	-- take care of the most common case where the FBX SDK is installed to the
	-- default location and part of the path contains a space
	-- god help you if you install the FBX SDK using a totally different path
	-- that contains a space AND you want to use Xcode
	-- Premake + Xcode combined fuck this up so badly making it nigh-impossible
	-- to do any kind of _proper_ path escaping here (I wasted an hour on this)
	-- (maybe I should have used CMake ....)
	FBX_SDK_ROOT = string.gsub(FBX_SDK_ROOT, "FBX SDK", "'FBX SDK'")
end
-- ok, you can look again

BUILD_DIR = "build"
if _ACTION == "clean" then
	os.rmdir(BUILD_DIR)
end

solution "fbx-conv"
	configurations { "Debug", "Release" }
	location (BUILD_DIR .. "/" .. _ACTION)
	
project "fbx-conv"
	--- GENERAL STUFF FOR ALL PLATFORMS --------------------------------
	kind "ConsoleApp"
	language "C++"
	location (BUILD_DIR .. "/" .. _ACTION)
	files {
		"./src/**.c*",
		"./src/**.h",
	}
	includedirs {
		(FBX_SDK_ROOT .. "/include"),
		"./libs/libpng/include",
		"./libs/zlib/include",
	}
	defines {
		"FBXSDK_NEW_API",
		"BUILD_NUMBER=" .. BUILD_NUMBER,
	}
	--- debugdir "."

	configuration "Debug"
		defines {
			"DEBUG",
		}
		flags { "Symbols" }
	
	configuration "Release"
		defines {
			"NDEBUG",
		}
		flags { "Optimize" }

	--- VISUAL STUDIO --------------------------------------------------
	configuration "vs*"
		flags {
			"NoPCH",
			"NoMinimalRebuild"
		}
		buildoptions { "/MP" }
		defines {
			"_CRT_SECURE_NO_WARNINGS",
			"_CRT_NONSTDC_NO_WARNINGS"
		}
		includedirs {
			(FBX_SDK_ROOT .. "/include/"),
		}
		libdirs {			
			"./libs/libpng/lib/windows/x86",
			"./libs/zlib/lib/windows/x86",
		}
		links {
			"libpng14",
			"zlib",
			"libfbxsdk-md",
		}
		
	configuration { "vs*", "Debug" }
		libdirs {
			(FBX_SDK_ROOT .. "/lib/vs2015/x86/debug"),
		}
		
	configuration { "vs*", "Release" }
		libdirs {
			(FBX_SDK_ROOT .. "/lib/vs2015/x86/release"),
		}

	--- LINUX ----------------------------------------------------------
	configuration { "linux" }
		kind "ConsoleApp"
		buildoptions { "-Wall" }
		-- TODO: while using x64 will likely be fine for most people nowadays,
		--       we still need to make this configurable
		libdirs {
			"./libs/libpng/lib/linux/x64",
			"./libs/zlib/lib/linux/x64",
		}
		links {
			"png",
			"z",
			"pthread",
			"xml2",
			"fbxsdk",
			"dl",
		}

	configuration { "linux", "Debug" }
		libdirs {
			(FBX_SDK_ROOT .. "/lib/gcc4/x64/debug"),
		}
		
	configuration { "linux", "Release" }
		libdirs {
			(FBX_SDK_ROOT .. "/lib/gcc4/x64/release"),
		}

	--- MAC ------------------------------------------------------------
	configuration { "macosx" }
		kind "ConsoleApp"
		buildoptions { "-Wall" }
		
		xcodebuildsettings {
			["ALWAYS_SEARCH_USER_PATHS"] = "YES"
		}

		libdirs {
			(FBX_SDK_ROOT .. "/lib/clang"),
			"./libs/libpng/lib/macosx",
			"./libs/zlib/lib/macosx",
		}
		links {
			"png",
			"z",
			"CoreFoundation.framework",
			"fbxsdk",
		}

	configuration { "macosx", "Debug" }
		libdirs {
			(FBX_SDK_ROOT .. "/lib/clang/debug"),
		}
		
	configuration { "macosx", "Release" }
		libdirs {
			(FBX_SDK_ROOT .. "/lib/clang/release"),
		}
