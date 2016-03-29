#pragma once

#include "Trunc.h"
#include "DataLoader.h"
#include "InMemoryDataGenerator.h"

class SplomLoader : public DataLoader {
public:
	static SplomLoader& getInstance();
	virtual void write(const std::string& path) override;

protected:
	virtual InMemoryDataGenerator getInMemoryGenerator(const std::string& path) override;
	
private:
	inline float randUniform(int min, int max);
	inline float randNormal(float mean, float stdev);

	SplomLoader() = default;
	~SplomLoader() = default;

	std::default_random_engine _random_engine;
	std::uniform_real_distribution<double> _uniform_dist {0.0, 1.0};
};

float SplomLoader::randUniform(int min, int max) {
	float delta = max - min;
	return (float)(min + delta * _uniform_dist(_random_engine));
}

float SplomLoader::randNormal(float mean, float stdev) {
	float x = 0, y = 0, rds, c;
	do {
		x = (float)(_uniform_dist(_random_engine) * 2 - 1);
		y = (float)(_uniform_dist(_random_engine) * 2 - 1);
		rds = x * x + y * y;
	} while (rds == 0 || rds > 1);
	c = (float)sqrt(-2 * log(rds) / rds);
	return mean + x * c * stdev;
}
