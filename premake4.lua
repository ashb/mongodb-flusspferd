
newoption {
  trigger = "mongodb-root",
  description = "directory under which to find mongodb include/ and lib/ dirs",
  value = "dir"
}

solution "mongodb"

  targetdir "build"

  configurations { "Release", "Debug" }

  flags { "ExtraWarnings" }

  if _OPTIONS["mongodb-root"] then
    includedirs { _OPTIONS["mongodb-root"] .. "/include" }
    libdirs { _OPTIONS["mongodb-root"] .. "/lib" }
  end
  
  configuration "Release"
      flags { "Optimize" }
  configuration "Debug"
      flags { "Symbols" }
 
  configuration() -- all configurations

  project "mongodb"

    kind "SharedLib"
    language "C++"
    files { "src/*.cpp" }

    links { "flusspferd", "mongoclient", "boost_thread-gcc40-mt", "boost_system-gcc40-mt" }

    configuration { "not windows" }
      includedirs { 
        "/usr/local/include/boost-1_37",
        "/Users/ash/code/js/mozjs_debug/include"
      }

      libdirs {
        "/Users/ash/code/js/mozjs_debug/lib"
      }


