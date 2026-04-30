#pragma once

#include "Layer.h"
#include "Service.h"

class Application final
{
public:
	struct Options final
	{

	};

	/**
	 * @brief 생성자.
	 *
	 * @param options 생성할 애플리케이션의 설정.
	 */
	explicit Application(const Options& options) noexcept;

	/**
	 * @brief 소멸자.
	 */
	~Application() noexcept;

	template <std::derived_from<Service> TService>
	inline TService* const AddService();

	template <std::derived_from<Service> TService>
	[[nodiscard]] inline TService* const GetService() const noexcept;

	template <std::derived_from<Service> TService>
	[[nodiscard]] inline bool HasService() const noexcept;

	template <std::derived_from<Service> TService>
	inline void RemoveService();

	/**
	 * @brief 해당 애플리케이션을 초기화합니다.
	 *
	 * @return 초기화 성공 여부.
	 */
	bool Initialize();

	/**
	 * @brief 해당 애플리케이션을 종료합니다.
	 */
	void Shutdown();

	/**
	 * @brief 해당 애플리케이션을 실행합니다.
	 *
	 * @return int 종료 코드
	 */
	int Run();

private:
	/**
	 * @brief 해당 애플리케이션 내에 추가된 서비스들.
	 */
	std::unordered_map<std::type_index, std::unique_ptr<Service>> mServices;

	/**
	 * @brief 해당 애플리케이션 내에 추가된 레이어들.
	 */
	std::unordered_map<std::type_index, std::unique_ptr<Layer>> mLayers;
};

template <std::derived_from<Service> TService>
inline TService* const Application::AddService()
{
	if (std::type_index index(typeid(TService)); !mServices.contains(index))
	{
		mServices[index] = std::make_unique<TService>();
		mServices[index]->Add(this);

		return static_cast<TService*>(mServices[index].get());
	}

	return nullptr;
}

template <std::derived_from<Service> TService>
inline TService* const Application::GetService() const noexcept
{
	if (std::type_index index(typeid(TService)); mServices.contains(index))
	{
		return static_cast<TService*>(mServices.at(index).get());
	}

	return nullptr;
}

template <std::derived_from<Service> TService>
inline bool Application::HasService() const noexcept
{
	return mServices.contains(std::type_index(typeid(TService)));
}

template <std::derived_from<Service> TService>
inline void Application::RemoveService()
{
	if (std::type_index index(typeid(TService)); mServices.contains(index))
	{
		mServices[index]->Remove();
		mServices.erase(index);
	}
}
