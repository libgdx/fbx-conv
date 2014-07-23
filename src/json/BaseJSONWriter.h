/*******************************************************************************
 * Copyright 2011 See AUTHORS file.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
/** @author Xoppa */
#ifdef _MSC_VER 
#pragma once
#endif
#ifndef JSON_BASEJSONWRITER_H
#define JSON_BASEJSONWRITER_H

#include <ostream>
#include <fstream>
#include <stack>
#include <vector>
#include <string>
#include <cassert>
#include <stdio.h>

namespace json {

template <typename T> struct remove_const { typedef T type; };
template <typename T> struct remove_const<const T> { typedef T type; };
template <typename T> struct remove_volatile { typedef T type; };
template <typename T> struct remove_volatile<volatile T> { typedef T type; };
template <typename T> struct remove_cv : remove_const<typename remove_volatile<T>::type> {};
template <typename T> struct is_unqualified_pointer { enum { value = false }; };
template <typename T> struct is_unqualified_pointer<T*> { enum { value = true }; };
template <typename T> struct is_pointer : is_unqualified_pointer<typename remove_cv<T>::type> {};
template<bool _Test, class _Type = void> struct enable_if { };
template<class _Type> struct enable_if<true, _Type> { typedef _Type type; };

typedef char (&yes)[1];
typedef char (&no)[2];

template <typename B, typename D> struct Host
{
  operator B*() const;
  operator D*();
};

template <typename B, typename D> struct is_base_of
{
  template <typename T> static yes check(D*, T);
  static no check(B*, int);
  static const bool value = sizeof(check(Host<B,D>(), int())) == sizeof(yes);
};


class BaseJSONWriter;

struct ConstSerializable {
	virtual void serialize(BaseJSONWriter &writer) const = 0;
};

// The possible operations:
// - is: delimiter between key and value
static const char op_is  = '=';
// - obj: open an object with optional capacity
static const char op_obj = '{';
// - arr: open an array with optional capacity
static const char op_arr = '[';
// - end: close the current object or array
static const char op_end = ';';
// - dat: write an array of data with a specified size, must be followed by a pointer
static const char op_dat = 'H';
// - nul: write null value
static const char op_nul = '0';

static const unsigned int useDefaultSize = ((1 << 16) - 1);

template<const char T> struct JSONOp { 
	JSONOp() {}
};
template<const char T> struct JSONBlockOp : public JSONOp<T> {
	const long &capacity;
	JSONBlockOp(const long &capacity = -1) : capacity(capacity) {}
	JSONBlockOp<T> operator()(const long &capacity) const {
		return JSONBlockOp<T>(capacity);
	}
	JSONBlockOp<T> operator[](const long &capacity) const {
		return JSONBlockOp<T>(capacity);
	}
};
template<const char T, class P> struct JSONPtrOp : public JSONOp<T> {
	const P * const &ptr;
	const size_t &size;
	JSONPtrOp(const size_t &size = 0, const P * const &p = 0) : size(size), ptr(p) {}
	template<class S> inline JSONPtrOp<T, S> operator()(const long &size, const S * const &p) const {
		return JSONPtrOp<T, S>(size, p);
	}
	template<class S, size_t n> inline JSONPtrOp<T, S> operator()(const S (&value)[n]) const {
		return JSONPtrOp<T, S>(n, &value[0]);
	}
};

static const JSONPtrOp<op_dat, void> dat;
static const JSONBlockOp<op_obj>	 obj;
static const JSONBlockOp<op_arr>	 arr;
static const JSONOp<op_end>			 end;
static const JSONOp<op_is>			 is;
static const JSONOp<op_nul>			 nul;

class BaseJSONWriter {
protected:
	struct Block {
		enum Type { ROOT = 0, OBJECT = 1, ARRAY = 2 };

		Type type;
		long long capacity;
		long long size;
		unsigned int maxLineSize;
		unsigned int lineSize;
		bool wroteKey;
		Block(const Block &rhs) 
			: type(rhs.type), capacity(rhs.capacity), size(rhs.size), wroteKey(rhs.wroteKey), maxLineSize(rhs.maxLineSize), lineSize(rhs.lineSize) {}
		Block(const Type &type, const long long &capacity = -1, const unsigned int maxLineSize = 0) 
			: type(type), capacity(capacity), size(0), wroteKey(false), maxLineSize(maxLineSize), lineSize(0) {}
	};

private:
	std::stack<Block> blocks;
	Block block;

	void nextValue(const bool &ispair, const bool &isblock) {
		assert(("Can only write key-value pairs within an object", !((ispair && (block.type != Block::OBJECT)) || (!ispair && (block.type == Block::OBJECT)))));
		assert(("Must start with an array or an object", isblock || block.type != Block::ROOT));
		assert(("Can only write one object or array per instance", block.size == 0 || block.type != Block::ROOT));
		//assert(("Block capacity exceeded", block.capacity < 0 || block.size < block.capacity));

		writeNextValue(block.size++ == 0, ++block.lineSize > block.maxLineSize);
		if (block.lineSize > block.maxLineSize)
			block.lineSize = 1;
	}

	BaseJSONWriter &openBlock(const Block::Type &type, const long long &capacity = -1, const unsigned int &lineSize = 0) {
		assert(("Unknown block type", type == Block::OBJECT || type == Block::ARRAY));

		if (!checkKey())
			nextValue(inObject(), true);

		blocks.push(block);
		block = Block(type, capacity, lineSize);

		if (type == Block::OBJECT)
			writeOpenObject(block.capacity < 0, block.capacity, block.capacity >= 0 && block.capacity <= block.maxLineSize);
		else
			writeOpenArray(block.capacity < 0, block.capacity, block.capacity >= 0 && block.capacity <= block.maxLineSize);
			
		return *this;
	}

	BaseJSONWriter &closeBlock() {
		assert(("Unmatched blocks", block.type != Block::ROOT && !blocks.empty()));
		//assert(("Reserved capacity not reached yet", block.capacity < 0 || block.size == block.capacity));
		assert(("Key written without value", !block.wroteKey));
		assert(("Unknown block type", block.type == Block::OBJECT || block.type == Block::ARRAY));

		if (block.type == Block::OBJECT)
			writeCloseObject(block.capacity < 0, block.size, block.capacity >= 0 && block.capacity <= block.maxLineSize);
		else
			writeCloseArray(block.capacity < 0, block.size, block.capacity >= 0 && block.capacity <= block.maxLineSize);

		block = blocks.top();
		blocks.pop();
		return *this;
	}

protected:
	virtual void writeOpenObject(const bool &varsize, const long long &size, const bool &inl) = 0;
	virtual void writeCloseObject(const bool &varsize, const long long &size, const bool &inl) = 0;
	virtual void writeOpenArray(const bool &varsize, const long long &size, const bool &inl) = 0;
	virtual void writeCloseArray(const bool &varsize, const long long &size, const bool &inl) = 0;
	virtual void writeNextValue(const bool &first, const bool &newline) = 0;
	virtual void writeNull() = 0;
	virtual void writeValue(const char * const &value, const bool &iskey = false) = 0;
	virtual void writeValue(const bool &value, const bool &iskey = false) = 0;
	virtual void writeValue(const char &value, const bool &iskey = false) = 0;
	virtual void writeValue(const short &value, const bool &iskey = false) = 0;
	virtual void writeValue(const int &value, const bool &iskey = false) = 0;
	virtual void writeValue(const long &value, const bool &iskey = false) = 0;
	virtual void writeValue(const float &value, const bool &iskey = false) = 0;
	virtual void writeValue(const double &value, const bool &iskey = false) = 0;
	virtual void writeValue(const unsigned char &value, const bool &iskey = false) = 0;
	virtual void writeValue(const unsigned short &value, const bool &iskey = false) = 0;
	virtual void writeValue(const unsigned int &value, const bool &iskey = false) = 0;
	virtual void writeValue(const unsigned long &value, const bool &iskey = false) = 0;
	// If writeOpenXXXData returns false, it falls back on writing out an array
	inline virtual bool writeOpenFloatData(const size_t& count) { return false; }
	inline virtual bool writeOpenDoubleData(const size_t& count) { return false; }
	inline virtual bool writeOpenShortData(const size_t& count) { return false; }
	inline virtual bool writeOpenUShortData(const size_t& count) { return false; }
	inline virtual bool writeOpenIntData(const size_t& count) { return false; }
	inline virtual bool writeOpenUIntData(const size_t& count) { return false; }
	inline virtual bool writeOpenLongData(const size_t& count) { return false; }
	inline virtual bool writeOpenULongData(const size_t& count) { return false; }
	inline virtual void writeFloatData(const float * const &values, const size_t &count) {}
	inline virtual void writeDoubleData(const double * const &values, const size_t &count) {}
	inline virtual void writeShortData(const short * const &values, const size_t &count) {}
	inline virtual void writeUShortData(const unsigned short * const &values, const size_t &count) {}
	inline virtual void writeIntData(const int * const &values, const size_t &count) {}
	inline virtual void writeUIntData(const unsigned int * const &values, const size_t &count) {}
	inline virtual void writeLongData(const long * const &values, const size_t &count) {}
	inline virtual void writeULongData(const unsigned long * const &values, const size_t &count) {}
	inline virtual void writeCloseData() {}

private:
	bool checkKey(const bool &allowKey = true) {
		if (!inObject())
			return false;
		block.wroteKey = !block.wroteKey;

		assert(("Key not allowed", allowKey || !block.wroteKey));

		return !block.wroteKey;
	}

	template<class T> inline bool openData(const T &dummy, const size_t &items) { return false; }
	inline bool openData(const float &dummy, const size_t &items) { return writeOpenFloatData(items); }
	inline bool openData(const double &dummy, const size_t &items) { return writeOpenDoubleData(items); }
	inline bool openData(const short &dummy, const size_t &items) { return writeOpenShortData(items); }
	inline bool openData(const unsigned short &dummy, const size_t &items) { return writeOpenUShortData(items); }
	inline bool openData(const int &dummy, const size_t &items) { return writeOpenIntData(items); }
	inline bool openData(const unsigned int &dummy, const size_t &items) { return writeOpenUIntData(items); }
	inline bool openData(const long &dummy, const size_t &items) { return writeOpenLongData(items); }
	inline bool openData(const unsigned long &dummy, const size_t &items) { return writeOpenULongData(items); }
	template<class T> inline void dataItem(const T * const &value, const size_t &count) {}
	inline void dataItem(const float * const &value, const size_t &count) { writeFloatData(value, count); }
	inline void dataItem(const double * const &value, const size_t &count) { writeDoubleData(value, count); }
	inline void dataItem(const short * const &value, const size_t &count) { writeShortData(value, count); }
	inline void dataItem(const unsigned short * const &value, const size_t &count) { writeUShortData(value, count); }
	inline void dataItem(const int * const &value, const size_t &count) { writeIntData(value, count); }
	inline void dataItem(const unsigned int * const &value, const size_t &count) { writeUIntData(value, count); }
	inline void dataItem(const long * const &value, const size_t &count) { writeLongData(value, count); }
	inline void dataItem(const unsigned long * const &value, const size_t &count) { writeULongData(value, count); }
	
	template<class T, size_t n> inline void values(const T (&value)[n], const unsigned int &lineSize = 0) { values(&value[0], n, lineSize); }
	template<class T> inline void values(const T * const &values, const size_t &count, const unsigned int &lineSize = 0) {
		if (openData(values[0], count)) {
			dataItem(values, count);
			writeCloseData();
		} else {
			checkKey();
			arr(values, count, lineSize);
		}
	}

	template<class T> inline void values(const std::vector<T> &values, const unsigned int &lineSize = 0) {
		const size_t count = values.size();
		const size_t bytes = count * sizeof(T);
		if (count > 0 && openData(values[0], count)) {
			for (typename std::vector<T>::const_iterator it = values.begin(); it != values.end(); ++it)
				dataItem(&(*it), 1);
			writeCloseData();
		} else {
			checkKey();
			arr(values, lineSize);
		}
	}

	template<class T, size_t n> inline void valueArray(const T (&value)[n], const bool &iskey = false) { valueArray(&value[0], n, iskey); }
	template<class T> inline void valueArray(const T * const &values, const size_t &size, const bool &iskey = false) { checkKey(); data(values, size); }
	inline void valueArray(const char * const &values, const size_t &size, const bool &iskey) { value(values, iskey); }

	//template<class T> inline void value(const T &value, const bool &iskey = false) { assert(("Not a serializable value", false)); }
	//template<class T> inline void value(const T * const &value, const bool &iskey = false) { assert(("Not a serializable value", false)); }
	inline void value(const char * const &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const bool &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const char &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const short &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const int &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const long &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const float &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const double &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const std::string &value, const bool &iskey) { writeValue(value.c_str(), iskey); }
	inline void value(const unsigned char &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const unsigned short &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const unsigned int &value, const bool &iskey) { writeValue(value, iskey); }
	inline void value(const unsigned long &value, const bool &iskey) { writeValue(value, iskey); }

	template<class V, class N> inline void value(const V &value, const N &name, const bool &iskey = false) { assert(("Not implemented", false)); }

private:
	// Quick workaround because visual studio doesnt always deduct pointer correctly
	template<class T> void _val(const T *v) { __ptr(v); }
	template<class T> typename enable_if<!is_pointer<T>::value>::type _val(const T &v) { __val(v); }

	template<class T> typename enable_if<!is_base_of<ConstSerializable, T>::value, void>::type __val(const T &v) {
		if (!checkKey()) nextValue(inObject(), false);
		value(v, block.wroteKey);
	}
	template<class T> typename enable_if<!is_base_of<ConstSerializable, T>::value, void>::type __ptr(const T *v) { 
		if (!checkKey()) nextValue(inObject(), false);
		value(v, block.wroteKey);
	}
	void __val(const ConstSerializable &v) {
		v.serialize(*this);
	}
	void __ptr(const ConstSerializable *v) {
		v->serialize(*this);
	}
public:
	unsigned int defaultDataLineSize;

	BaseJSONWriter() : block(Block::ROOT), defaultDataLineSize(32) {}

	virtual ~BaseJSONWriter() {
		assert(("Unclosed object or array", !inArray() && !inObject()));
	}

	/** Start writing an array, make sure to call .end(); when finished. */
	inline BaseJSONWriter &arr(const long long &capacity = -1, const unsigned int &maxLineSize = 0)	{ 
		return openBlock(Block::ARRAY, capacity, maxLineSize);  
	}
	/** Start writing an object, make sure to call .end(); when finished. */
	inline BaseJSONWriter &obj(const long long &capacity = -1, const unsigned int &maxLineSize = 0)	{
		return openBlock(Block::OBJECT, capacity, maxLineSize);
	}
	/** End the last started array or object */
	inline BaseJSONWriter &end() {
		return closeBlock();
	}
	/** Check if currently writing an array */
	inline bool inArray() {
		return block.type == Block::ARRAY;
	}
	/** Check if currently writing an object */
	inline bool inObject() {
		return block.type == Block::OBJECT;
	}
	/** Separate between key and value (not required, pure semantics to keep clean code) */
	BaseJSONWriter &is() {
		assert(("No key written to assign", inObject() && block.wroteKey));
		return *this;
	}
	/** Write a null value */
	BaseJSONWriter &nul() {
		if (!checkKey(false)) nextValue(inObject(), false);
		writeNull();
		return *this;
	}
	/* Write a single value */
	template<class T> BaseJSONWriter &val(const T &v) {
		_val(v);
		return *this;
	}
	/** WIP Write an enum value, where v is the scalar value and name is the (textual) representation of it */
	/*template<class V, class N> BaseJSONWriter &val(const V &v, const N &name) {
		if (!checkKey()) nextValue(inObject(), false);
		value<T>(v, name, block.wroteKey);
		return *this;
	}*/
	/* Write a fixed length array of values (usually a string) */
	template<class T, size_t n> BaseJSONWriter &val(const T (&v)[n]) {
		if (!checkKey()) nextValue(inObject(), false);
		valueArray<T, n>(v, block.wroteKey);
		return *this;
	}
	/** WIP Write an enum value, where v is the scalar value and n is the (textual) representation of it */
	/*template<class V, class N, size_t n> BaseJSONWriter &val(const V &v, const N (&name)[n]) {
		if (!checkKey()) nextValue(inObject(), false);
		value<T>(v, name, block.wroteKey);
		return *this;
	}*/
	/** Open an array, write the values and end the array, consider using .data() instead. */
	template<class T> BaseJSONWriter &arr(const T * const &values, const size_t &size, const unsigned int &lineSize = 0) {
		arr(size, lineSize);
		for (size_t i = 0; i < size; i++)
			val(values[i]);
		end();
		return *this;
	}
	/** Open an array, write the values and end the array, consider using .data() instead. */
	template<class T> BaseJSONWriter &arr(const std::vector<T> &values, const unsigned int &lineSize = 0) {
		arr(values.size(), lineSize);
		for (typename std::vector<T>::const_iterator it = values.begin(); it != values.end(); ++it)
			val(*it);
		end();
		return *this;
	}
	/** Write an array of the same type of scalar values in the most optimal way. */
	template<class T> inline BaseJSONWriter &data(const T * const &v, const size_t &size, const unsigned int &lineSize = useDefaultSize) {
		if (!checkKey(false)) nextValue(inObject(), false);
		values(v, size, lineSize == useDefaultSize ? defaultDataLineSize : lineSize);
		return *this;
	}
	/** Write an fixed size array of the same type of scalar values in the most optimal way. */
	template<class T, size_t n> BaseJSONWriter &data(const T (&v)[n], const unsigned int &lineSize = useDefaultSize) {
		if (!checkKey(false)) nextValue(inObject(), false);
		values(v, lineSize == useDefaultSize ? defaultDataLineSize : lineSize);
		return *this;
	}
	/** Write an array of the same type of scalar values in the most optimal way. */
	template<class T> inline BaseJSONWriter &data(const std::vector<T> &v, const unsigned int &lineSize = useDefaultSize) {
		if (!checkKey(false)) nextValue(inObject(), false);
		values(v, lineSize == useDefaultSize ? defaultDataLineSize : lineSize);
		return *this;
	}
	/** Write a key - value pair, sugar for .val(key).is().val(value); */
	template<class K, class V> inline BaseJSONWriter &pair(const K &key, const V &value) {
		return val(key).is().val(value);
	}
	/** Write a key - value pair, sugar for .val(key).is().val(value); */
	template<class K, class V, size_t n> inline BaseJSONWriter &pair(const K &key, const V (&value)[n]) {
		return val(key).is().val(value);
	}
	/** Write a key - value pair, sugar for .val(p.first).is().val(p.second); */
	template<class K, class V> inline BaseJSONWriter &pair(const std::pair<K, V> &p) {
		return pair(p.first, p.second);
	}
	/** Write a key - array pair, sugar for .val(key).is().data(values, size); */
	template<class K, class V> inline BaseJSONWriter &pair(const K * const &key, const V * const &values, const size_t &size) {
		return val(key).is().data(values, size);
	}
	/** Write a key - array pair, sugar for .val(key).is().data(values, size); */
	template<class K, class V> inline BaseJSONWriter &pair(const K * const &key, const std::vector<V> &values) {
		return val(key).is().data(values);
	}

public:
	inline BaseJSONWriter &op(const JSONOp<op_is> &op)		{ return is(); }
	inline BaseJSONWriter &op(const JSONOp<op_nul> &op)		{ return nul(); }
	inline BaseJSONWriter &op(const JSONOp<op_end> &op)		{ return end(); }
	inline BaseJSONWriter &op(const JSONBlockOp<op_arr> &op)	{ return arr(op.capacity); }
	inline BaseJSONWriter &op(const JSONBlockOp<op_obj> &op)	{ return obj(op.capacity); }
	template<class P> inline BaseJSONWriter &op(const JSONPtrOp<op_dat, P> &op)	{ return data(op.ptr, op.size); }

public:
	/** Start writing an array, sugar for .arr(capacity); */
	inline BaseJSONWriter &operator[](const long long &capacity) { return arr(capacity); }
	/** Stream a single value, sugar for .val(value); */
	template<class T> inline BaseJSONWriter &operator<<(const T &v)	{ return val(v);		}
	/** Stream a single (array) value, sugar for .val(value); */
	template<class T> inline BaseJSONWriter &operator,(const T &v)	{ return val(v);		}
	/** Stream a single value after writing a key, sugar for .is().val(value); */
	template<class T> inline BaseJSONWriter &operator=(const T &v)	{ return is().val(v);}
	/** Stream a fixed length array (usually a string) as value, sugar for .val(v); */
	template<class T, size_t n> inline BaseJSONWriter &operator<<(const T (&v)[n])	{ return val(v);		}
	/** Stream a fixed length array (usually a string) as (array) value, sugar for .val(v); */
	template<class T, size_t n> inline BaseJSONWriter &operator,(const T (&v)[n])	{ return val(v);		}
	/** Stream a fixed length array (usually a string) as value after writing a key, sugar for .is().val(v); */
	template<class T, size_t n> inline BaseJSONWriter &operator=(const T (&v)[n])	{ return is().val(v);	}
	/** Stream an array as data, sugar for .data(v); */
	template<class T> inline BaseJSONWriter &operator<<(const std::vector<T> &v){ return data(v);		}
	/** Stream an array as data, sugar for .data(v); */
	template<class T> inline BaseJSONWriter &operator,(const std::vector<T> &v)	{ return data(v);		}
	/** Stream an array as data after writing a key, sugar for .is.data(v); */
	template<class T> inline BaseJSONWriter &operator=(const std::vector<T> &v)	{ return is().data(v);	}
	/** Stream an array as data, sugar for .data(v); */
	template<class T> inline BaseJSONWriter &operator<<(const std::vector<T *> &v){ return arr(v);		}
	/** Stream an array as data, sugar for .data(v); */
	template<class T> inline BaseJSONWriter &operator,(const std::vector<T *> &v)	{ return arr(v);		}
	/** Stream an array as data after writing a key, sugar for .is.data(v); */
	template<class T> inline BaseJSONWriter &operator=(const std::vector<T *> &v)	{ return is().arr(v);	}
	/** Stream an operator, sugar .op(v) */
	template<const char T> inline BaseJSONWriter &operator<<(const JSONOp<T> &v){ return op(v); }
	/** Stream an operator, sugar .op(v) */
	template<const char T> inline BaseJSONWriter &operator,(const JSONOp<T> &v)	{ return op(v); }
	/** Stream an operator after writing a key, sugar .op(v) */
	template<const char T> inline BaseJSONWriter &operator=(const JSONOp<T> &v){ return is().op(v); }
	/** Stream an operator, sugar .op(v) */
	template<const char T> inline BaseJSONWriter &operator<<(const JSONBlockOp<T> &v){ return op(v); }
	/** Stream an operator, sugar .op(v) */
	template<const char T> inline BaseJSONWriter &operator,(const JSONBlockOp<T> &v)	{ return op(v); }
	/** Stream an operator after writing a key, sugar .op(v) */
	template<const char T> inline BaseJSONWriter &operator=(const JSONBlockOp<T> &v){ return is().op(v); }
	/** Stream an operator, sugar .op(v) */
	template<const char T, class P> inline BaseJSONWriter &operator<<(const JSONPtrOp<T, P> &v){ return op(v); }
	/** Stream an operator, sugar .op(v) */
	template<const char T, class P> inline BaseJSONWriter &operator,(const JSONPtrOp<T, P> &v)	{ return op(v); }
	/** Stream an operator after writing a key, sugar .op(v) */
	template<const char T, class P> inline BaseJSONWriter &operator=(const JSONPtrOp<T, P> &v){ return is().op(v); }

};

}

#endif //JSON_BASEJSONWRITER_H