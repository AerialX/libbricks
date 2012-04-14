#include "bricks/io/serializer.h"
#include "bricks/io/streamreader.h"
#include "bricks/io/streamwriter.h"
#include "bricks/io/stream.h"
#include "bricks/core/data.h"
#include "bricks/core/value.h"

using namespace Bricks::Collections;

namespace Bricks { namespace IO { namespace Internal {
	class DictionarySerializer : public IO::ObjectSerializer<SerializationDictionary, 0xa563cab6>
	{
	public:
		void SerializeData(StreamWriter* writer, SerializationDictionary* dictionary) const
		{
			writer->WriteInt32(dictionary->GetCount());
			foreach (SerializationDictionary::IteratorType& item, dictionary) {
				serializer->Serialize(writer, tempnew item.GetKey());
				serializer->Serialize(writer, item.GetValue());
			}
		}

		ReturnPointer<SerializationDictionary> DeserializeData(StreamReader* reader) const
		{
			AutoPointer<SerializationDictionary> dictionary = autonew SerializationDictionary();
			int count = reader->ReadInt32();
			for (int i = 0; i < count; i++) {
				AutoPointer<String> key = CastTo<String>(serializer->Deserialize(reader));
				dictionary->Add(*key, serializer->Deserialize(reader));
			}
			return dictionary;
		}
	};

	class ArraySerializer : public IO::ObjectSerializer<SerializationArray, 0x3e346fc4>
	{
	public:
		void SerializeData(StreamWriter* writer, SerializationArray* array) const
		{
			writer->WriteInt32(array->GetCount());
			foreach (SerializationArray::IteratorType& item, array)
				serializer->Serialize(writer, item);
		}

		ReturnPointer<SerializationArray> DeserializeData(StreamReader* reader) const
		{
			AutoPointer<SerializationArray> array = autonew SerializationArray();
			int count = reader->ReadInt32();
			for (int i = 0; i < count; i++)
				array->AddItem(serializer->Deserialize(reader));
			return array;
		}
	};

	class StringSerializer : public IO::ObjectSerializer<String, 0x197ad112>
	{
	public:
		void SerializeData(StreamWriter* writer, String* string) const
		{
			int size = string->GetSize();
			writer->WriteInt32(size);
			writer->WriteBytes(string->CString(), size);
		}

		ReturnPointer<String> DeserializeData(StreamReader* reader) const
		{
			int size = reader->ReadInt32();
			return autonew String((const char*)reader->ReadBytes(size).GetData(), size);
		}
	};

	class ValueSerializer : public IO::ObjectSerializer<Value, 0x5ee289ef>
	{
	public:
		void SerializeData(StreamWriter* writer, Value* value) const
		{
			writer->WriteInt32((int)value->GetType());
			writer->WriteBytes(value->GetData(), value->GetSize());
		}

		ReturnPointer<Value> DeserializeData(StreamReader* reader) const
		{
			ValueType::Enum type = (ValueType::Enum)reader->ReadInt32();
			return autonew Value(reader->ReadBytes(Value::SizeOfType(type)), type);
		}
	};

	class DataSerializer : public IO::ObjectSerializer<Data, 0xf52ee9ce>
	{
	public:
		void SerializeData(StreamWriter* writer, Data* data) const
		{
			writer->WriteInt32(data->GetSize());
			writer->WriteBytes(data->GetData(), data->GetSize());
		}

		ReturnPointer<Data> DeserializeData(StreamReader* reader) const
		{
			int size = reader->ReadInt32();
			return autonew Data(reader->ReadBytes(size));
		}
	};

	struct NullObject { };
	class NullSerializer : public Bricks::IO::Internal::ObjectSerializer
	{
	public:
		NullSerializer() :
			ObjectSerializer(TypeInfo::OfType<NullObject>(), 0)
		{

		}

		void Serialize(StreamWriter* writer, Object* object) const
		{

		}

		Bricks::ReturnPointer<Bricks::Object> Deserialize(StreamReader* reader) const
		{
			return NULL;
		}
	};
} } }

namespace Bricks { namespace IO {
	Serializer::Serializer()
	{
		RegisterSerializer(autonew Internal::DictionarySerializer());
		RegisterSerializer(autonew Internal::ArraySerializer());
		RegisterSerializer(autonew Internal::StringSerializer());
		RegisterSerializer(autonew Internal::ValueSerializer());
		RegisterSerializer(autonew Internal::DataSerializer());
		RegisterSerializer(autonew Internal::NullSerializer());
	}

	void Serializer::RegisterSerializer(Internal::ObjectSerializer* serializer)
	{
		serializers.Add(serializer->GetType(), serializer);
		serializer->SetSerializer(this);
	}

	void Serializer::UnregisterSerializer(Internal::ObjectSerializer* serializer)
	{
		serializers.RemoveValue(serializer);
	}

	void Serializer::Serialize(StreamWriter* writer, Object* object) const
	{
		TypeInfo type = object ? TypeOf(object) : TypeInfo::OfType<Internal::NullObject>();
		Internal::ObjectSerializer* serializer = serializers.GetItem(type);
		writer->WriteInt32(serializer->GetIdentifier());
		serializer->Serialize(writer, object);
	}

	Bricks::ReturnPointer<Object> Serializer::Deserialize(StreamReader* reader) const
	{
		int value = reader->ReadInt32();
		foreach (const SerializerDictionary::IteratorType& item, serializers) {
			if (item.GetValue()->GetIdentifier() == value)
				return item.GetValue()->Deserialize(reader);
		}
		BRICKS_FEATURE_THROW(InvalidArgumentException());
	}

	void Serializer::Serialize(Stream* stream, Object* object) const
	{
		Serialize(tempnew StreamWriter(stream, Endian::BigEndian), object);
	}

	ReturnPointer<Object> Serializer::Deserialize(Stream* stream) const
	{
		return Deserialize(tempnew StreamReader(stream, Endian::BigEndian));
	}
} }
