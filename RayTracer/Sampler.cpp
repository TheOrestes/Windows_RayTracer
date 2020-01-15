
#include "Sampler.h"
#include "Helper.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
Sampler::Sampler()
{
	m_vecSamples.clear();
	m_vecDiskSamples.clear();
	m_vecHemisphereSamples.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Sampler::MapSamplesToDisk()
{
	float r, phi;						// polar coordinates
	glm::vec2 sample = glm::vec2(0,0);	// sample point on unit disk

	uint32_t size = m_vecSamples.size();

	m_vecDiskSamples.reserve(size);

	for (int i = 0; i < size; ++i)
	{
		// map sample point to [-1,1]

		sample.x = 2.0f * m_vecSamples[i].x - 1.0f;
		sample.y = 2.0f * m_vecSamples[i].y - 1.0f;

		if (sample.x > -sample.y)
		{
			// sector 1 & 2
			if (sample.x > sample.y)
			{
				// sector 1
				r = sample.x;
				phi = sample.y / sample.x;
			}
			else
			{
				// sector 2
				r = sample.y;
				phi = 2.0f - (sample.x / sample.y);
			}
		}
		else
		{
			// sector 3 & 4
			if (sample.x < sample.y)
			{
				// sector 3
				r = -sample.x;
				phi = 4 + (sample.y / sample.x);
			}
			else
			{
				// sector 4
				r = -sample.y;
				if (sample.y != 0.0f)
				{
					phi = 6 - (sample.x / sample.y);
				}
				else
				{
					phi = 0.0f;
				}
			}
		}

		phi *= PI / 4.0f;

		m_vecDiskSamples[i].x = r * cos(phi);
		m_vecDiskSamples[i].y = r * sin(phi);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Sampler::MapSamplesToHemisphere(const float e)
{
	uint32_t size = m_vecSamples.size();
	m_vecHemisphereSamples.reserve(size);

	for (int i = 0; i < size; ++i)
	{
		float cos_phi = cos(2.0f * PI * m_vecSamples[i].x);
		float sin_phi = sin(2.0f * PI * m_vecSamples[i].x);

		float cos_theta = pow((1.0f - m_vecSamples[i].y), 1.0f / (e + 1.0f));
		float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

		float pu = sin_theta * cos_phi;
		float pv = sin_theta * sin_phi;
		float pw = cos_theta;

		m_vecHemisphereSamples.push_back(glm::vec3(pu, pv, pw));
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RandomSampler::GenerateSamples(uint32_t _numSamples)
{
	m_strName = "RandomSampler";
	m_uiNumSamples = _numSamples;
	m_vecSamples.reserve(m_uiNumSamples);

	for (uint32_t i = 0; i < m_uiNumSamples; ++i)
	{
		glm::vec2 sample = glm::vec2(Helper::GetRandom01(), Helper::GetRandom01());
		m_vecSamples.push_back(sample);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void RegularSampler::GenerateSamples(uint32_t _numSamples)
{
	m_strName = "RegularSampler";
	m_uiNumSamples = _numSamples;
	m_vecSamples.reserve(m_uiNumSamples);

	uint32_t numSets;				// horizontal or vertical sections
	uint32_t numGrids;				// total number of grids in a pixel
	uint32_t numSamplesPerGrid;		// per grid samples!

	// derive number of sets based on sample count to avoid zero samples per grid!
	numSets = (m_uiNumSamples < 4) ? 1 : sqrt(m_uiNumSamples);
	
	numGrids = numSets * numSets;
	numSamplesPerGrid = m_uiNumSamples / numGrids;
	
	for (uint32_t i = 0; i < numSets; ++i)
	{
		for (uint32_t j = 0; j < numSets; ++j)
		{
			for (uint32_t k = 0; k < numSamplesPerGrid; ++k)
			{
				// This loop doesn't really matter since in Regular
				// sampling, all the points will lie on same location!
				// However, I have addded it to keep in sync with logic 
				// in Jittered sampler!
				float x = (i + 0.5f) / numSets;
				float y = (j + 0.5f) / numSets;
				glm::vec2 sample = glm::vec2(x, y);
				m_vecSamples.push_back(sample);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void JitteredSampler::GenerateSamples(uint32_t _numSamples)
{
	m_strName = "JitteredSampler";
	m_uiNumSamples = _numSamples;
	m_vecSamples.reserve(m_uiNumSamples);

	uint32_t numSets;				// horizontal or vertical sections
	uint32_t numGrids;				// total number of grids in a pixel
	uint32_t numSamplesPerGrid;		// per grid samples!

	// derive number of sets based on sample count to avoid zero samples per grid!
	numSets = (m_uiNumSamples < 4) ? 1 : sqrt(m_uiNumSamples);

	numGrids = numSets * numSets;
	numSamplesPerGrid = m_uiNumSamples / numGrids;

	for (uint32_t i = 0; i < numSets; ++i)
	{
		for (uint32_t j = 0; j < numSets; ++j)
		{
			for (uint32_t k = 0; k < numSamplesPerGrid; ++k)
			{
				// This loop doesn't really matter since in Regular
				// sampling, all the points will lie on same location!
				// However, I have addded it to keep in sync with logic 
				// in Jittered sampler!
				float x = (i + Helper::GetRandom01()) / numSets;
				float y = (j + Helper::GetRandom01()) / numSets;
				glm::vec2 sample = glm::vec2(x, y);
				m_vecSamples.push_back(sample);
			}
		}
	}
}

