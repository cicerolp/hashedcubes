#pragma once

class DataGenerator {
public:
	DataGenerator(const std::string& path, const std::vector<tuple_t> tokens) : _path(path), _tokens(tokens) { }
	virtual ~DataGenerator() = default;

	inline const std::string& path() const;
	
	virtual void addInt(const std::string& key, ulong value) = 0;
	virtual void addFloat(const std::string& key, float value) = 0;

	virtual void execute() = 0;
	virtual void commit() = 0;
	
protected:
	std::string _path;
	const std::vector<tuple_t> _tokens;
	
private:
};

const std::string& DataGenerator::path() const {
	return _path;
}
