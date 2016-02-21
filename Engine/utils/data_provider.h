#ifndef INC_UTILS_DATA_PROVIDER_H_
#define INC_UTILS_DATA_PROVIDER_H_

// --------------------------------------
class CDataProvider {
public:
	virtual ~CDataProvider() { }
	virtual bool isValid() = 0;
	virtual void readBytes(void* dst, size_t num_bytes) = 0;

	template< class TPOD >
	void read(TPOD& dst) {
		readBytes(&dst, sizeof(TPOD));
	}
};

// --------------------------------------
class CFileDataProvider : public CDataProvider {
	FILE* f;
public:
	CFileDataProvider(const char *filename) {
		f = fopen(filename, "rb");
	}
	~CFileDataProvider() {
		if (f)
			fclose(f), f = nullptr;
	}
	bool isValid() override {
		return f != nullptr;
	}
	void readBytes(void* dst, size_t num_bytes) override {
		auto bytes_read = ::fread(dst, 1, num_bytes, f);
		assert(bytes_read == num_bytes);
	}
};

#endif
