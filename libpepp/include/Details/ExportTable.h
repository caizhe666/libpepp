#pragma once
#include <vector>
#include <string>
#include <windows.h>

#include "../libpepp.h"

namespace libpepp {
	namespace Details {
		class ExportTable
		{
		public:
			using Entry = struct _ENTRY {
				uint32_t Ordinal = 0;
				std::string Name;
				uint32_t Rva = 0;
			};

			explicit
				ExportTable() = default;

			explicit
				ExportTable(
					Pe& file
				);

		private:
			std::vector<Entry> m_Table{};
		};
	}
}