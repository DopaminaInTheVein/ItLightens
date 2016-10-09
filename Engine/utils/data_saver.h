#ifndef INC_UTILS_DATA_SAVER_H_
#define INC_UTILS_DATA_SAVER_H_

#include <vector>

// --------------------------------------
class CDataSaver {

public:
  virtual ~CDataSaver() { }
  virtual bool isValid() = 0;
  virtual void writeBytes(const void* src, size_t num_bytes) = 0;

  template< class TPOD >
  void write(TPOD& dst) {
    writeBytes(&dst, sizeof(TPOD));
  }
};

// --------------------------------------
class CMemoryDataSaver : public CDataSaver {
  std::vector< unsigned char > data;
public:
  bool isValid() override {
    return true; 
  }
  void writeBytes(const void* src, size_t num_bytes) override {
    data.resize(data.size() + num_bytes);
    memcpy(&data[data.size() - num_bytes], src, num_bytes);
  }
  size_t size() const { return data.size(); }
  const void *getDataBase() const { return &data[0]; }
  bool writeToFile(const char* ofilename) const {
    FILE *f = fopen(ofilename, "wb");
    if (!f)
      return false;
    auto nbytes = fwrite(&data[0], 1, data.size(), f);
    assert(nbytes == data.size());
    fclose(f);
    return true;
  }
};

#endif


