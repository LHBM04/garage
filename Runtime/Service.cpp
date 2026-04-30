#include "Precompiled.h"
#include "Service.h"

Service::Service() noexcept
	: mApplication(nullptr)
{
}

Application* const Service::GetApplication() const noexcept
{
	return mApplication;
}

void Service::Add(Application* const app)
{
	mApplication = app;
	OnAdd();
}

void Service::Remove()
{
	OnRemove();
	mApplication = nullptr;
}
