#include "stdafx.h"
#include "SplomLoader.h"

SplomLoader& SplomLoader::getInstance() {
	static SplomLoader instance;
	return instance;
}

void SplomLoader::write(const std::string& path) {

	auto generator = getInMemoryGenerator(path);

	for (int l = 0; l < 1000000000; ++l) {

		float d0, d1, d2, d3, d4;
		
		d0 = randNormal(10, 10);
		d1 = randNormal(10, 10);
		d2 = randNormal(d0, 10);
		d3 = (log(fabs(d0) + 1) + randUniform(3, 1));
		d4 = randNormal(10, 10);
		
		generator.addInt("dim0_10", util::linearScale(-40.f, 60.f, 0, 10, d0));
		generator.addInt("dim1_10", util::linearScale(-40.f, 60.f, 0, 10, d1));
		generator.addInt("dim2_10", util::linearScale(-70.f, 80.f, 0, 10, d2));
		generator.addInt("dim3_10", util::linearScale(1.f, 7.f, 0, 10, d3));
		generator.addInt("dim4_10", util::linearScale(-40.f, 60.f, 0, 10, d4));

		generator.addInt("dim0_20", util::linearScale(-40.f, 60.f, 0, 20, d0));
		generator.addInt("dim1_20", util::linearScale(-40.f, 60.f, 0, 20, d1));
		generator.addInt("dim2_20", util::linearScale(-70.f, 80.f, 0, 20, d2));
		generator.addInt("dim3_20", util::linearScale(1.f, 7.f, 0, 20, d3));
		generator.addInt("dim4_20", util::linearScale(-40.f, 60.f, 0, 20, d4));

		generator.addInt("dim0_30", util::linearScale(-40.f, 60.f, 0, 30, d0));
		generator.addInt("dim1_30", util::linearScale(-40.f, 60.f, 0, 30, d1));
		generator.addInt("dim2_30", util::linearScale(-70.f, 80.f, 0, 30, d2));
		generator.addInt("dim3_30", util::linearScale(1.f, 7.f, 0, 30, d3));
		generator.addInt("dim4_30", util::linearScale(-40.f, 60.f, 0, 30, d4));

		generator.addInt("dim0_40", util::linearScale(-40.f, 60.f, 0, 40, d0));
		generator.addInt("dim1_40", util::linearScale(-40.f, 60.f, 0, 40, d1));
		generator.addInt("dim2_40", util::linearScale(-70.f, 80.f, 0, 40, d2));
		generator.addInt("dim3_40", util::linearScale(1.f, 7.f, 0, 40, d3));
		generator.addInt("dim4_40", util::linearScale(-40.f, 60.f, 0, 40, d4));

		generator.addInt("dim0_50", util::linearScale(-40.f, 60.f, 0, 50, d0));
		generator.addInt("dim1_50", util::linearScale(-40.f, 60.f, 0, 50, d1));
		generator.addInt("dim2_50", util::linearScale(-70.f, 80.f, 0, 50, d2));
		generator.addInt("dim3_50", util::linearScale(1.f, 7.f, 0, 50, d3));
		generator.addInt("dim4_50", util::linearScale(-40.f, 60.f, 0, 50, d4));

		generator.execute();
	}

	generator.commit();
}

InMemoryDataGenerator SplomLoader::getInMemoryGenerator(const std::string& path) {
	std::vector<tuple_t> tokens;

	tokens.emplace_back("dim0_10", "dim0_10", Categorical);
	tokens.emplace_back("dim1_10", "dim1_10", Categorical);
	tokens.emplace_back("dim2_10", "dim2_10", Categorical);
	tokens.emplace_back("dim3_10", "dim3_10", Categorical);
	tokens.emplace_back("dim4_10", "dim4_10", Categorical);

	tokens.emplace_back("dim0_20", "dim0_20", Categorical);
	tokens.emplace_back("dim1_20", "dim1_20", Categorical);
	tokens.emplace_back("dim2_20", "dim2_20", Categorical);
	tokens.emplace_back("dim3_20", "dim3_20", Categorical);
	tokens.emplace_back("dim4_20", "dim4_20", Categorical);

	tokens.emplace_back("dim0_30", "dim0_30", Categorical);
	tokens.emplace_back("dim1_30", "dim1_30", Categorical);
	tokens.emplace_back("dim2_30", "dim2_30", Categorical);
	tokens.emplace_back("dim3_30", "dim3_30", Categorical);
	tokens.emplace_back("dim4_30", "dim4_30", Categorical);

	tokens.emplace_back("dim0_40", "dim0_40", Categorical);
	tokens.emplace_back("dim1_40", "dim1_40", Categorical);
	tokens.emplace_back("dim2_40", "dim2_40", Categorical);
	tokens.emplace_back("dim3_40", "dim3_40", Categorical);
	tokens.emplace_back("dim4_40", "dim4_40", Categorical);

	tokens.emplace_back("dim0_50", "dim0_50", Categorical);
	tokens.emplace_back("dim1_50", "dim1_50", Categorical);
	tokens.emplace_back("dim2_50", "dim2_50", Categorical);
	tokens.emplace_back("dim3_50", "dim3_50", Categorical);
	tokens.emplace_back("dim4_50", "dim4_50", Categorical);

	return InMemoryDataGenerator(path, tokens);
}

