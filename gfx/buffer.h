#pragma once

#include <cstddef>
#include <cstdint>

namespace ndyn::gfx {

/**
 * Buffer represents GPU memory that can be used for storage, vertex, index,
 * uniform, or indirect purposes. The interface provides size and usage queries
 * and allows uploading data to the GPU.
 */
enum class BufferUsage { storage, vertex, index, uniform, indirect };

class Buffer {
 public:
  virtual ~Buffer() = default;

  /**
   * Returns the size of the buffer in bytes.
   */
  virtual std::size_t size() const = 0;

  /**
   * Returns the intended usage of this buffer.
   */
  virtual BufferUsage usage() const = 0;

  /**
   * Uploads raw data into the buffer at the specified offset.
   * @param data Pointer to source data.
   * @param bytes Number of bytes to upload.
   * @param offset Byte offset into the buffer to begin writing.
   */
  virtual void upload(const void* data, std::size_t bytes, std::size_t offset = 0) = 0;
};

}  // namespace ndyn::gfx
