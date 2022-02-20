#include "stdafx.h"
#include "stdio.h"
#include <windows.h>
#include "..\includes\injector\injector.hpp"
#include <cstdint>
#include "..\includes\IniReader.h"
#include <d3d9.h>
#include "settings.h"
#include "assembly.h"

void Init()
{

	// Read values from .ini
	CIniReader iniReader("NFSMWFEShadows.ini");

	// Main
	RenderWorldShadows = iniReader.ReadInteger("MAIN", "RenderWorldShadows", 1);
	RenderCarShadows = iniReader.ReadInteger("MAIN", "RenderCarShadows", 1);
	DisableShadowsOnCars = iniReader.ReadInteger("MAIN", "DisableShadowsOnCars", 1); 

	{
		// Front-End Shadow Render Code
		injector::MakeJMP(0x6E5186, FEShadowCodeCave, true);
		// Fixes Car Self-Shadowing
		injector::MakeJMP(0x74E37F, FESelfShadowingCodeCave, true);
		injector::MakeNOP(0x74E384, 2, true);
		// Fixes Particle Speed
		injector::MakeJMP(0x7A22F2, FEParticleSpeedCodeCave, true);
		// Fixes Shadows Not Rendering On Ground
		injector::MakeJMP(0x6E0B6E, FEShadowRenderFixCodeCave, true);
		injector::MakeNOP(0x6E0B73, 3, true);
		// Overwrites ShadowBias in FE
		// Compatible with Widescreen Fix
		injector::MakeJMP(0x6E551B, ShadowBiasCodeCave, true);
		injector::MakeNOP(0x6E5520, 1, true);
	}

	if (DisableShadowsOnCars)
	{
		// Disables Shadows Rendering On Cars
		injector::MakeJMP(0x6C8323, ShadowsTransformCodeCave, true);
		injector::MakeNOP(0x6C8328, 1, true);
	}
}
	
BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		uintptr_t base = (uintptr_t)GetModuleHandleA(NULL);
		IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)(base);
		IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)(base + dos->e_lfanew);

		if ((base + nt->OptionalHeader.AddressOfEntryPoint + (0x400000 - base)) == 0x7C4040) // Check if .exe file is compatible - Thanks to thelink2012 and MWisBest
			Init();

		else
		{
			MessageBoxA(NULL, "This .exe is not supported.\nPlease use v1.3 English speed.exe (5.75 MB (6.029.312 bytes)).", "NFSMW FE Shadows by Aero_", MB_ICONERROR);
			return FALSE;
		}
	}
	return TRUE;
}