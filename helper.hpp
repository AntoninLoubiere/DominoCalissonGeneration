#include <ostream>

class ByteCompressor {
 public:
  ByteCompressor(std::ostream& os, int element_size);

  int element_size() const;
  void set_element_size(int size);
  ByteCompressor& add_element(int elem, int size);

  void end();

 private:
  int element_size_;
  std::ostream& os_;
  char partial_byte_;
  char byte_current_;
};
ByteCompressor& operator<<(ByteCompressor& bc, int elem);
