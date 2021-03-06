/*
 Copyright (c) 2009 Peter "Corsix" Cawley

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include "config.h"
#include "Src/main.h"
#include "Src/bootstrap.h"
#include <stack>
#include <SDL.h>
#include <android/log.h>
#include <jni.h>
#include <unistd.h>
#include "Src/lua_sdl.h"
#include "commands.h"
#include "logging.h"

// Template magic for checking type equality
template<typename T1, typename T2>
struct types_equal {
	enum {
		result = -1,
	};
};

template<typename T1>
struct types_equal<T1, T1> {
	enum {
		result = 1,
	};
};

Configuration masterConfig;

JavaVM* jvm;
lua_State* L;

const char *speeds[] = { "Pause", "Slowest", "Slower", "Normal", "Max speed",
		"And then some more" };

static int showkeyboard(lua_State *L) {
	LOG_INFO("Showing keyboard");
	return sendCommand(jvm, COMMAND_SHOW_KEYBOARD);
}

static int hidekeyboard(lua_State *L) {
	LOG_INFO("Hiding keyboard");
	return sendCommand(jvm, COMMAND_HIDE_KEYBOARD);
}

static int showmenu(lua_State *L) {
	LOG_INFO("Showing Menu");
	return sendCommand(jvm, COMMAND_SHOW_MENU);
}

static int quickload(lua_State *L) {
	LOG_INFO("Quick Load");
	return sendCommand(jvm, COMMAND_QUICK_LOAD);
}

static int showloaddialog(lua_State *L) {
	LOG_INFO("Showing Load Dialog");
	return sendCommand(jvm, COMMAND_SHOW_LOAD_DIALOG);
}

static int showerrordialog(lua_State *L) {
	LOG_INFO("Showing Error Dialog");
	return sendCommand(jvm, COMMAND_GAME_LOAD_ERROR);
}

static int showsettingsdialog(lua_State *L) {
	LOG_INFO("Showing Settings Dialog");
	return sendCommand(jvm, COMMAND_SHOW_SETTINGS_DIALOG);
}

static int startvibration(lua_State *L) {
    LOG_INFO("Starting Vibration");
    int arg = lua_gettop(L);
    int vibrationCode = lua_tointeger(L, arg);
    return sendCommandInt(jvm, COMMAND_START_VIBRATION, vibrationCode);
}

static int stopvibration(lua_State *L) {
    LOG_INFO("Stopping Vibration");
    return sendCommand(jvm, COMMAND_STOP_VIBRATION);
}
static int gamespeedupdated(lua_State *L) {
	LOG_INFO("Game speed updated");
	int arg = lua_gettop(L);
	const char* gamespeed = lua_tostring(L, arg);

	for (int n = 0; n < 6; n++) {
		if (strcmp(gamespeed, speeds[n]) == 0) {
			return sendCommandInt(jvm, COMMAND_GAME_SPEED_UPDATED, n);
		}
	}
	LOG_INFO("Couldn't find game speed. Going with Normal.");
	return sendCommandInt(jvm, COMMAND_GAME_SPEED_UPDATED, 4);
}

extern "C" void Java_uk_co_armedpineapple_cth_SDLActivity_onNativeLowMemory(
		JNIEnv* env, jclass cls) {
	LOG_INFO("Calling Lua GC");
	int previously = lua_gc(L, LUA_GCCOUNT, NULL);
	lua_gc(L, LUA_GCCOLLECT, NULL);
	int now = lua_gc(L, LUA_GCCOUNT, NULL);
	int diff = now - previously;
	char* logmsg = new char[512];
	sprintf(logmsg, "Freed up %i KB", previously - now);

	LOG_INFO(logmsg);
}

extern "C" void Java_uk_co_armedpineapple_cth_SDLActivity_cthRestartGame(
		JNIEnv* env, jclass cls) {
	LOG_INFO("Restarting game");
	SDL_Event e;
	e.type = SDL_USEREVENT_RESTART;
	SDL_PushEvent(&e);
	LOG_INFO("Done");
}

extern "C" void Java_uk_co_armedpineapple_cth_SDLActivity_cthShowCheats(
		JNIEnv* env, jclass cls) {
	LOG_INFO("Showing cheats menu");
	SDL_Event e;
	e.type = SDL_USEREVENT_SHOWCHEATS;
	SDL_PushEvent(&e);
	LOG_INFO("Done");
}

extern "C" void Java_uk_co_armedpineapple_cth_SDLActivity_cthTryAutoSave(
		JNIEnv* env, jclass cls, jstring filename) {
	const char *nativeString = env->GetStringUTFChars(filename, 0);
	LOG_INFO("Attempting AutoSave");
	SDL_Event e;
	e.type = SDL_USEREVENT_AUTOSAVE;
	e.user.data1 = (void*) nativeString;
	SDL_PushEvent(&e);
	LOG_INFO("Done");
}

extern "C" void Java_uk_co_armedpineapple_cth_SDLActivity_cthSaveGame(
		JNIEnv* env, jclass jcls, jstring path) {

	const char *nativeString = env->GetStringUTFChars(path, 0);
	LOG_INFO("Saving game");
	SDL_Event e;
	e.type = SDL_USEREVENT_SAVE;
	e.user.data1 = (void*) nativeString;
	SDL_PushEvent(&e);
	//env->ReleaseStringUTFChars(path, nativeString);
	LOG_INFO("Done");

}

extern "C" void Java_uk_co_armedpineapple_cth_SDLActivity_cthLoadGame(
		JNIEnv* env, jclass jcls, jstring path) {

	const char *nativeString = env->GetStringUTFChars(path, 0);
	LOG_INFO("Loading game");
	SDL_Event e;
	e.type = SDL_USEREVENT_LOAD;
	e.user.data1 = (void*) nativeString;
	SDL_PushEvent(&e);
	//env->ReleaseStringUTFChars(path, nativeString);
	LOG_INFO("Done");
}

extern "C" void Java_uk_co_armedpineapple_cth_SDLActivity_cthGameSpeed(
		JNIEnv* env, jclass jcls, jint speed) {
	LOG_INFO("Setting game speed");
	SDL_Event e;
	e.type = SDL_USEREVENT_GAMESPEED;
	e.user.data1 = (void*) speeds[speed];
	SDL_PushEvent(&e);
	LOG_INFO("Done");
}

void populateConfiguration(JNIEnv* env, jobject configuration) {
	LOG_INFO("Populating Configuration Object");
	jclass configclass = env->GetObjectClass(configuration);
	jstring cthpath = (jstring) env->CallObjectMethod(configuration,
			env->GetMethodID(configclass, "getCthPath",
					"()Ljava/lang/String;"));
	jstring originalfiles = (jstring) env->CallObjectMethod(configuration,
			env->GetMethodID(configclass, "getOriginalFilesPath",
					"()Ljava/lang/String;"));
	jstring language = (jstring) env->CallObjectMethod(configuration,
			env->GetMethodID(configclass, "getLanguage",
					"()Ljava/lang/String;"));

	jint fpsLimit = env->CallIntMethod(configuration,
			env->GetMethodID(configclass, "getFpsLimit", "()I"));
	jint edgeBordersSize = env->CallIntMethod(configuration,
			env->GetMethodID(configclass, "getEdgeBordersSize", "()I"));
	jint edgeScrollSpeed = env->CallIntMethod(configuration,
			env->GetMethodID(configclass, "getEdgeScrollSpeed", "()I"));
	jint controlsMode = env->CallIntMethod(configuration,
			env->GetMethodID(configclass, "getControlsMode", "()I"));
	jboolean playSoundFx = env->CallBooleanMethod(configuration,
			env->GetMethodID(configclass, "getPlaySoundFx", "()Z"));
	jboolean playMusic = env->CallBooleanMethod(configuration,
			env->GetMethodID(configclass, "getPlayMusic", "()Z"));
	jboolean playAnnouncements = env->CallBooleanMethod(configuration,
			env->GetMethodID(configclass, "getPlayAnnouncements", "()Z"));
	jboolean adviserEnabled = env->CallBooleanMethod(configuration,
			env->GetMethodID(configclass, "getAdviser", "()Z"));
	jboolean edgeScrollEnabled = env->CallBooleanMethod(configuration,
			env->GetMethodID(configclass, "getEdgeScroll", "()Z"));

	masterConfig.cthPath = (char*) env->GetStringUTFChars(cthpath, 0);
	masterConfig.originalFilesPath = (char*) env->GetStringUTFChars(
			originalfiles, 0);
	masterConfig.language = (char*) env->GetStringUTFChars(language, 0);

	masterConfig.fpsLimit = (int) fpsLimit;
	masterConfig.playSoundFx = (unsigned char) playSoundFx;
	masterConfig.playMusic = (unsigned char) playMusic;
	masterConfig.playAnnouncements = (unsigned char) playAnnouncements;
	masterConfig.adviserEnabled = (unsigned char) adviserEnabled;
	masterConfig.edgeScroll = (unsigned char) edgeScrollEnabled;
	masterConfig.edgeScrollSize = (int) edgeBordersSize;
	masterConfig.edgeScrollSpeed = (int) edgeScrollSpeed;
	masterConfig.controlsMode = (Configuration::controls_mode) controlsMode;
	LOG_INFO("Done");
}

extern "C" void Java_uk_co_armedpineapple_cth_SDLActivity_cthUpdateConfiguration(
		JNIEnv* env, jclass jcls, jobject configuration) {
	LOG_INFO("Configuration Updated");

	populateConfiguration(env, configuration);

	// Update FPS Limit
	set_fps_limit(masterConfig.fpsLimit);

	SDL_Event e;
	e.type = SDL_USEREVENT_CONFIGURATION;
	e.user.data1 = &masterConfig;
	SDL_PushEvent(&e);
	LOG_INFO("Done");
}

//! Program entry point
/*!
 Prepares a Lua state for, and catches errors from, CorsixTH_lua_main(). By
 executing in Lua mode as soon as possible, errors can be nicely caught
 sooner, hence this function does as little as possible and leaves the rest
 for CorsixTH_lua_main().
 */
int SDL_main(int argc, char** argv, JavaVM* vm, jobject configuration) {

	// Initialise the environment and set initial configuration
	JNIEnv* jEnv;
	vm->AttachCurrentThread(&jEnv, NULL);
	populateConfiguration(jEnv, configuration);

	// Enable Logging
	START_LOGGING(masterConfig.cthPath);

	// Set FPS Limit
	set_fps_limit(masterConfig.fpsLimit);

	LOG_INFO("Starting CTH Android");
	jvm = vm;

	struct compile_time_lua_check {
		// Lua 5.1, not 5.0, is required
		int lua_5_point_1_required[LUA_VERSION_NUM >= 501 ? 1 : -1];

		// Lua numbers must be doubles so that the mantissa has at least
		// 32 bits (floats only have 24 bits)
		int number_is_double[types_equal<lua_Number, double>::result];
	};

	bool bRun = true;

	while (bRun) {
		L = NULL;

		L = luaL_newstate();

		if (L == NULL) {
			fprintf(stderr, "Fatal error starting CorsixTH: "
					"Cannot open Lua state.\n");
			return 0;
		}
		lua_atpanic(L, CorsixTH_lua_panic);
		luaL_openlibs(L);

		// Register C functions
		lua_register(L, "showkeyboard", showkeyboard);
		lua_register(L, "hidekeyboard", hidekeyboard);
		lua_register(L, "showmenu", showmenu);
		lua_register(L, "showloaddialog", showloaddialog);
		lua_register(L, "quickload", quickload);
		lua_register(L, "gamespeedupdated", gamespeedupdated);
		lua_register(L, "showerrordialog", showerrordialog);
		lua_register(L, "showsettings", showsettingsdialog);
		lua_register(L, "startvibration", startvibration);
		lua_register(L, "stopvibration", stopvibration);

		lua_settop(L, 0);
		lua_pushcfunction(L, CorsixTH_lua_stacktrace);
		lua_pushcfunction(L, CorsixTH_lua_main);

		// Move command line parameters onto the Lua stack
		lua_checkstack(L, argc);
		for (int i = 0; i < argc; ++i) {
			lua_pushstring(L, argv[i]);
		}

		if (lua_pcall(L, argc, 0, 1) != 0) {
			const char* err = lua_tostring(L, -1);
			if (err != NULL) {
				fprintf(stderr, "%s\n", err);
			} else {
				fprintf(stderr, "An error has occured in CorsixTH:\n"
						"Uncaught non-string Lua error\n");
			}
			lua_pushcfunction(L, Bootstrap_lua_error_report);
			lua_insert(L, -2);
			if (lua_pcall(L, 1, 0, 0) != 0) {
				fprintf(stderr, "%s\n", lua_tostring(L, -1));
			}
			sendCommand(jvm, COMMAND_GAME_LOAD_ERROR);
		}

		lua_getfield(L, LUA_REGISTRYINDEX, "_RESTART");
		bRun = lua_toboolean(L, -1) != 0;

		// Get cleanup functions out of the Lua state (but don't run them yet)
		std::stack<void (*)(void)> stkCleanup;
		lua_getfield(L, LUA_REGISTRYINDEX, "_CLEANUP");
		if (lua_type(L, -1) == LUA_TTABLE) {
			for (unsigned int i = 1; i <= lua_objlen(L, -1); ++i) {
				lua_rawgeti(L, -1, (int) i);
				stkCleanup.push((void (*)(void))lua_touserdata(L, -1));lua_pop
				(L, 1);
			}
		}

		lua_close(L);

		// The cleanup functions are executed _after_ the Lua state is fully
		// closed, and in reserve order to that in which they were registered.
		while (!stkCleanup.empty()) {
			if (stkCleanup.top() != NULL)
				stkCleanup.top()();
			stkCleanup.pop();
		}

		if (bRun) {
			printf("Restarting...\n");
		}
	}
	return 0;
}

