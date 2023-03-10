#include "../include/NtHeaders.h"

namespace libpepp {
	NtHeaders::NtHeaders(
		uint32_t offset
	)
	{
		spdlog::trace("Nt headers constructed with offset: {}.", offset);
		open(offset);
	}

	void
		NtHeaders::open(
			uint32_t offset
		)
	{
		spdlog::trace("Building nt headers with base object and offset: {}.", offset);
		open(m_buffer, offset);
	}

	void
		NtHeaders::open(
			const Buffer& buffer,
			uint32_t offset
		)
	{
		if (buffer.empty()) {
			m_valid = false;
			return;
		}

		spdlog::trace("Building nt headers with given buffer and offset: {}.", offset);
		const auto pNtHeaders = reinterpret_cast<IMAGE_NT_HEADERS const*>(buffer.data() + offset);

		// Check nt headers
		if (pNtHeaders->Signature != IMAGE_NT_SIGNATURE) {
			spdlog::error("Not a valid pe file.");
			m_valid = false;
			return;
		}

		// Initialize signature
		Signature = pNtHeaders->Signature;

		// Initialize file header
		auto pFileHeader = dynamic_cast<FileHeader*>(this);
		pFileHeader->FileHeader::open(offset + sizeof(pNtHeaders->Signature));

		// Initialize optional header
		auto pOptionalHeader = dynamic_cast<OptionalHeader*>(this);
		pOptionalHeader->OptionalHeader::open(offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER), pFileHeader->SizeOfOptionalHeader);

		m_valid = true;
	}

	FileHeader&
		NtHeaders::getFileHeader(
			void
		)
	{
		return dynamic_cast<FileHeader&>(*this);
	}

	OptionalHeader&
		NtHeaders::getOptionalHeader(
			void
		)
	{
		return dynamic_cast<OptionalHeader&>(*this);
	}

	size_t
		NtHeaders::copyTo(
			uint32_t offset
		)
	{
		return copyTo(m_buffer, offset);
	}

	size_t
		NtHeaders::copyTo(
			Buffer& buffer,
			uint32_t offset
		)
	{
		spdlog::trace("Copied nt headers to buffer with allocate");
		auto& rFileHeader = getFileHeader();
		auto& rOptionalHeader = getOptionalHeader();

		buffer.resize(offset + sizeof(DWORD));
		std::copy(reinterpret_cast<uint8_t*>(&Signature), reinterpret_cast<uint8_t*>(&Signature + 1), buffer.begin() + offset);

		rFileHeader.copyTo(buffer, offset + sizeof(DWORD));
		rOptionalHeader.copyTo(buffer, offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER), rFileHeader.SizeOfOptionalHeader);

		return offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + rFileHeader.SizeOfOptionalHeader;
	}

	size_t
		NtHeaders::copyToNoAlloc(
			uint32_t offset
		)
	{
		return copyToNoAlloc(m_buffer, offset);
	}

	size_t
		NtHeaders::copyToNoAlloc(
			Buffer& buffer,
			uint32_t offset
		)
	{
		spdlog::trace("Copied nt headers to buffer without allocate");
		auto& rFileHeader = getFileHeader();
		auto& rOptionalHeader = getOptionalHeader();

		std::copy(reinterpret_cast<uint8_t*>(&Signature), reinterpret_cast<uint8_t*>(&Signature + 1), buffer.begin() + offset);

		rFileHeader.copyToNoAlloc(buffer, offset + sizeof(DWORD));
		rOptionalHeader.copyToNoAlloc(buffer, offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER), rFileHeader.SizeOfOptionalHeader);

		return offset + sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + rFileHeader.SizeOfOptionalHeader;
	}

	size_t
		NtHeaders::totalSize(
			void
		)
	{
		auto& rFileHeader = getFileHeader();

		return sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + rFileHeader.SizeOfOptionalHeader;
	}
}
