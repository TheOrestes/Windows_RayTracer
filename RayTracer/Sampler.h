#pragma once

#include <vector>
#include <string>
#include "glm/glm.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
class Sampler
{
public:
	Sampler(); 
	virtual ~Sampler() {};

	// pure virtual function to be implemented by derived class!
	virtual void					GenerateSamples(uint32_t _numSamples) = 0;

	virtual void					MapSamplesToDisk();
	virtual void					MapSamplesToHemisphere(const float e);

	virtual std::vector<glm::vec2>  GetSamples()			{ return m_vecSamples; };
	virtual std::vector<glm::vec2>	GetDiskSamples()		{ return m_vecDiskSamples; }
	virtual std::vector<glm::vec3>	GetHemisphereSamples()	{ return m_vecHemisphereSamples; }
	virtual std::string				GetName()				{ return m_strName; }

protected:
	uint32_t						m_uiNumSamples;
	std::vector<glm::vec2>			m_vecSamples;
	std::vector<glm::vec2>			m_vecDiskSamples;
	std::vector<glm::vec3>			m_vecHemisphereSamples;
	std::string						m_strName;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RandomSampler : public Sampler
{
public:
	RandomSampler() {};
	~RandomSampler() {};

	virtual void					GenerateSamples(uint32_t _numSamples);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class RegularSampler : public Sampler
{
public:
	RegularSampler() {};
	~RegularSampler() {};

	virtual void					GenerateSamples(uint32_t _numSamples);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
class JitteredSampler : public Sampler
{
public:
	JitteredSampler() {};
	~JitteredSampler() {};

	virtual void					GenerateSamples(uint32_t _numSamples);
};


