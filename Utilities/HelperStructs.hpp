#pragma once
#include <glm/glm.hpp>
#include <string>
#include <nlohmannjson/json.hpp>
#include <SDL3/SDL.h>

using json = nlohmann::json;

enum class Levels {
	LEVEL_1,
	TEST_LEVEL,
	TEST_LEVEL_OLD,
	END
};

enum class ActiveKeys {
	MOVE_LEFT,
	MOVE_RIGHT,
	SPACE,
	DUCK
};

enum class LookDirections {
	LEFT,
	RIGHT,
	END
};

struct PhysicsState
{
	glm::vec2 position{ 0.0f };
	glm::vec2 velocity{ 0.0f };
};

struct Derivative
{
	glm::vec2 dpos{ 0.0f };
	glm::vec2 dvel{ 0.0f };
};


struct Box {
	glm::vec2 Position;
	glm::vec4 Color;
	glm::vec2 Size;
	glm::vec2 TexturePosition;
	int TextureIndex;
	float RotationalAngle;
	float AngleModifier;
	glm::vec2 TranslateMargin;
};

struct Text {
	glm::vec2 Possition{0.0f, 0.0f};
	glm::vec3 Color{0.0f, 0.0f, 0.0f};
	std::string TextField{""};
	glm::mat4 *ModelMatrix;
	bool CenteredX{ true };
	bool CenteredY{ true };
	bool RightSided{ false };
	float Scale;
};

struct Settings {
	bool VSync{ false };
	bool debugMode{ false };
	bool autoRestart{ false };
	bool showFpsInGame{ true };

	int SFXVolume{ 5 };
	int MusicVolume{ 5 };

	float textSizeMultiplier{ 800.0f };

	glm::ivec2 screenSize{ 0 };

	int currentWindowModeIndex{ 0 };
	int currentResolutionIndex{ 4 };

	glm::ivec2 viewportOffset{ 0 };

	friend void to_json(json& j, const Settings& s) {
		j = json{
			{"VSync", s.VSync},
			{"debugMode", s.debugMode},
			{"autoRestart", s.autoRestart},
			{"SFXVolume", s.SFXVolume},
			{"MusicVolume", s.MusicVolume},
			{"textSizeMultiplier", s.textSizeMultiplier},
			{"currentWindowModeIndex", s.currentWindowModeIndex},
			{"currentResolutionIndex", s.currentResolutionIndex},
			{"showFpsInGame", s.showFpsInGame}
		};
	}

	friend void from_json(const json& j, Settings& s) {
		j.at("VSync").get_to(s.VSync);
		j.at("debugMode").get_to(s.debugMode);
		j.at("autoRestart").get_to(s.autoRestart);
		j.at("SFXVolume").get_to(s.SFXVolume);
		j.at("MusicVolume").get_to(s.MusicVolume);
		j.at("textSizeMultiplier").get_to(s.textSizeMultiplier);
		j.at("currentWindowModeIndex").get_to(s.currentWindowModeIndex);
		j.at("currentResolutionIndex").get_to(s.currentResolutionIndex);
		j.at("showFpsInGame").get_to(s.showFpsInGame);
	}
};

struct SaveData {
	int Level_1_best_levelTime_minutes{ 0 };
	int Level_1_best_levelTime_seconds{ 0 };
	int Level_1_best_levelTime_centiseconds{ 0 };

	friend void to_json(json& j, const SaveData& s) {
		j = json{
			{"Level_1_TimeM", s.Level_1_best_levelTime_minutes},
			{"Level_1_TimeS", s.Level_1_best_levelTime_seconds},
			{"Level_1_TimeCS", s.Level_1_best_levelTime_centiseconds},
		};
	}

	friend void from_json(const json& j, SaveData& s) {
		j.at("Level_1_TimeM").get_to(s.Level_1_best_levelTime_minutes);
		j.at("Level_1_TimeS").get_to(s.Level_1_best_levelTime_seconds);
		j.at("Level_1_TimeCS").get_to(s.Level_1_best_levelTime_centiseconds);
	}
};

struct Controls {
	SDL_Scancode LEFT_KEY_BIND{ SDL_SCANCODE_A };
	SDL_Scancode RIGHT_KEY_BIND{ SDL_SCANCODE_D };
	SDL_Scancode JUMP_KEY_BIND{ SDL_SCANCODE_SPACE };
	SDL_Scancode DUCK_KEY_BIND{ SDL_SCANCODE_LSHIFT };

	friend void to_json(json& j, const Controls& s) {
		j = json{
			{"LEFT_KEY_BIND", SDL_GetKeyName(SDL_GetKeyFromScancode(s.LEFT_KEY_BIND, SDL_KMOD_NONE, false))},
			{"RIGHT_KEY_BIND", SDL_GetKeyName(SDL_GetKeyFromScancode(s.RIGHT_KEY_BIND, SDL_KMOD_NONE, false))},
			{"JUMP_KEY_BIND", SDL_GetKeyName(SDL_GetKeyFromScancode(s.JUMP_KEY_BIND, SDL_KMOD_NONE, false))},
			{"DUCK_KEY_BIND", SDL_GetKeyName(SDL_GetKeyFromScancode(s.DUCK_KEY_BIND, SDL_KMOD_NONE, false))},

		};
	}

	friend void from_json(const json& j, Controls& s) {
		auto parseKey = [](const std::string& keyName, SDL_Scancode defaultScancode) {
			SDL_Keycode keycode = SDL_GetKeyFromName(keyName.c_str());
			if (keycode == SDLK_UNKNOWN) {
				return defaultScancode;  // Fallback to default if invalid
			}
			return SDL_GetScancodeFromKey(keycode, SDL_KMOD_NONE);
			};

		s.LEFT_KEY_BIND = parseKey(j.value("LEFT_KEY_BIND", "A"), SDL_SCANCODE_A);
		s.RIGHT_KEY_BIND = parseKey(j.value("RIGHT_KEY_BIND", "D"), SDL_SCANCODE_D);
		s.JUMP_KEY_BIND = parseKey(j.value("JUMP_KEY_BIND", "Space"), SDL_SCANCODE_SPACE);
		s.DUCK_KEY_BIND = parseKey(j.value("DUCK_KEY_BIND", "Left Shift"), SDL_SCANCODE_LSHIFT);
	}
};

struct Config {
	float GravityUP{ -900.0f };
	float GravityDOWN{ -1200.0f };
	float FallSpeedLimit{ -1500.0f };

	float RunAccelerationOnFoot{ 1000.0f };
	float RunAccelerationMidAir{ 400.0f };
	float RunSpeedLimit{ 800.0f };

	float CrouchAcceleration{ 1000.0f };
	float CrouchSpeedLimit{ 200.0f };

	float DefaultFrictionStopSpeed{ 20.0f };
	float DefaultFriction{ 2000.0f };
	// values > ~3800.0f are unstable 

	float AirResistanceStopSpeed{ 10.0f };
	float DefaultAirResistance{ 300.0f };
	// values > ~3800.0f are unstable 

	float SlidingFrictionStopSpeed{ 20.0f };
	float SlidingFriction{ 1000.0f };
	// values > ~3800.0f are unstable 

	float SlidingImpulse{ 50000.0f };
	float ThresholdSpeedToSlide{ 300.0f };

	float JumpStartImpulse{ 50000.0f };
	float JumpContinuousAcceleration{ 2000.0f };

	float DoubleJumpStartImpulse{ 50000.0f };
	float DoubleJumpContinuousAcceleration{ 2000.0f };

	float WallJumpStartImpulse{ 50000.0f };
	float WallJumpStartSideImpulse{ 25000.0f };
	float WallJumpContinuousAcceleration{ 2000.0f };

	int VariableJumpTicks{ 32 };
	int VariableDoubleJumpTicks{ 32 };
	int VariableWallJumpTicks{ 32 };

	int JumpBufferTicks{ 25 };

	int CoyoteTimeTicks{ 12 };

	float WallSlideSpeed{ 125.0f };

	float SlidingCollidorFactor{ 0.75f };

	friend void to_json(json& j, const Config& c) {
		j = json{
			{"GravityUP", c.GravityUP},
			{"GravityDOWN", c.GravityDOWN},
			{"FallSpeedLimit", c.FallSpeedLimit},
			{"RunAccelerationOnFoot", c.RunAccelerationOnFoot},
			{"RunAccelerationMidAir", c.RunAccelerationMidAir},
			{"RunSpeedLimit", c.RunSpeedLimit},
			{"CrouchAcceleration", c.CrouchAcceleration},
			{"CrouchSpeedLimit", c.CrouchSpeedLimit},
			{"DefaultFrictionStopSpeed", c.DefaultFrictionStopSpeed},
			{"DefaultFriction", c.DefaultFriction},
			{"AirResistanceStopSpeed", c.AirResistanceStopSpeed},
			{"DefaultAirResistance", c.DefaultAirResistance},
			{"SlidingFrictionStopSpeed", c.SlidingFrictionStopSpeed},
			{"SlidingFriction", c.SlidingFriction},
			{"SlidingImpulse", c.SlidingImpulse},
			{"ThresholdSpeedToSlide", c.ThresholdSpeedToSlide},
			{"JumpStartImpulse", c.JumpStartImpulse},
			{"JumpContinuousAcceleration", c.JumpContinuousAcceleration},
			{"DoubleJumpStartImpulse", c.DoubleJumpStartImpulse},
			{"DoubleJumpContinuousAcceleration", c.DoubleJumpContinuousAcceleration},
			{"WallJumpStartImpulse", c.WallJumpStartImpulse},
			{"WallJumpStartSideImpulse", c.WallJumpStartSideImpulse},
			{"WallJumpContinuousAcceleration", c.WallJumpContinuousAcceleration},
			{"VariableJumpTicks", c.VariableJumpTicks},
			{"VariableDoubleJumpTicks", c.VariableDoubleJumpTicks},
			{"VariableWallJumpTicks", c.VariableWallJumpTicks},
			{"JumpBufferTicks", c.JumpBufferTicks},
			{"CoyoteTimeTicks", c.CoyoteTimeTicks},
			{"WallSlideSpeed", c.WallSlideSpeed},
			{"SlidingCollidorFactor", c.SlidingCollidorFactor},
		};
	}

	friend void from_json(const json& j, Config& c) {
		j.at("GravityUP").get_to(c.GravityUP);
		j.at("GravityDOWN").get_to(c.GravityDOWN);
		j.at("FallSpeedLimit").get_to(c.FallSpeedLimit);
		j.at("RunAccelerationOnFoot").get_to(c.RunAccelerationOnFoot);
		j.at("RunAccelerationMidAir").get_to(c.RunAccelerationMidAir);
		j.at("RunSpeedLimit").get_to(c.RunSpeedLimit);
		j.at("CrouchAcceleration").get_to(c.CrouchAcceleration);
		j.at("CrouchSpeedLimit").get_to(c.CrouchSpeedLimit);
		j.at("DefaultFrictionStopSpeed").get_to(c.DefaultFrictionStopSpeed);
		j.at("DefaultFriction").get_to(c.DefaultFriction);
		j.at("AirResistanceStopSpeed").get_to(c.AirResistanceStopSpeed);
		j.at("DefaultAirResistance").get_to(c.DefaultAirResistance);
		j.at("SlidingFrictionStopSpeed").get_to(c.SlidingFrictionStopSpeed);
		j.at("SlidingFriction").get_to(c.SlidingFriction);
		j.at("SlidingImpulse").get_to(c.SlidingImpulse);
		j.at("ThresholdSpeedToSlide").get_to(c.ThresholdSpeedToSlide);
		j.at("JumpStartImpulse").get_to(c.JumpStartImpulse);
		j.at("JumpContinuousAcceleration").get_to(c.JumpContinuousAcceleration);
		j.at("DoubleJumpStartImpulse").get_to(c.DoubleJumpStartImpulse);
		j.at("DoubleJumpContinuousAcceleration").get_to(c.DoubleJumpContinuousAcceleration);
		j.at("WallJumpStartImpulse").get_to(c.WallJumpStartImpulse);
		j.at("WallJumpStartSideImpulse").get_to(c.WallJumpStartSideImpulse);
		j.at("WallJumpContinuousAcceleration").get_to(c.WallJumpContinuousAcceleration);
		j.at("VariableJumpTicks").get_to(c.VariableJumpTicks);
		j.at("VariableDoubleJumpTicks").get_to(c.VariableDoubleJumpTicks);
		j.at("VariableWallJumpTicks").get_to(c.VariableWallJumpTicks);
		j.at("JumpBufferTicks").get_to(c.JumpBufferTicks);
		j.at("CoyoteTimeTicks").get_to(c.CoyoteTimeTicks);
		j.at("WallSlideSpeed").get_to(c.WallSlideSpeed);
		j.at("SlidingCollidorFactor").get_to(c.SlidingCollidorFactor);
	}
};