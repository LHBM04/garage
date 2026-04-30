#pragma once

#include "Service.h"

class Application;

class Renderer final : public Service
{
public:
	void Clear();
	void Present();

private:
	virtual void OnAdd() override;
	virtual void OnRemove() override;

	Microsoft::WRL::ComPtr<ID3D12Device> m_device;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
};
