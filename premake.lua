-- TODO: Because there is plenty ...
--    1. What about other IDE's that Premake supports? e.g. codeblocks, xcode
--    2. x86/x64 switching
--    3. get macosx library for zlib
--    4. actually test linux/mac build configurations
--    5. clean this file up because I'm sure it could be organized better
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
	printf("Set it to something like: C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2013.3")
	os.exit()
end

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
	}
	debugdir "."

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
		libdirs {
			(FBX_SDK_ROOT .. "/lib/vs2010/x86"),
			"./libs/libpng/lib/windows/x86",
			"./libs/zlib/lib/windows/x86",
		}
		links {
			"libpng14",
			"zlib",
		}
		
	configuration { "vs*", "Debug" }
		links {
			"fbxsdk-2013.3-mdd",
		}
		
	configuration { "vs*", "Release" }
		links {
			"fbxsdk-2013.3-md",
		}

	--- LINUX (GCC+MAKE) -----------------------------------------------
	configuration { "linux", "gmake" }
		kind "ConsoleApp"
		buildoptions { "-Wall" }
		libdirs {
			(FBX_SDK_ROOT .. "/lib/gcc4/x86"),
			"./libs/libpng/lib/linux/x86",
			"./libs/zlib/lib/linux/x86",
		}
		links {
			"libpng",
			"libz",
		}

	configuration { "linux", "gmake", "Debug" }
		links {
			"fbxsdk-2013-staticd",
		}
		
	configuration { "linux", "gmake", "Release" }
		links {
			"fbxsdk-2013-static",
		}

	--- MAC (GCC+MAKE) -------------------------------------------------
	configuration { "macosx", "gmake" }
		kind "ConsoleApp"
		buildoptions { "-Wall" }
		libdirs {
			(FBX_SDK_ROOT .. "/lib/gcc4/ub"),
			"./libs/libpng/lib/macosx/x86",
			"./libs/zlib/lib/macosx/x86",
		}
		links {
			"libpng",
			"libz",
		}

	configuration { "macosx", "gmake", "Debug" }
		links {
			"fbxsdk-2013-staticd",
		}
		
	configuration { "macosx", "gmake", "Release" }
		links {
			"fbxsdk-2013-static",
		}
