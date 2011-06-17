#pragma once

#include "bricks/string.h"

namespace Bricks { namespace IO {
	class FilePath : public String
	{
	public:
		static const String DirectorySeparators;
#ifdef BRICKS_FEATURE_WINDOWS
		static const char DirectorySeparator = '\\';
#else
		static const char DirectorySeparator = '/';
#endif
		static const char ExtensionSeparator = '.';

		BRICKS_COPY_CONSTRUCTOR(FilePath);
	private:
		void RemoveLeafSeparator()
		{
			size_t size = GetLength();
			if (LastIndexOf(DirectorySeparators) == size - 1)
				self[size - 1] = '\0';
		}

		static size_t NposOrAdd(size_t num, size_t add) { return num + ((num == npos) ? 0 : add); }

	public:
		FilePath(const String& string) : String(string) { RemoveLeafSeparator(); }

		const String& GetFullPath() const { return self; }
		String& GetDirectory() const { return Substring(0, LastIndexOf(DirectorySeparators)); }
		String& GetFileName() const { return Substring(NposOrAdd(LastIndexOf(DirectorySeparators), 1)); }
		String& RootPath(const String& root) { if (IsPathRooted()) return self; return FilePath(root).Combine(self); }
//		String GetRoot() const;

		String& GetExtension() const { String name = GetFileName(); return name.Substring(NposOrAdd(name.FirstIndexOf(ExtensionSeparator), 1)); }
		String& GetFileNameWithoutExtension() const { String name = GetFileName(); return name.Substring(0, name.FirstIndexOf(ExtensionSeparator)); }
		String& Combine(const String& leaf) const { return self + DirectorySeparator + leaf; }

#ifdef BRICKS_FEATURE_WINDOWS
		bool IsPathRooted() const { return FirstIndexOf(DirectorySeparators) == 2; } // TODO: Fixit.
#else
		bool IsPathRooted() const { return FirstIndexOf(DirectorySeparators) == 0; }
#endif
		bool HasExtension() const { String name = GetFileName(); size_t index = FirstIndexOf(ExtensionSeparator); return index != npos && index < name.GetLength() - 1; }

		String operator /(const String& leaf) { return Combine(leaf); }
	};
} }
