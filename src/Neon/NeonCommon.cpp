#include <Neon/NeonCommon.h>

void _CheckGLError(const char* file, int line)
{
	GLenum err(glGetError());

	while (err != GL_NO_ERROR)
	{
		std::string error;
		switch (err)
		{
		case GL_INVALID_OPERATION:  error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:       error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:      error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:      error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}
		std::cout << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
		err = glGetError();
	}

	return;
}

namespace Neon
{
	time_point<high_resolution_clock> Time::Now()
	{
		return high_resolution_clock::now();
	}

	double Time::DeltaNano(const time_point<high_resolution_clock>& t)
	{
		return double(duration_cast<nanoseconds>(high_resolution_clock::now() - t).count());
	}

	double Time::DeltaNano(const time_point<high_resolution_clock>& t0, const time_point<high_resolution_clock>& t1)
	{
		return double(duration_cast<nanoseconds>(t1 - t0).count());
	}

	double Time::DeltaMicro(const time_point<high_resolution_clock>& t)
	{
		return double(duration_cast<nanoseconds>(high_resolution_clock::now() - t).count()) / 1000.0;
	}

	double Time::DeltaMicro(const time_point<high_resolution_clock>& t0, const time_point<high_resolution_clock>& t1)
	{
		return double(duration_cast<nanoseconds>(t1 - t0).count()) / 1000.0;
	}

	double Time::DeltaMili(const time_point<high_resolution_clock>& t)
	{
		return double(duration_cast<nanoseconds>(high_resolution_clock::now() - t).count()) / 1000000.0;
	}

	double Time::DeltaMili(const time_point<high_resolution_clock>& t0, const time_point<high_resolution_clock>& t1)
	{
		return double(duration_cast<nanoseconds>(t1 - t0).count()) / 1000000.0;
	}

	Time::Time(const string& name)
		: name(name)
	{
		startedTime = Now();
		touchedTime = startedTime;
	}

	Time::~Time()
	{
		Stop();
	}

	void Time::Stop()
	{
		if (stoped == false)
		{
			if (name.empty() == false)
			{
				cout << "[" << name << "] ";
			}
			cout << DeltaMili(startedTime) << " miliseconds" << endl;
			stoped = true;
		}
	}

	void Time::Touch()
	{
		touchCount++;
		auto now = Now();

		if (name.empty() == false)
		{
			cout << "[" << name << " : " << touchCount << "] ";
		}
		else
		{
			cout << "[" << touchCount << "] ";
		}
		cout << DeltaMili(touchedTime, now) << " miliseconds" << endl;

		touchedTime = now;
	}

	unsigned int NextPowerOf2(unsigned int n)
	{
		unsigned int p = 1;
		if (n && !(n & (n - 1)))
			return n;

		while (p < n)
			p <<= 1;

		return p;
	}
}