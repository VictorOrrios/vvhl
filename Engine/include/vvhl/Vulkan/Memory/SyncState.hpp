#pragma once

#include <vvhl/vvhl.hpp>

namespace vvhl {

struct SyncState {
  VkAccessFlags2 access = 0;
  bool isReadAccess() const {
    return access & (VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT |
                     VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
                     VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_TRANSFER_READ_BIT);
  }

  bool isWriteAccess() const {
    return access & (VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT);
  }

  bool isReadWriteAccess() const { return isReadAccess() && isWriteAccess(); }
};

} // namespace vvhl