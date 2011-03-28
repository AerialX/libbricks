#pragma once

#include <dirent.h>
#include <sys/stat.h>

#include "bricks/collections.h"
#include "bricks/io/types.h"
#include "bricks/io/stream.h"

namespace Bricks { namespace IO {
	namespace NodeType { enum Enum {
		Unknown = DT_UNKNOWN,
		File = DT_REG,
		Directory = DT_DIR,
		BlockDevice = DT_BLK,
		CharacterDevice = DT_CHR,
		FIFO = DT_FIFO,
		SymbolicLink = DT_LNK,
		Socket = DT_SOCK
	}; }

	class FileNode : public Object, public Bricks::Collections::Iterable<FileNode>
	{
	private:
		NodeType::Enum type;
		CopyPointer<String> name;

	public:
		FileNode() : type(NodeType::Unknown) { }
		FileNode(NodeType::Enum type, const String& name) : type(type), name(name) { }
		virtual ~FileNode() { }

		virtual NodeType::Enum GetType() const { return type; }
		virtual const String& GetName() const { return *name; }
		virtual const String& GetFullName() const = 0;
		virtual Pointer<FileNode> GetParent() const = 0;
		virtual u64 GetSize() const = 0;
		virtual Stream& OpenStream(
			FileOpenMode::Enum createmode = FileOpenMode::Create,
			FileMode::Enum mode = FileMode::ReadWrite,
			FilePermissions::Enum permissions = FilePermissions::OwnerReadWrite
		) = 0;
	};
} }
