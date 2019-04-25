#pragma once

#include <string>

class Profiler
{
public:
	static Profiler& getInstance()
	{
		static Profiler instance;
		return instance;
	}

	~Profiler();

	std::string GetProfilerTexts() { return m_strInfo; }

	void WriteToProfiler(const char* _inputStr);
	void WriteToProfiler(const std::string& _inputStr);
	void WriteToProfiler(const std::string& _inputStr, float _value);
	void WriteToProfiler(const std::string& _inputStr, uint64_t _value);

private:
	Profiler();

	std::string m_strInfo;
};
