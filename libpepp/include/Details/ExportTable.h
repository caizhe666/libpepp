#pragma once
#include <vector>
#include <string>
#include <functional>
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
				ExportTable() = delete;

			explicit
				ExportTable(
					Pe& pe
				);

			void
				enumAll(
					std::function<bool(Entry&)> func
				);

		private:
			std::vector<Entry> m_Table{};
		};
	}
}
