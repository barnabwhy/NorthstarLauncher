#pragma once
#include "pch.h"
#include "dedicated.h"
#include "clientmaterialsystem.h"
#include "hookutils.h"
#include "gameutils.h"
#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>
#pragma comment(lib, "d3d11.lib")
#include <d3d11on12.h>
#include <wrl.h>

typedef HRESULT (*__stdcall D3D11CreateDeviceType)(
	void* pAdapter, int DriverType, HMODULE Software, UINT Flags, int* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, void** ppDevice,
	int* pFeatureLevel, void** ppImmediateContext);
D3D11CreateDeviceType _D3D11CreateDevice;

Microsoft::WRL::ComPtr<ID3D12Device> pD3D12Device;
Microsoft::WRL::ComPtr<ID3D12CommandQueue> ppCommandQueue;

HRESULT __stdcall D3D11CreateDeviceHook2(
	void* pAdapter, int DriverType, HMODULE Software, UINT Flags, int* pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, void** ppDevice,
	int* pFeatureLevel, void** ppImmediateContext)
{
	if (CommandLine()->CheckParm("-d3d12"))
	{
		Flags = D3D10_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;
		*pFeatureLevel = D3D_FEATURE_LEVEL_11_1;
		D3D_FEATURE_LEVEL d3d12FeatureLevel = D3D_FEATURE_LEVEL_11_1;

		spdlog::info("Creating D3D12 device");
		D3D12CreateDevice((IUnknown*)pAdapter, d3d12FeatureLevel, IID_PPV_ARGS(&pD3D12Device));

		spdlog::info("Creating D3D12 command queue");
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
		pD3D12Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&ppCommandQueue));

		spdlog::info("Creating D3D11OnD12 device");
		spdlog::info("Feature Level (DX12): {}", d3d12FeatureLevel);
		spdlog::info("Feature Level (DX11): {}", *pFeatureLevel);

		HRESULT hr = D3D11On12CreateDevice(
			pD3D12Device.Get(), Flags, (D3D_FEATURE_LEVEL*)pFeatureLevel, 0, reinterpret_cast<IUnknown**>(ppCommandQueue.GetAddressOf()), 1,
			0, (ID3D11Device**)ppDevice, (ID3D11DeviceContext**)ppImmediateContext, nullptr);

		return hr;
	}
	else
	{
		return _D3D11CreateDevice(
			pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext);
	}
}

void InitialiseClientMaterialSystem(HMODULE baseAddress)
{
	if (IsDedicated())
		return;

	HookEnabler hook;
	ENABLER_CREATEHOOK(hook, (char*)baseAddress + 0xD9A0E, &D3D11CreateDeviceHook2, reinterpret_cast<LPVOID*>(&_D3D11CreateDevice));
}
