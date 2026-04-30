#include "Precompiled.h"
#include "Window.h"

bool Window::Contains(const Window::Id& id) const noexcept
{
	return mHandles.contains(id);
}

std::size_t Window::GetCount() const noexcept
{
	return mHandles.size();
}

Window::Handle Window::GetHandle(const Window::Id& id) const noexcept
{
	return Find(id);
}

std::wstring Window::GetTitle(const Window::Id& id) const
{
	const Handle handle = GetHandle(id);
	if (!handle)
	{
		return {};
	}

	const int length = GetWindowTextLengthW(handle);
	if (length <= 0)
	{
		return {};
	}

	std::wstring title(static_cast<std::size_t>(length) + 1, L'\0');
	GetWindowTextW(handle, title.data(), length + 1);
	title.resize(static_cast<std::size_t>(length));
	return title;
}

void Window::SetTitle(const Window::Id& id, const std::wstring& title)
{
	const Handle handle = GetHandle(id);
	if (!handle)
	{
		return;
	}

	SetWindowTextW(handle, title.c_str());
}

std::pair<int, int> Window::GetSize(const Window::Id& id) const noexcept
{
	const Handle handle = GetHandle(id);
	if (!handle)
	{
		return { 0, 0 };
	}

	RECT rect{};
	if (!GetClientRect(handle, &rect))
	{
		return { 0, 0 };
	}

	return { rect.right - rect.left, rect.bottom - rect.top };
}

void Window::SetSize(const Window::Id& id, int width, int height)
{
	const Handle handle = GetHandle(id);
	if (!handle)
	{
		return;
	}

	RECT rect{ 0, 0, width, height };
	AdjustWindowRect(&rect, GetStyle(handle), FALSE);

	SetWindowPos(
		handle,
		nullptr,
		0,
		0,
		rect.right - rect.left,
		rect.bottom - rect.top,
		SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

Window::Id Window::Create(const Window::Options& options)
{
	if (!gWindowClass)
	{
		return InvalidId;
	}

	Id id = gNextId++;
	RECT rect{ 0, 0, options.width, options.height };
	const DWORD style = GetStyle(options);
	AdjustWindowRect(&rect, style, FALSE);

	const Handle handle = CreateWindowExW(
		0,
		gWindowClass->lpszClassName,
		options.title.c_str(),
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		gWindowClass->hInstance,
		&id);

	if (!handle)
	{
		return InvalidId;
	}

	mHandles.emplace(id, handle);
	if (options.visible)
	{
		ShowWindow(handle, SW_SHOW);
		UpdateWindow(handle);
	}

	return id;
}

void Window::Destroy(const Window::Id& id)
{
	const Handle handle = GetHandle(id);
	if (!handle)
	{
		return;
	}

	DestroyWindow(handle);
	RemoveByHandle(handle);
}

LRESULT Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		const auto* create = reinterpret_cast<CREATESTRUCTW*>(lParam);
		const auto* id = static_cast<Id*>(create->lpCreateParams);
		if (id && gInstance)
		{
			gInstance->mIds.emplace(hWnd, *id);
		}
	}

	if (gInstance)
	{
		return gInstance->HandleMessage(hWnd, uMsg, wParam, lParam);
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void Window::OnAdd()
{
	gInstance = this;
	if (gWindowClass)
	{
		return;
	}

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = GetModuleHandleW(nullptr);
	wc.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
	wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
	wc.lpszClassName = L"GarageWindowClass";
	wc.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);

	if (!RegisterClassExW(&wc))
	{
		gInstance = nullptr;
		return;
	}

	gWindowClass.emplace(wc);
}

void Window::OnRemove()
{
	std::vector<Handle> handles;
	handles.reserve(mHandles.size());
	for (const auto& [id, handle] : mHandles)
	{
		handles.push_back(handle);
	}

	for (Handle handle : handles)
	{
		if (handle)
		{
			DestroyWindow(handle);
		}
	}

	mHandles.clear();
	mIds.clear();

	if (gWindowClass)
	{
		UnregisterClassW(gWindowClass->lpszClassName, gWindowClass->hInstance);
		gWindowClass.reset();
	}

	if (gInstance == this)
	{
		gInstance = nullptr;
	}
}

Window::Handle Window::Find(const Window::Id& id) const noexcept
{
	const auto iter = mHandles.find(id);
	return iter != mHandles.end() ? iter->second : nullptr;
}

DWORD Window::GetStyle(const Window::Options& options) const noexcept
{
	if (options.fullscreen || options.borderless)
	{
		return WS_POPUP;
	}

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	if (options.resizeable)
	{
		style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
	}

	return style;
}

DWORD Window::GetStyle(const Window::Handle handle) const noexcept
{
	if (!handle)
	{
		return 0;
	}

	return static_cast<DWORD>(GetWindowLongPtrW(handle, GWL_STYLE));
}

LRESULT Window::HandleMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_NCDESTROY:
		RemoveByHandle(hWnd);
		if (mHandles.empty())
		{
			PostQuitMessage(0);
		}
		return 0;
	}

	return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

void Window::RemoveByHandle(HWND hWnd) noexcept
{
	const auto idIter = mIds.find(hWnd);
	if (idIter == mIds.end())
	{
		return;
	}

	mHandles.erase(idIter->second);
	mIds.erase(idIter);
}

std::optional<WNDCLASSEXW> Window::gWindowClass{ std::nullopt };

Window* Window::gInstance{ nullptr };

Window::Id Window::gNextId{ 0 };
