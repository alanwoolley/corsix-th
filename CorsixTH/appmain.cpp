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

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

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

JNIEnv* jEnv;
lua_State* L;

static int sendCommand(jint cmd) {
	jclass cls = jEnv->FindClass("uk/co/armedpineapple/corsixth/SDLActivity");
	jmethodID method = jEnv->GetStaticMethodID(cls, "sendCommand", "(I)V");
	jEnv->CallStaticVoidMethod(cls, method, cmd);
	return 0;
}

static int showkeyboard(lua_State *L) {
	LOGI("Showing keyboard");
	return sendCommand(COMMAND_SHOW_KEYBOARD);
}

static int hidekeyboard(lua_State *L) {
	LOGI("Hiding keyboard");
	return sendCommand(COMMAND_HIDE_KEYBOARD);
}

static int showmenu(lua_State *L) {
	LOGI("Showing Menu");
	return sendCommand(COMMAND_SHOW_MENU);
}

static int quickload(lua_State *L) {
	LOGI("Quick Load");
	return sendCommand(COMMAND_QUICK_LOAD);
}

static int showloaddialog(lua_State *L) {
	LOGI("Showing Load Dialog");
	return sendCommand(COMMAND_SHOW_LOAD_DIALOG);
}

extern "C" void Java_uk_co_armedpineapple_corsixth_SDLActivity_cthRestartGame(
		JNIEnv* env, jclass cls) {
	LOGI("Restarting game");
	SDL_Event e;
    e.type = SDL_USEREVENT_RESTART;
    SDL_PushEvent(&e);
    LOGI("Done");
}

extern "C" void Java_uk_co_armedpineapple_corsixth_SDLActivity_cthSaveGame(
		JNIEnv* env, jclass jcls, jstring path) {
	
	const char *nativeString = env->GetStringUTFChars(path, 0);
	LOGI("Saving game");
	SDL_Event e;
    e.type = SDL_USEREVENT_SAVE;
    e.user.data1 = (void*)nativeString;
    SDL_PushEvent(&e);
	//env->ReleaseStringUTFChars(path, nativeString);
	LOGI("Done");

}

extern "C" void Java_uk_co_armedpineapple_corsixth_SDLActivity_cthLoadGame(
		JNIEnv* env, jclass jcls, jstring path) {

	const char *nativeString = env->GetStringUTFChars(path, 0);
	LOGI("Loading game");
	SDL_Event e;
    e.type = SDL_USEREVENT_LOAD;
    e.user.data1 = (void*)nativeString;
    SDL_PushEvent(&e);
	//env->ReleaseStringUTFChars(path, nativeString);
	LOGI("Done");
}

//! Program entry point
/*!
 Prepares a Lua state for, and catches errors from, CorsixTH_lua_main(). By
 executing in Lua mode as soon as possible, errors can be nicely caught
 sooner, hence this function does as little as possible and leaves the rest
 for CorsixTH_lua_main().
 */
int SDL_main(int argc, char** argv, JNIEnv* env) {
	jEnv = env;

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
		lua_register(L, "showkeyboard", showkeyboard);
		lua_register(L, "hidekeyboard", hidekeyboard);
		lua_register(L, "showmenu", showmenu);
		lua_register(L, "showloaddialog", showloaddialog);
		lua_register(L, "quickload", quickload);
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

