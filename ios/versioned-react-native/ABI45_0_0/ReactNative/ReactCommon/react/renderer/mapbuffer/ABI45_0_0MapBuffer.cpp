/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "ABI45_0_0MapBuffer.h"

using namespace ABI45_0_0facebook::ABI45_0_0React;

namespace ABI45_0_0facebook {
namespace ABI45_0_0React {

static inline int32_t bucketOffset(int32_t index) {
  return sizeof(MapBuffer::Header) + sizeof(MapBuffer::Bucket) * index;
}

static inline int32_t valueOffset(int32_t bucketIndex) {
  return bucketOffset(bucketIndex) + offsetof(MapBuffer::Bucket, data);
}

// TODO T83483191: Extend MapBuffer C++ implementation to support basic random
// access
MapBuffer::MapBuffer(std::vector<uint8_t> data) : bytes_(std::move(data)) {
  auto header = reinterpret_cast<Header const *>(bytes_.data());
  count_ = header->count;

  if (header->bufferSize != bytes_.size()) {
    LOG(ERROR) << "Error: Data size does not match, expected "
               << header->bufferSize << " found: " << bytes_.size();
    abort();
  }
}

int32_t MapBuffer::getKeyBucket(Key key) const {
  int32_t lo = 0;
  int32_t hi = count_ - 1;
  while (lo <= hi) {
    int32_t mid = (lo + hi) >> 1;

    Key midVal =
        *reinterpret_cast<Key const *>(bytes_.data() + bucketOffset(mid));

    if (midVal < key) {
      lo = mid + 1;
    } else if (midVal > key) {
      hi = mid - 1;
    } else {
      return mid;
    }
  }

  return -1;
}

int32_t MapBuffer::getInt(Key key) const {
  auto bucketIndex = getKeyBucket(key);
  ABI45_0_0React_native_assert(bucketIndex != -1 && "Key not found in MapBuffer");

  return *reinterpret_cast<int32_t const *>(
      bytes_.data() + valueOffset(bucketIndex));
}

bool MapBuffer::getBool(Key key) const {
  return getInt(key) != 0;
}

double MapBuffer::getDouble(Key key) const {
  auto bucketIndex = getKeyBucket(key);
  ABI45_0_0React_native_assert(bucketIndex != -1 && "Key not found in MapBuffer");

  return *reinterpret_cast<double const *>(
      bytes_.data() + valueOffset(bucketIndex));
}

int32_t MapBuffer::getDynamicDataOffset() const {
  // The start of dynamic data can be calculated as the offset of the next
  // key in the map
  return bucketOffset(count_);
}

std::string MapBuffer::getString(Key key) const {
  // TODO T83483191:Add checks to verify that offsets are under the boundaries
  // of the map buffer
  int32_t dynamicDataOffset = getDynamicDataOffset();
  int32_t offset = getInt(key);
  int32_t stringLength = *reinterpret_cast<int32_t const *>(
      bytes_.data() + dynamicDataOffset + offset);
  uint8_t const *stringPtr =
      bytes_.data() + dynamicDataOffset + offset + sizeof(int);

  return std::string(stringPtr, stringPtr + stringLength);
}

MapBuffer MapBuffer::getMapBuffer(Key key) const {
  // TODO T83483191: Add checks to verify that offsets are under the boundaries
  // of the map buffer
  int32_t dynamicDataOffset = getDynamicDataOffset();

  int32_t offset = getInt(key);
  int32_t mapBufferLength = *reinterpret_cast<int32_t const *>(
      bytes_.data() + dynamicDataOffset + offset);

  std::vector<uint8_t> value(mapBufferLength);

  memcpy(
      value.data(),
      bytes_.data() + dynamicDataOffset + offset + sizeof(int32_t),
      mapBufferLength);

  return MapBuffer(std::move(value));
}

size_t MapBuffer::size() const {
  return bytes_.size();
}

uint8_t const *MapBuffer::data() const {
  return bytes_.data();
}

uint16_t MapBuffer::count() const {
  return count_;
}

} // namespace ABI45_0_0React
} // namespace ABI45_0_0facebook
