#include "../include/libpepp.h"
#include <fstream>
#include <tuple>
#include <queue>
#include <spdlog/spdlog.h>

namespace libpepp {
	Pe::Pe(
		const std::filesystem::path& path
	)
	{
		spdlog::trace("Pe constructed.");
		open(path);
	}

	Pe::Pe(
		const Buffer& buffer
	)
	{
		spdlog::trace("Pe constructed.");
		open(buffer);
	}

	bool
		Pe::open(
			const std::filesystem::path& path
		)
	{
		spdlog::trace("Building pe with given path.");
		// Initialize base object
		auto& rBaseObject = asBaseObject();
		auto ret = rBaseObject.open(path);

		open();

		return ret;
	}

	void
		Pe::open(
			const Buffer& buffer
		)
	{
		spdlog::trace("Building pe with given buffer.");
		// Initialize base object
		auto& rBaseObject = asBaseObject();
		rBaseObject.open(buffer);

		open();
	}

	void
		Pe::open(
			void
		)
	{
		// Initialize dos header
		auto& rDosHeader = getDosHeader();
		rDosHeader.open();

		// Initialize nt headers
		auto& rNtHeaders = getNtHeaders();
		rNtHeaders.open(rDosHeader.e_lfanew);

		// Initialize section headers
		WORD i = 0;
		auto& rSectionHeaders = getSectionHeaders();
		rSectionHeaders.resize(rNtHeaders.getFileHeader().NumberOfSections);
		for (auto& sec : rSectionHeaders) {
			sec.open(m_buffer, rDosHeader.e_lfanew // At the beginning of nt headers
				+ sizeof(DWORD) + sizeof(IMAGE_FILE_HEADER) + rNtHeaders.getFileHeader().SizeOfOptionalHeader // At the beginning of section headers
				+ i * sizeof(IMAGE_SECTION_HEADER));

			i++;
		}
	}

	DosHeader&
		Pe::getDosHeader(
			void
		)
	{
		return dynamic_cast<DosHeader&>(*this);
	}

	NtHeaders&
		Pe::getNtHeaders(
			void
		)
	{
		return dynamic_cast<NtHeaders&>(*this);
	}

	SectionHeaders&
		Pe::getSectionHeaders(
			void
		)
	{
		return dynamic_cast<SectionHeaders&>(*this);
	}

	BaseObject&
		Pe::asBaseObject(
			void
		)
	{
		return dynamic_cast<BaseObject&>(*this);
	}

	SectionHeader&
		Pe::getSectionByVa(
			size_t va
		)
	{
		auto rva = va - getNtHeaders().getOptionalHeader().ImageBase;

		return getSectionByRva(rva);
	}

	SectionHeader&
		Pe::getSectionByRva(
			size_t rva
		)
	{
		auto& rSectionHeaders = getSectionHeaders();
		for (const auto& sec : rSectionHeaders) {
			if (rva == std::clamp(rva, static_cast<size_t>(sec.VirtualAddress),
				static_cast<size_t>(sec.VirtualAddress + sec.Misc.VirtualSize))) return const_cast<SectionHeader&>(sec);
		}

		throw std::out_of_range("No section matched given RVA.");
	}

	SectionHeader&
		Pe::getSectionByFo(
			size_t fo
		)
	{
		auto& rSectionHeaders = getSectionHeaders();
		for (const auto& sec : rSectionHeaders) {
			if (fo == std::clamp(fo, static_cast<size_t>(sec.PointerToRawData),
				static_cast<size_t>(sec.PointerToRawData + sec.SizeOfRawData))) return const_cast<SectionHeader&>(sec);
		}

		throw std::out_of_range("No section matched given FO.");
	}

	SectionHeader&
		Pe::getSectionByName(
			const std::string& name
		)
	{
		auto& rSectionHeaders = getSectionHeaders();
		for (const auto& sec : rSectionHeaders) {
			if (name == reinterpret_cast<char const*>(sec.Name)) return const_cast<SectionHeader&>(sec);
		}

		throw std::out_of_range("No section matched given name.");
	}

	//
	//	Pe read: enumerate data dictionary
	//

	void
		Pe::enumResource(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumException(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumSecurity(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumDebug(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumCopyright(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumArchitecture(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumGlobalptr(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumTls(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumLoadConfig(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumBoundImport(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumIat(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumDelayImport(
			void
		)
	{
		// TODO: Not implemented
	}

	void
		Pe::enumComDescriptor(
			void
		)
	{
		// TODO: Not implemented
	}

	//
	//	Pe write
	//

		/*
	void
		Pe::setImport(
			Imports& imports
		)
	{
		// TODO: Not implemented

		// TODO: Support forwarder chain

		// Get information
		auto& importDir = getNtHeaders().getOptionalHeader().DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		auto& rva = importDir.VirtualAddress;
		auto& size = importDir.Size;

		// Total size of the section
		size_t totalSize = 0;
		// Add size of IMAGE_IMPORT_DESCRIPTORs without name, plus 1 indicates a null terminator
		totalSize += (imports.size() + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
		// Dll names position
		auto posDllNames = totalSize;

		// Calculate size of dll names
		for (const auto& imp : imports) {
			// Plus 1 indicates a null terminator
			totalSize += imp.DllName.size() + 1;
		}
		// Iat position
		auto posIat = totalSize;

		// Calculate size of iat without name
		for (const auto& imp : imports) {
			auto& usingTable = imp.Iat.size() ? imp.Iat : imp.Ilt;

			totalSize += (usingTable.size() + imports.size()) * sizeof(IMAGE_THUNK_DATA);
		}
		// Ilt position
		auto posIlt = totalSize;

		// Calculate size of ilt without name
		for (const auto& imp : imports) {
			auto& usingTable = imp.Iat.size() ? imp.Iat : imp.Ilt;

			totalSize += (usingTable.size() + imports.size()) * sizeof(IMAGE_THUNK_DATA);
		}
		// Iat names position
		auto posIatNames = totalSize;

		// Calculate size of iat by name
		for (const auto& imp : imports) {
			auto& usingTable = imp.Iat.size() ? imp.Iat : imp.Ilt;
			for (const auto& entry : usingTable) {
				if (!FlagOn(entry.Ordinal, IMAGE_ORDINAL_FLAG)) {
					// Add size of hint
					totalSize += sizeof(WORD);
					// Plus 2 indicates a null terminator and a padding
					totalSize += entry.ByName.Name.size() + 2;
				}
			}
		}

		size = totalSize;
		totalSize = align_up(totalSize, getNtHeaders().getOptionalHeader().FileAlignment);
		// Generate a new section
		SectionHeader secImport;
		auto& secAdded = getSectionHeaders().emplace_back(secImport);
		// Set name
		secAdded.Name[0] = '.';
		secAdded.Name[1] = 'i';
		secAdded.Characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE | IMAGE_SCN_CNT_INITIALIZED_DATA;
		auto& content = secAdded.m_content;
		secAdded.Misc.VirtualSize = totalSize;
		content.resize(totalSize);
		updateHeaders();
		rva = secAdded.VirtualAddress;

		auto pDescriptorStart = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(content.data());
		auto pDllNamesStart = content.data() + posDllNames;
		auto pIat = reinterpret_cast<PIMAGE_THUNK_DATA>(content.data() + posIat);
		auto pIlt = reinterpret_cast<PIMAGE_THUNK_DATA>(content.data() + posIlt);
		auto pIatNamesStart = content.data() + posIatNames;

		for (const auto& imp : imports) {
			auto& usingTable = imp.Iat.size() ? imp.Iat : imp.Ilt;

			// Copy iat and ilt
			pDescriptorStart->FirstThunk = reinterpret_cast<uint8_t*>(pIat) - content.data() + rva;
			pDescriptorStart->OriginalFirstThunk = reinterpret_cast<uint8_t*>(pIlt) - content.data() + rva;
			for (const auto& entry : usingTable) {
				// Copy iat content
				pIat->u1.Ordinal = pIatNamesStart - content.data() + rva;
				pIat++;
				// Copy ilt content
				pIlt->u1.Ordinal = pIatNamesStart - content.data() + rva;
				pIlt++;

				// Copy iat import by name
				if (!FlagOn(entry.Ordinal, IMAGE_ORDINAL_FLAG)) {
					*reinterpret_cast<WORD*>(pIatNamesStart) = entry.ByName.Hint;
					pIatNamesStart += sizeof(WORD);
					std::copy(entry.ByName.Name.cbegin(), entry.ByName.Name.cend(), pIatNamesStart);
					pIatNamesStart += entry.ByName.Name.size() + 1;
					if ((pIatNamesStart - content.data() + secAdded.PointerToRawData) % 2) pIatNamesStart++;
				}
				else {
					SetFlag(pIat->u1.Ordinal, IMAGE_ORDINAL_FLAG);
					SetFlag(pIlt->u1.Ordinal, IMAGE_ORDINAL_FLAG);
				}
			}
			pIat++;
			pIlt++;

			// Copy dll name
			pDescriptorStart->Name = pDllNamesStart - content.data() + rva;
			std::copy(imp.DllName.cbegin(), imp.DllName.cend(), pDllNamesStart);
			pDllNamesStart += imp.DllName.size() + 1;

			pDescriptorStart->ForwarderChain = imp.ForwarderChain;
			pDescriptorStart->TimeDateStamp = imp.TimeDateStamp;

			pDescriptorStart++;
		}
}
		*/

	size_t
		Pe::updateHeaders(
			void
		)
	{
		// Total raw size
		size_t totalSize = 0;
		// Get all headers
		auto& rDosHeader = getDosHeader();
		auto& rNtHeaders = getNtHeaders();
		auto& rSectionHeaders = getSectionHeaders();
		auto& rOptionalHeader = rNtHeaders.getOptionalHeader();
		auto& rFileHeader = rNtHeaders.getFileHeader();

		// Set e_lfanew to a correct value
		rDosHeader.e_lfanew = std::max<size_t>(rDosHeader.e_lfanew, rDosHeader.totalSize());
		// Add the size of dos header to totalSize
		totalSize += rDosHeader.totalSize();
		// The start position of nt headers
		auto posNtHeaders = totalSize;

		// Add the size of nt headers to totalSize
		totalSize += rNtHeaders.totalSize();
		// The start position of section headers
		auto posSectionHeaders = totalSize;

		// Add the size of section headers to totalSize
		totalSize += rSectionHeaders.size() * sizeof(IMAGE_SECTION_HEADER);
		// The total size of headers
		auto sizeOfHeaders = align_up(totalSize, rOptionalHeader.FileAlignment);
		// The start position of section contents in file offset
		auto posContentFo = sizeOfHeaders;
		// The start position of section contents in rva
		auto posContentRva = align_up(totalSize, rOptionalHeader.SectionAlignment);
		uint32_t sizeOfCode = 0;
		uint32_t sizeOfInitedData = 0;
		uint32_t sizeOfUninitedData = 0;
		for (auto& rSection : rSectionHeaders) {
			// Correct the parameters that users don't give correctly
			rSection.SizeOfRawData = rSection.m_content.size();
			rSection.PointerToRawData = align_up(rSection.PointerToRawData, rOptionalHeader.FileAlignment);
			// Check if the content offset is smaller than current offset,
			// if true, then set it to current offset
			rSection.PointerToRawData = std::max<size_t>(rSection.PointerToRawData, posContentFo);
			rSection.VirtualAddress = align_up(rSection.VirtualAddress, rOptionalHeader.SectionAlignment);
			// Check if the rva conflict with other section,
			// if true, then set it to the nearest end and align it
			rSection.VirtualAddress = std::max<size_t>(rSection.VirtualAddress, posContentRva);

			// Add the content size to totalSize
			totalSize = static_cast<size_t>(rSection.PointerToRawData) + rSection.SizeOfRawData;

			// Move the posContentFo to next content
			posContentFo = align_up(totalSize, rOptionalHeader.FileAlignment);
			// Move the posContentRva to next content
			posContentRva = align_up(static_cast<size_t>(rSection.VirtualAddress) + rSection.Misc.VirtualSize, rOptionalHeader.SectionAlignment);

			// Check if the section has code attribute and add to size of code
			if (rSection.Characteristics & IMAGE_SCN_CNT_CODE ||
				rSection.Characteristics & IMAGE_SCN_MEM_EXECUTE)
				sizeOfCode += rSection.Misc.VirtualSize;
			// Check if the section has initialized data attribute and add to size of initialized data
			if (rSection.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) sizeOfInitedData += rSection.Misc.VirtualSize;
			// Check if the section has code attribute and add to size of code
			if (rSection.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) sizeOfUninitedData += rSection.Misc.VirtualSize;
		}

		// Update sizes
		sizeOfCode = align_up(sizeOfCode, rOptionalHeader.SectionAlignment);
		sizeOfInitedData = align_up(sizeOfInitedData, rOptionalHeader.SectionAlignment);
		sizeOfUninitedData = align_up(sizeOfUninitedData, rOptionalHeader.SectionAlignment);
		rOptionalHeader.SizeOfImage = posContentRva;
		rOptionalHeader.SizeOfCode = sizeOfCode;
		rOptionalHeader.SizeOfInitializedData = sizeOfInitedData;
		rOptionalHeader.SizeOfUninitializedData = sizeOfUninitedData;
		rOptionalHeader.SizeOfHeaders = sizeOfHeaders;

		// Update section numbers
		rFileHeader.NumberOfSections = rSectionHeaders.size();

		// TODO: Update checksum

		return totalSize;
	}

	bool
		Pe::rebuild(
			void
		)
	{
		size_t offset = 0;
		auto totalSize = updateHeaders();
		m_buffer.clear();
		m_buffer.resize(totalSize);
		// Get all headers
		auto& rDosHeader = getDosHeader();
		auto& rNtHeaders = getNtHeaders();
		auto& rSectionHeaders = getSectionHeaders();
		auto& rOptionalHeader = rNtHeaders.getOptionalHeader();
		auto& rFileHeader = rNtHeaders.getFileHeader();

		// Copy dos header
		rDosHeader.copyToNoAlloc();
		// Copy nt headers
		offset = rNtHeaders.copyToNoAlloc(rDosHeader.e_lfanew);
		// Copy section headers
		for (auto& rSection : rSectionHeaders) {
			auto oldOffset = offset;
			// Copy section header
			offset = rSection.copyHeaderToNoAlloc(m_buffer, offset);
			// Copy section content
			rSection.copyContentToNoAlloc(m_buffer, rSection.PointerToRawData);
			rSection.open(m_buffer, oldOffset);
		}

		rDosHeader.open();
		rNtHeaders.open(rDosHeader.e_lfanew);

		return true;
	}

	bool
		Pe::write(
			const std::filesystem::path& path
		)
	{
		if (!m_valid) return false;

		std::ofstream file(path, std::ios_base::binary);

		if (!file.is_open()) return false;

		std::copy(m_buffer.cbegin(), m_buffer.cend(), std::ostreambuf_iterator<char>(file));
		return true;
	}

	void
		Pe::write(
			Buffer& buffer
		)
	{
		buffer.clear();
		std::copy(m_buffer.cbegin(), m_buffer.cend(), buffer.begin());
	}

	//
	//	Validation
	//

	bool
		Pe::validVa(
			size_t va
		)
	{
		if (va > getNtHeaders().getOptionalHeader().SizeOfImage) return false;

		return true;
	}

	bool
		Pe::validRva(
			size_t rva
		)
	{
		if (rva + getNtHeaders().getOptionalHeader().ImageBase > getNtHeaders().getOptionalHeader().SizeOfImage) return false;

		return true;
	}

	bool
		Pe::validFo(
			size_t fo
		)
	{
		if (fo > m_buffer.size()) return false;

		return true;
	}

	bool
		Pe::isValid(
			void
		) const
	{
		return m_valid;
	}

	//
	//	Other stuffs
	//

	uint32_t
		Pe::computeChecksum(
			void
		)
	{
		static const uint16_t posChecksum = 64;
		static const uint32_t top = 0xFFFFFFFF;
		size_t checksum = 0;
		auto base = m_buffer.data();
		for (size_t i = 0; i < m_buffer.size(); i += sizeof(uint32_t))
		{
			uint32_t dw = *reinterpret_cast<uint32_t*>(base + i);

			//Skip "CheckSum" position
			if (i == posChecksum) continue;

			// Calculate checksum
			checksum = (checksum & 0xffffffff) + dw + (checksum >> 32);
			if (checksum > top)
				checksum = (checksum & 0xffffffff) + (checksum >> 32);
		}

		//Finish checksum
		checksum = (checksum & 0xffff) + (checksum >> 16);
		checksum = (checksum)+(checksum >> 16);
		checksum = checksum & 0xffff;

		checksum += m_buffer.size();

		return static_cast<uint32_t>(checksum);
	}
}
