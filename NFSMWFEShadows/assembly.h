#pragma once

DWORD sub_7A1FA0 = 0x7A1FA0;
DWORD sub_7A2200 = 0x7A2200;
DWORD sub_750980 = 0x750980;
DWORD sub_723FA0 = 0x723FA0;
DWORD loc_6E518B = 0x6E518B;

void __declspec(naked) FEShadowCodeCave()
{
	__asm
	{
		call sub_723FA0
		cmp byte ptr ds : [0x925E90], 0x03
		jne ExitCode // jumps if not frontend
		cmp byte ptr ds : [0x901830], 0x00
		jle ExitCode // jumps if shadows are disabled
		call sub_7A1FA0
		cmp eax, 0x00
		je ExitCode

	FE_WorldShadow:
		cmp byte ptr ds : [RenderWorldShadows], 0x00
		jle FE_CarShadow // jumps if RenderWorldShadows is disabled
		push 0x04
		mov ecx, eax
		call sub_7A2200

	FE_CarShadow:
		cmp byte ptr ds : [RenderCarShadows], 0x00
		jle ExitCode // jumps if RenderCarShadow is disabled
		push 0x00
		push 0x919C70
		call sub_750980
		add esp, 0x08

	ExitCode:
		jmp loc_6E518B
	}
}

DWORD loc_74E386 = 0x74E386;

void __declspec(naked) FESelfShadowingCodeCave()
{
	__asm
	{
		push eax
		mov eax, dword ptr ds : [0x982A20]
		mov eax, dword ptr ds : [eax + 0x04]
		imul eax, eax, 0x70
		mov eax, dword ptr ds : [eax + 0x9195E4]
		cmp eax, 0x01
		je ExitCode // jumps if not main render

	FE_CarShadow:
		cmp dword ptr ds : [0x925E90], 0x03
		jne ExitCode // jumps if not frontend
		mov cl, 0x00

	ExitCode:
		pop eax
		mov dword ptr ds : [esp + 0xDC], eax
		jmp loc_74E386
	}
}

DWORD sub_50D320 = 0x50D320;

void __declspec(naked) FEParticleSpeedCodeCave()
{
	__asm
	{
		push eax
		mov eax, dword ptr ds : [0x982A20]
		mov eax, dword ptr ds : [eax + 0x04]
		imul eax, eax, 0x70
		mov eax, dword ptr ds : [eax + 0x9195E4]
		cmp eax, 0x01
		je Main

	NotMain:
		pop eax
		push ecx
		push edi
		pop edi
		pop ecx
		ret 4

	Main:
		pop eax
		jmp sub_50D320
	}
}

DWORD loc_6E0BED = 0x6E0BED;
DWORD loc_6E0B76 = 0x6E0B76;

void __declspec(naked) FEShadowRenderFixCodeCave()
{
	__asm
	{
		push eax
		cmp byte ptr ds : [0x925E90] , 0x03
		jne ExitCode // jumps if not frontend
		cmp byte ptr ds : [0x901830] , 0x00
		jle ExitCode // jumps if shadows are not enabled
		mov eax, dword ptr ds : [ebp + 0x18]
		mov eax, dword ptr ds : [eax]
		
	RenderShadowsOnTexture:
		pop eax
		movsx eax, dl
		mov dword ptr ds : [esi + 0x38], eax
		mov ebx, dword ptr ds : [esi + 0x38]
		mov edx, dword ptr ds : [esi + 0x0C]
		xor ebx, edx
		mov edx, dword ptr ds : [0x982A20]
		mov dword ptr ds : [esi + 0x38], ebx
		// Writes Rendering Order
		mov ebx, dword ptr ds : [ebp + 0x18]
		mov ebx, dword ptr ds : [ebx]
		movzx ebx, byte ptr ds : [ebx + 0x51]
		mov byte ptr ds : [esi + 0x3B], bl
		// Writes Rendering Order
		jmp loc_6E0BED

	ExitCode:
		pop eax
		movsx eax, dl
		or eax, 0x80000000
		jmp loc_6E0B76
	}
}

DWORD ShadowsTransformCodeCaveExit = 0x6C8329;

void __declspec(naked) ShadowsTransformCodeCave()
{
	__asm
	{
		cmp byte ptr ds : [0x925E90] , 0x03
		jne ExitCode // jumps if not frontend
		cmp byte ptr ds : [0x901830] , 0x00
		jle ExitCode // jumps if shadows are not enabled
		push eax
		mov eax, dword ptr ds : [0x982C80]
		cmp byte ptr ds : [eax + 0x04], 0x04
		pop eax
		jne ExitCode
		mov dword ptr ds : [esp + 0x54], 0xFFC00000 // NaN

	ExitCode:
		lea esi, dword ptr ds : [esp + 0x50]
		mov edx, dword ptr ds : [eax]
		jmp ShadowsTransformCodeCaveExit
	}
}

DWORD ShadowBiasCodeCaveExit = 0x6E5521;
int ShadowResDivider = 2048;
float ShadowResResult;
float ShadowBiasMultiplier = 2.0f;
float ShadowBiasResult;

void __declspec(naked) ShadowBiasCodeCave()
{
	__asm
	{
		cmp byte ptr ds : [0x925E90] , 0x03
		jne ExitCode // jumps if not frontend
		fild dword ptr ds : [0x93DB3C] // Loads Shadow Res X
		fidiv dword ptr ds : [ShadowResDivider] // Divides by 2048
		fstp dword ptr ds : [ShadowResResult] // Stores Result
		fld dword ptr ds : [ShadowResResult] // Loads Result
		fmul dword ptr ds : [ShadowBiasMultiplier] // Multiplies by 2.0f
		fstp dword ptr ds : [ShadowBiasResult] // Stores Result
		
		cmp dword ptr ds : [ShadowBiasResult], 0x40000000
		jl DefaultShadowBias // jumps if ShadowBias is less than 2.0f
		mov eax, dword ptr ds : [ShadowBiasResult]
		mov dword ptr ds : [esp + 0x08], eax
		jmp ExitCode

	DefaultShadowBias:
		mov dword ptr ds : [esp + 0x08], 0x40000000 // 2.0f

	ExitCode:
		call dword ptr ds : [ecx + 0xE4]
		jmp ShadowBiasCodeCaveExit
	}
}