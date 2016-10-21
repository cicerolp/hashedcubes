#pragma once

class Data {
public:
	Data() = default;
	virtual ~Data() = default;

	void initialize(ulong size);

	void sortHash(ulong fromIndex, ulong toIndex);

	virtual void dispose() = 0;

	virtual void prepareKey(const std::string& key) = 0;
	virtual void prepareKey(const std::string& str, const std::vector<ulong>& key) = 0;

	inline int getIntProperty(ulong id) { return getIntProperty(id, 0); };
	inline float getFloatProperty(ulong id) { return getFloatProperty(id, 0); };

	virtual int getIntProperty(ulong id, ulong key) = 0;
	virtual float getFloatProperty(ulong id, ulong key) = 0;

	void setHash(ulong id, unsigned short value);

	inline bool comparator(ulong bit0, ulong bit1) const {
		return _hash[bit0] < _hash[bit1];
    }

	virtual ulong size() const = 0;

protected:
	unsigned long getHash(ulong index) const;

	data_container<ulong>::result _index;
	data_container<unsigned short>::result _hash;

private:
};

/* Local Variables:    */
/* mode: c             */
/* indent-tabs-mode: t */
/* c-basic-offset: 8   */
/* tab-width: 8        */
/* End:                */
