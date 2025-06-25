#pragma once

#include "types.h"

namespace async {

void init();
void deinit();

Handle connect(const std::size_t &blockSize);
void receive(const Handle &handle, const char *dataBlock, const std::size_t &dataBlockSize);
void disconnect(const Handle &handle);

}

