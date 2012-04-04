#pragma once

#include "bricks/core/returnpointer.h"
#include "bricks/core/typeinfo.h"
#include "bricks/collections/array.h"
#include "bricks/collections/dictionary.h"

namespace Bricks { namespace IO {
	class Stream;
	class StreamReader;
	class StreamWriter;
	class Serializer;

	namespace Internal {
		class ObjectSerializer : public Object
		{
		protected:
			Serializer* serializer;
			TypeInfo type;
			int identifier;

		public:
			ObjectSerializer(const TypeInfo& type, int identifier) : type(type), identifier(identifier) { }

			TypeInfo GetType() const { return type; }
			int GetIdentifier() const { return identifier; }

			virtual void Serialize(StreamWriter* writer, Object* object) const = 0;
			virtual ReturnPointer<Object> Deserialize(StreamReader* reader) const = 0;

			void SetSerializer(Serializer* value) { serializer = value; }
		};
	}

	template<typename T, int N>
	class ObjectSerializer : public Internal::ObjectSerializer
	{
	public:
		ObjectSerializer() : Internal::ObjectSerializer(TypeInfo::OfType<T>(), N) { }

		virtual void SerializeData(StreamWriter* writer, T* object) const = 0;
		virtual ReturnPointer<T> DeserializeData(StreamReader* reader) const = 0;

		void Serialize(StreamWriter* writer, Object* object) const { SerializeData(writer, CastTo<T>(object)); }
		ReturnPointer<Object> Deserialize(StreamReader* reader) const { return DeserializeData(reader); }
	};

	typedef Collections::Dictionary<String, AutoPointer<Object> > SerializationDictionary;
	typedef Collections::Array<AutoPointer<Object> > SerializationArray;

	class Serializer : public Object
	{
	protected:
		typedef Collections::Dictionary<TypeInfo, AutoPointer<Internal::ObjectSerializer> > SerializerDictionary;
		SerializerDictionary serializers;

	public:
		Serializer();

		void RegisterSerializer(Internal::ObjectSerializer* serializer);
		void UnregisterSerializer(Internal::ObjectSerializer* serializer);

		void Serialize(StreamWriter* writer, Object* object) const;
		ReturnPointer<Object> Deserialize(StreamReader* reader) const;

		void Serialize(Stream* writer, Object* object) const;
		ReturnPointer<Object> Deserialize(Stream* reader) const;
	};
} }
