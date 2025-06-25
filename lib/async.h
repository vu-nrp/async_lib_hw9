#pragma once

#include "types.h"

namespace async {

//!
//! \brief init - initialize library
//!
void init();

//!
//! \brief deinit - deinit library
//!
void deinit();

//!
//! \brief connect - function of connction to ...
//! \param blockSize - count commands in static block
//! \return descriptor of connection
//!
Handle connect(const std::size_t &blockSize);

//!
//! \brief receive - receve block of data and process it
//! \param handle - connection descriptor
//! \param dataBlock
//! \param dataBlockSize
//!
void receive(const Handle &handle, const char *dataBlock, const std::size_t &dataBlockSize);

//!
//! \brief disconnect - close connection
//! \param handle - connection descriptor
//!
void disconnect(const Handle &handle);

}

