#include "helper.hpp"

#include <ostream>

const int BYTE_SIZE = 8;
ByteCompressor::ByteCompressor(std::ostream &os, int es)
    : element_size_(es), os_(os), partial_byte_(0), byte_current_(0) {}

ByteCompressor &ByteCompressor::add_element(int elem, int size) {
  if (byte_current_ + size < BYTE_SIZE) {
    partial_byte_ |= elem << byte_current_;
    byte_current_ += size;
    return *this;
  }

  os_ << (char)(partial_byte_ | (elem << byte_current_));
  int data_rem = size - BYTE_SIZE + byte_current_;
  elem >>= byte_current_;
  while (data_rem >= BYTE_SIZE) {
    os_ << (char)elem;
    elem >>= BYTE_SIZE;
    data_rem -= BYTE_SIZE;
  }

  partial_byte_ = elem;
  byte_current_ = data_rem;

  return *this;
}

int ByteCompressor::element_size() const { return element_size_; }

void ByteCompressor::set_element_size(int size) { element_size_ = size; }

ByteCompressor &operator<<(ByteCompressor &bc, int el) {
  return bc.add_element(el, bc.element_size());
}

void ByteCompressor::end() {
  if (byte_current_ > 0) os_ << partial_byte_;
  partial_byte_ = 0;
  byte_current_ = 0;
}
