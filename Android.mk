LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := appmain

SDL_PATH := ../SDL
LUA_PATH := ../LUA
SDL_MIXER_PATH :=../SDL_mixer
AGG_PATH := ../AGG
CORSIX_TH_SRC := CorsixTH/Src
LFS_SRC := LFS
LPEG_SRC := LPEG
FREETYPE_PATH := ../freetype2
SDL_GFX_PATH := ../SDL_gfx
FFMPEG_PATH := ../ffmpeg

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
					$(LOCAL_PATH)/$(AGG_PATH)/include \
					$(LOCAL_PATH)/$(AGG_PATH)/src \
					$(LOCAL_PATH)/$(FREETYPE_PATH)/include \
					$(LOCAL_PATH)/$(FREETYPE_PATH)/include/freetype \
					$(LOCAL_PATH)/$(SDL_MIXER_PATH) \
					$(LOCAL_PATH)/$(CORSIX_TH_SRC) \
					$(LOCAL_PATH)/$(CTH_ASSETS) \
					$(LOCAL_PATH)/$(LFS_SRC) \
					$(LOCAL_PATH)/$(LPEG) \
					$(LOCAL_PATH)/$(SDL_GFX_PATH) \
					$(LOCAL_PATH)/$(LUA_PATH) \
					$(LOCAL_PATH)/$(FFMPEG_PATH) \
					$(LOCAL_PATH)/$(FFMPEG_PATH)/libavcodec \
					$(LOCAL_PATH)/$(FFMPEG_PATH)/libavformat \
					$(LOCAL_PATH)/$(FFMPEG_PATH)/libavutil \
					$(LOCAL_PATH)/$(FFMPEG_PATH)/libswresample \
					$(LOCAL_PATH)/$(FFMPEG_PATH)/libswscale
					
LOCAL_CFLAGS := -DPLAY_MOD 

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.cpp \
			$(CORSIX_TH_SRC)/../appmain.cpp \
			$(CORSIX_TH_SRC)/../logging.cpp \
			$(CORSIX_TH_SRC)/../commands.cpp \
			$(CORSIX_TH_SRC)/main.cpp \
			$(CORSIX_TH_SRC)/bootstrap.cpp \
			$(CORSIX_TH_SRC)/th_lua.cpp \
			$(CORSIX_TH_SRC)/th.cpp \
			$(CORSIX_TH_SRC)/th_lua_map.cpp \
			$(CORSIX_TH_SRC)/random.c \
			$(CORSIX_TH_SRC)/th_pathfind.cpp\
			$(CORSIX_TH_SRC)/th_lua_gfx.cpp \
			$(CORSIX_TH_SRC)/th_map.cpp \
			$(CORSIX_TH_SRC)/th_lua_anims.cpp \
			$(CORSIX_TH_SRC)/th_gfx.cpp \
			$(CORSIX_TH_SRC)/th_lua_sound.cpp \
			$(CORSIX_TH_SRC)/th_gfx_sdl.cpp \
			$(CORSIX_TH_SRC)/th_lua_strings.cpp \
			$(CORSIX_TH_SRC)/run_length_encoder.cpp \
			$(CORSIX_TH_SRC)/th_lua_ui.cpp \
			$(CORSIX_TH_SRC)/th_sound.cpp \
			$(CORSIX_TH_SRC)/th_gfx_font.cpp \
			$(CORSIX_TH_SRC)/rnc.cpp \
			$(CORSIX_TH_SRC)/iso_fs.cpp \
			$(CORSIX_TH_SRC)/persist_lua.cpp \
			$(CORSIX_TH_SRC)/sdl_core.cpp \
			$(CORSIX_TH_SRC)/sdl_audio.cpp \
			$(CORSIX_TH_SRC)/sdl_wm.cpp \
			$(CORSIX_TH_SRC)/xmi2mid.cpp \
			$(CORSIX_TH_SRC)/th_lua_movie.cpp \
			$(CORSIX_TH_SRC)/th_movie.cpp \
			$(LFS_SRC)/lfs.c \
			$(LFS_SRC)/lfs_ext.c \
			$(LPEG_SRC)/lpeg.c
			

LOCAL_SHARED_LIBRARIES := libluajit libSDL libSDL_mixer libffmpeg
LOCAL_STATIC_LIBRARIES := libfreetype2 libSDL_gfx libAGG

LOCAL_LDLIBS := -llog

LOCAL_CPP_FEATURES += exceptions

include $(BUILD_SHARED_LIBRARY)
