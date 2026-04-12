#include <scythe/opengl/opengl_provider.h>

namespace scythe {

	static constexpr uint8_t kDefaultContextMajorVersion = 3;
	static constexpr uint8_t kDefaultContextMinorVersion = 3;
	static constexpr bool kDefaultUseMultisample = false;
	static constexpr uint8_t kDefaultMSAASamplesCount = 4;

	OpenGLGraphicsProvider::OpenGLGraphicsProvider(Application* application)
	: GraphicsProvider(application)
	, context_major_version_(kDefaultContextMajorVersion)
	, context_minor_version_(kDefaultContextMinorVersion)
	, is_multisample_(kDefaultUseMultisample)
	, msaa_samples_(kDefaultMSAASamplesCount)
	{
	}
	uint8_t OpenGLGraphicsProvider::GetContextMajorVersion() const
	{
		return context_major_version_;
	}
	uint8_t OpenGLGraphicsProvider::GetContextMinorVersion() const
	{
		return context_minor_version_;
	}
	bool OpenGLGraphicsProvider::IsMultisample() const
	{
		return is_multisample_;
	}
	uint8_t OpenGLGraphicsProvider::GetMSAASamples() const
	{
		return msaa_samples_;
	}
	void OpenGLGraphicsProvider::SetContextVersion(uint8_t major, uint8_t minor)
	{
		if (initialized_)
			return;
		context_major_version_ = major;
		context_minor_version_ = minor;
	}
	void OpenGLGraphicsProvider::SetMultisample(bool flag)
	{
		if (initialized_)
			return;
		is_multisample_ = flag;
	}
	void OpenGLGraphicsProvider::SetMSAASamples(uint8_t msaa_samples)
	{
		if (initialized_)
			return;
		msaa_samples_ = msaa_samples;
	}

} // namespace scythe