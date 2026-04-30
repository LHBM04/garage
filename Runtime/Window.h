#pragma once

#include <limits>

#include "Service.h"

class Window final : public Service
{
public:
	using Handle = HWND;
	using Id = std::uint32_t;

	static constexpr Id InvalidId = std::numeric_limits<Id>::max();

	struct Options final
	{
		std::wstring title = L"Garage";
		int width = 1280;
		int height = 720;
		bool fullscreen = false;
		bool resizeable = true;
		bool borderless = false;
		bool visible = true;
	};

	Window() noexcept = default;
	~Window() noexcept override = default;

	[[nodiscard]] bool Contains(const Id& id) const noexcept;
	[[nodiscard]] std::size_t GetCount() const noexcept;

	[[nodiscard]] Handle GetHandle(const Id& id) const noexcept;

	[[nodiscard]] std::wstring GetTitle(const Id& id) const;
	void SetTitle(const Id& id, const std::wstring& title);

	[[nodiscard]] std::pair<int, int> GetSize(const Id& id) const noexcept;
	void SetSize(const Id& id, int width, int height);

	Id Create(const Options& options);
	void Destroy(const Id& id);

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnAdd() override;
	void OnRemove() override;

	[[nodiscard]] Handle Find(const Id& id) const noexcept;
	[[nodiscard]] DWORD GetStyle(const Options& options) const noexcept;
	[[nodiscard]] DWORD GetStyle(Handle handle) const noexcept;

	LRESULT HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void RemoveByHandle(HWND hWnd) noexcept;

	static std::optional<WNDCLASSEXW> gWindowClass;
	static Window* gInstance;
	static Id gNextId;

	std::unordered_map<Id, Handle> mHandles;
	std::unordered_map<Handle, Id> mIds;
};
