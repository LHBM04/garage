#pragma once

class Application;

class Service
{
	friend class Application;
public:
	virtual ~Service() noexcept = default;

	/**
	 * @brief 해당 서비스가 속한 애플리케이션을 반환합니다.
	 * 
	 * @return Application* 해당 서비스가 속한 애플리케이션.
	 */
	[[nodiscard]] Application* const GetApplication() const noexcept;

	void Add(Application* const app);
	void Remove();

protected:
	Service() noexcept;

	/**
	 * @brief 해당 서비스가 추가될 때 호출됩니다.
	 */
	virtual void OnAdd() = 0;

	/**
	 * @brief 해당 서비스가 제거될 때 호출됩니다.
	 */
	virtual void OnRemove() = 0;

private:
	Service(const Service&) = delete;
	Service& operator=(const Service&) = delete;

	Service(Service&&) = delete;
	Service& operator=(Service&&) = delete;

	/**
	 * @brief 해당 서비스가 속한 애플리케이션.
	 */
	Application* mApplication;
};
