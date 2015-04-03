#line 2 "togo/core/io/memory_stream.cpp"
/**
@copyright MIT license; see @ref index or the accompanying LICENSE file.
*/

#include <togo/core/config.hpp>
#include <togo/core/utility/utility.hpp>
#include <togo/core/memory/memory.hpp>
#include <togo/core/collection/array.hpp>
#include <togo/core/io/memory_stream.hpp>

namespace togo {

// class MemoryStream implementation

MemoryStream::~MemoryStream() = default;

MemoryStream::MemoryStream(
	Allocator& allocator,
	u32 const init_capacity
)
	: _status()
	, _data(allocator)
	, _position(0)
{
	array::set_capacity(_data, init_capacity);
}

void MemoryStream::clear() {
	array::clear(_data);
	_position = 0;
}

IOStatus MemoryStream::status() const {
	return _status;
}

u64 MemoryStream::position() {
	return _position;
}

u64 MemoryStream::seek_to(u64 const position) {
	_position = min(position, static_cast<u64>(array::size(_data)));
	return _position;
}

u64 MemoryStream::seek_relative(s64 const offset) {
	_position = static_cast<u64>(
		clamp<s64>(static_cast<s64>(_position) + offset, 0, array::size(_data))
	);
	return _position;
}

IOStatus MemoryStream::read(void* const data, unsigned size, unsigned* const read_size) {
	if (_position + size > array::size(_data)) {
		if (read_size) {
			size = array::size(_data) - _position;
			_status.assign(false, true);
		} else {
			return _status.assign(false, true);
		}
	} else {
		_status.clear();
	}
	std::memcpy(data, array::begin(_data) + _position, size);
	_position += size;
	if (read_size) {
		*read_size = size;
	}
	return _status;
}

IOStatus MemoryStream::write(void const* const data, unsigned const size) {
	array::resize(_data, array::size(_data) + size);
	std::memcpy(array::begin(_data) + _position, data, size);
	_position += size;
	return _status.clear();
}

// class MemoryReader implementation

MemoryReader::~MemoryReader() = default;

MemoryReader::MemoryReader(u8 const* const buffer, u32_fast const size)
	: _status()
	, _size(size)
	, _data(buffer)
	, _position(0)
{
	TOGO_ASSERTE(_data);
}

MemoryReader::MemoryReader(StringRef const& ref)
	: _status()
	, _size(ref.size)
	, _data(reinterpret_cast<u8 const*>(ref.data))
	, _position(0)
{
	TOGO_ASSERTE(_data);
}

IOStatus MemoryReader::status() const {
	return _status;
}

u64 MemoryReader::position() {
	return _position;
}

u64 MemoryReader::seek_to(u64 const position) {
	_position = min(position, static_cast<u64>(_size));
	return _position;
}

u64 MemoryReader::seek_relative(s64 const offset) {
	_position = static_cast<u64>(
		clamp<s64>(static_cast<s64>(_position) + offset, 0, _size)
	);
	return _position;
}

IOStatus MemoryReader::read(void* const data, unsigned size, unsigned* const read_size) {
	if (_position + size > _size) {
		if (read_size) {
			size = _size - _position;
			_status.assign(false, true);
		} else {
			return _status.assign(false, true);
		}
	} else {
		_status.clear();
	}
	std::memcpy(data, _data + _position, size);
	_position += size;
	if (read_size) {
		*read_size = size;
	}
	return _status;
}

} // namespace togo
