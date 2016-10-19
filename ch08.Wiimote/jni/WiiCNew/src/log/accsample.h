#ifndef ACC_SAMPLE_H
#define ACC_SAMPLE_H

#include "sample.h"

class AccSample : public Sample
{
public:
	AccSample(float x, float y, float z) : Sample() { ax = x; ay = y; az = z; }
	~AccSample() {}
	inline float x() const { return ax;	}
	inline float y() const { return ay;	}
	inline float z() const { return az;	}

private:
	float ax;
	float ay;
	float az;
};

#endif

