#include "../include/SectionHeader.h"

namespace libpepp {
	SectionHeader::SectionHeader(
		size_t offset
	)
	{
		spdlog::trace("Section header constructed with offset: {}.", offset);
		open(offset);
	}

	void
		SectionHeader::open(
			size_t offset
		)
	{
		spdlog::trace("Building section header with base object and offset: {}.", offset);
		open(m_buffer, offset);
	}

	void
		SectionHeader::open(
			const Buffer& buffer,
			size_t offset
		)
	{
		spdlog::trace("Building section header with given buffer and offset: {}.", offset);
		const auto pSectionHeader = reinterpret_cast<IMAGE_SECTION_HEADER const*>(buffer.data() + offset);

		// Copy header to myself
		copyHeaderFrom(pSectionHeader);

		// Copy content to myself
		if (pSectionHeader->PointerToRawData) {
			const auto pContent = reinterpret_cast<void const*>(buffer.data() + PointerToRawData);
			copyContentFrom(pContent, SizeOfRawData);
		}

		m_valid = true;
	}

	size_t
		SectionHeader::copyHeaderTo(
			size_t offset
		)
	{
		return copyHeaderTo(m_buffer, offset);
	}

	size_t
		SectionHeader::copyHeaderTo(
			Buffer& buffer,
			size_t offset
		)
	{
		spdlog::trace("Copied section header to buffer with allocate");
		auto pointer = reinterpret_cast<uint8_t*>(dynamic_cast<PIMAGE_SECTION_HEADER>(this));
		buffer.resize(offset + sizeof(IMAGE_SECTION_HEADER));
		std::copy(pointer, pointer + sizeof(IMAGE_SECTION_HEADER), buffer.begin() + offset);

		return offset + sizeof(IMAGE_SECTION_HEADER);
	}

	size_t
		SectionHeader::copyHeaderToNoAlloc(
			size_t offset
		)
	{
		return copyHeaderToNoAlloc(m_buffer, offset);
	}

	size_t
		SectionHeader::copyHeaderToNoAlloc(
			Buffer& buffer,
			size_t offset
		)
	{
		spdlog::trace("Copied section header to buffer without allocate");
		auto pointer = reinterpret_cast<uint8_t*>(dynamic_cast<PIMAGE_SECTION_HEADER>(this));
		std::copy(pointer, pointer + sizeof(IMAGE_SECTION_HEADER), buffer.begin() + offset);

		return offset + sizeof(IMAGE_SECTION_HEADER);
	}

	size_t
		SectionHeader::copyContentTo(
			size_t offset
		)
	{
		return copyContentTo(m_buffer, offset, m_content);
	}

	size_t
		SectionHeader::copyContentTo(
			Buffer& buffer,
			size_t offset
		)
	{
		return copyContentTo(buffer, offset, m_content);
	}

	size_t
		SectionHeader::copyContentTo(
			Buffer& buffer,
			size_t offset,
			Buffer& content
		)
	{
		spdlog::trace("Copied section content to buffer with allocate");
		buffer.resize(offset + content.size());
		std::copy(content.cbegin(), content.cend(), buffer.begin() + offset);

		return offset + content.size();
	}

	size_t
		SectionHeader::copyContentToNoAlloc(
			size_t offset
		)
	{
		return copyContentToNoAlloc(m_buffer, offset, m_content);
	}

	size_t
		SectionHeader::copyContentToNoAlloc(
			Buffer& buffer,
			size_t offset
		)
	{
		return copyContentToNoAlloc(buffer, offset, m_content);
	}

	size_t
		SectionHeader::copyContentToNoAlloc(
			Buffer& buffer,
			size_t offset,
			Buffer& content
		)
	{
		spdlog::trace("Copied section content to buffer without allocate");
		std::copy(content.cbegin(), content.cend(), buffer.begin() + offset);

		return offset + content.size();
	}

	void
		SectionHeader::copyHeaderFrom(
			IMAGE_SECTION_HEADER const* pointer
		)
	{
		spdlog::trace("Copied to section header.");
		auto pSectionHeader = dynamic_cast<PIMAGE_SECTION_HEADER>(this);
		memcpy(pSectionHeader, pointer, sizeof(IMAGE_SECTION_HEADER));
	}

	void
		SectionHeader::copyContentFrom(
			const void* pointer,
			size_t size
		)
	{
		spdlog::trace("Copied to section content with size: {}.", size);
		m_content.clear();
		m_content.resize(size);
		auto buffer = reinterpret_cast<const uint8_t*>(pointer);
		std::copy(buffer, buffer + size, m_content.begin());
	}
}
