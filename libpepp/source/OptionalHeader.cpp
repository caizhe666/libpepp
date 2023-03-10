#include "../include/OptionalHeader.h"

namespace libpepp {
	OptionalHeader::OptionalHeader(
		size_t offset,
		size_t size
	)
	{
		spdlog::trace("Optional header constructed with offset: {} and size: {}.", offset, size);
		open(offset, size);
	}

	void
		OptionalHeader::open(
			size_t offset,
			size_t size
		)
	{
		spdlog::trace("Building optional header with base object and offset: {} and size: {}.", offset, size);
		open(m_buffer, offset, size);
	}

	void
		OptionalHeader::open(
			const Buffer& buffer,
			size_t offset,
			size_t size
		)
	{
		if (buffer.empty()) {
			m_valid = false;
			return;
		}

		spdlog::trace("Building optional header with given buffer and offset: {} and size: {}.", offset, size);
		const auto pOptionalHeader = reinterpret_cast<IMAGE_OPTIONAL_HEADER const*>(buffer.data() + offset);

		// Copy to myself
		copyFrom(pOptionalHeader, size);
		m_valid = true;
	}

	void
		OptionalHeader::copyTo(
			size_t offset,
			size_t size
		)
	{
		copyTo(m_buffer, offset, size);
	}

	void
		OptionalHeader::copyTo(
			Buffer& buffer,
			size_t offset,
			size_t size
		)
	{
		spdlog::trace("Copied optional header to buffer with allocate");
		auto pointer = reinterpret_cast<uint8_t*>(dynamic_cast<PIMAGE_OPTIONAL_HEADER>(this));
		buffer.resize(offset + size);
		std::copy(pointer, pointer + size, buffer.begin() + offset);
	}

	void
		OptionalHeader::copyToNoAlloc(
			size_t offset,
			size_t size
		)
	{
		copyToNoAlloc(m_buffer, offset, size);
	}

	void
		OptionalHeader::copyToNoAlloc(
			Buffer& buffer,
			size_t offset,
			size_t size
		)
	{
		spdlog::trace("Copied optional header to buffer without allocate");
		auto pointer = reinterpret_cast<uint8_t*>(dynamic_cast<PIMAGE_OPTIONAL_HEADER>(this));
		std::copy(pointer, pointer + size, buffer.begin() + offset);
	}

	void
		OptionalHeader::copyFrom(
			IMAGE_OPTIONAL_HEADER const* pointer,
			size_t size
		)
	{
		spdlog::trace("Copied to optional header with size: {}.", size);
		auto pOptionalHeader = dynamic_cast<PIMAGE_OPTIONAL_HEADER>(this);
		memset(pOptionalHeader, 0, sizeof(IMAGE_OPTIONAL_HEADER));
		memcpy(pOptionalHeader, pointer, size);
	}
}
