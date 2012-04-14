#pragma once

#include "bricks/core/string.h"

namespace Bricks { namespace IO {
	class FilePath : public String
	{
	public:
		static const String& GetDirectorySeparators();

#if BRICKS_ENV_WINDOWS
		static const char DirectorySeparator = '\\';
#else
		static const char DirectorySeparator = '/';
#endif
		static const char ExtensionSeparator = '.';

	private:
		void RemoveLeafSeparator()
		{
			size_t size = GetLength();
			if (size && LastIndexOf(GetDirectorySeparators()) == size - 1)
				Truncate(size - 1);
		}

		static size_t NposOrAdd(size_t num, size_t add) { return num + ((num == npos) ? 0 : add); }

	public:
		FilePath() { }
		FilePath(const String& string) : String(string) { RemoveLeafSeparator(); }

		const String& GetFullPath() const { return *this; }
		String GetDirectory() const { return Substring(0, LastIndexOf(GetDirectorySeparators())); }
		String GetFileName() const { return Substring(NposOrAdd(LastIndexOf(GetDirectorySeparators()), 1)); }
		String RootPath(const String& root) const { if (IsPathRooted()) return *this; return FilePath(root).Combine(*this); }
//		String GetRoot() const;

		String GetExtension() const { String name = GetFileName(); return name.Substring(NposOrAdd(name.FirstIndexOf(ExtensionSeparator), 1)); }
		String GetFileNameWithoutExtension() const { String name = GetFileName(); return name.Substring(0, name.FirstIndexOf(ExtensionSeparator)); }
		String Combine(const String& leaf) const {
			if (!leaf.GetLength())
				return *this;
			if (!GetLength() || FilePath(leaf).IsPathRooted())
				return leaf;
			return *this + DirectorySeparator + leaf;
		}

#if BRICKS_ENV_WINDOWS
		bool IsPathRooted() const { return FirstIndexOf(GetDirectorySeparators()) == 2; } // TODO: Fixit.
#else
		bool IsPathRooted() const { return FirstIndexOf(GetDirectorySeparators()) == 0; }
#endif
		bool HasExtension() const { String name = GetFileName(); size_t index = FirstIndexOf(ExtensionSeparator); return index != npos && index < name.GetLength() - 1; }

		String operator /(const String& leaf) { return Combine(leaf); }
	};
} }
