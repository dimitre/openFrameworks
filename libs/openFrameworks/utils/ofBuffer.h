#pragma once

// MARK: ofConstants FS
#include "ofConstants.h"

#include <vector>

//----------------------------------------------------------
// ofBuffer
//----------------------------------------------------------

/// \class ofBuffer
///
/// A buffer of data which can be accessed as simple bytes or text.
///
class ofBuffer {

public:
	ofBuffer();

	/// Create a buffer and set its contents from a raw byte pointer.
	///
	/// \param buffer pointer to the raw byte buffer to copy data from
	/// \param size the number of bytes to read
	/// \warning buffer *must* not be NULL
	/// \warning size *must* be <= the number of bytes allocated in buffer
	ofBuffer(const char * buffer, std::size_t size);

	/// Create a buffer and set its contents from an input stream.
	///
	/// \param ioBlockSize the number of bytes to read from the stream in chunks
	ofBuffer(std::istream & stream, std::size_t ioBlockSize = 1024);

	
	// FIXME: make an ofBuffer initializer using fs::path as a parameter
//	ofBuffer(const fs::path & fileName);

	
	/// Set the contents of the buffer from a raw byte pointer.
	///
	/// \warning buffer *must* not be NULL
	/// \warning size *must* be <= the number of bytes allocated in buffer
	/// \param buffer pointer to the raw byte buffer to copy data from
	/// \param size the number of bytes to read
	void set(const char * buffer, std::size_t size);

	/// Set contents of the buffer from a string.
	///
	/// \param text string to copy data from
	void set(const std::string & text);

	/// Set contents of the buffer from an input stream.
	///
	/// \param stream input stream to copy data from
	/// \param ioBlockSize the number of bytes to read from the stream in chunks
	bool set(std::istream & stream, std::size_t ioBlockSize = 1024);

	/// Set all bytes in the buffer to a given value.
	///
	/// \param mem byte value to set
	void setall(char mem);

	/// Append bytes to the end of buffer from a string.
	///
	/// \param buffer string to copy bytes from
	void append(const std::string & buffer);

	/// Append bytes to the end of the buffer from a raw byte pointer.
	///
	/// \warning buffer *must* not be NULL
	/// \warning size *must* be <= the number of bytes allocated in buffer
	/// \param buffer pointer to the raw byte buffer to copy data from
	/// \param size the number of bytes to read
	void append(const char * buffer, std::size_t size);

	/// Request that the buffer capacity be at least enough to contain a
	/// specified number of bytes.
	///
	/// \param size number of bytes to reserve space for
	void reserve(std::size_t size);

	/// Write contents of the buffer to an output stream.
	bool writeTo(std::ostream & stream) const;

	/// Remove all bytes from the buffer, leaving a size of 0.
	void clear();

	/// Request that the buffer capacity be at least enough to contain a
	/// specified number of bytes.
	///
	/// \param size number of bytes to reserve space for
	void allocate(std::size_t size);

	/// Resize the buffer to contain a specified number of bytes.
	///
	/// If size is < the current buffer size, the contents are reduced to size
	/// bytes & remaining bytes are removed. If size is > the current buffer
	/// size, the buffer's size is increased to size_ bytes.
	///
	/// \param size number of bytes to resize the buffer to
	void resize(std::size_t size);

	/// Access the buffer's contents using a raw byte pointer.
	///
	/// \warning Do not access bytes at indices beyond size()!
	/// \returns pointer to internal raw bytes
	char * getData();

	/// access the buffer's contents using a const raw byte pointer.
	///
	/// \warning Do not access bytes at indices beyond size()!
	/// \returns const pointer to internal raw bytes
	const char * getData() const;
	[[deprecated("Use getData")]]
	char * getBinaryBuffer();
	[[deprecated("Use getData")]]
	const char * getBinaryBuffer() const;

	/// get the contents of the buffer as a string.
	///
	/// \returns buffer contents as a string
	std::string getText() const;

	/// Use buffer as a string via cast.
	///
	/// \returns buffer contents as a string
	operator std::string() const;

	/// set contents of the buffer from a string
	ofBuffer & operator=(const std::string & text);

	/// Check the buffer's size.
	///
	/// \returns the size of the buffer's content in bytes
	std::size_t size() const;

	[[deprecated("use a lines iterator")]]
	std::string getNextLine();
	[[deprecated("use a lines iterator")]]
	std::string getFirstLine();
	[[deprecated("use a lines iterator")]]
	bool isLastLine();
	[[deprecated("use a lines iterator")]]
	void resetLineReader();

	friend std::ostream & operator<<(std::ostream & ostr, const ofBuffer & buf);
	friend std::istream & operator>>(std::istream & istr, ofBuffer & buf);

	std::vector<char>::iterator begin();
	std::vector<char>::iterator end();
	std::vector<char>::const_iterator begin() const;
	std::vector<char>::const_iterator end() const;
	std::vector<char>::reverse_iterator rbegin();
	std::vector<char>::reverse_iterator rend();
	std::vector<char>::const_reverse_iterator rbegin() const;
	std::vector<char>::const_reverse_iterator rend() const;

	/// A line of text in the buffer.
	///

	struct Line {
		Line(std::vector<char>::iterator _begin, std::vector<char>::iterator _end);
		const std::string & operator*() const;
		const std::string * operator->() const;
		const std::string & asString() const;

		using value_type = std::string;
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using pointer = const value_type *;
		using reference = const value_type &;

		/// Increment to the next line.
		Line & operator++();

		/// Increment to a number of lines.
		Line operator++(int);

		bool operator!=(Line const & rhs) const;
		bool operator==(Line const & rhs) const;

		/// Is this line empty? (aka an empty string "")
		bool empty() const;

	private:
		std::string line;
		std::vector<char>::iterator _current, _begin, _end;
	};

	/// A line of text in the buffer.
	///
	struct RLine {
		RLine(std::vector<char>::reverse_iterator _begin, std::vector<char>::reverse_iterator _end);
		const std::string & operator*() const;
		const std::string * operator->() const;
		const std::string & asString() const;

		using value_type = std::string;
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using pointer = const value_type *;
		using reference = const value_type &;

		/// Increment to the next line.
		RLine & operator++();

		/// Increment to a number of lines.
		RLine operator++(int);

		bool operator!=(RLine const & rhs) const;
		bool operator==(RLine const & rhs) const;

		/// Is this line empty? (aka an empty string "")
		bool empty() const;

	private:
		std::string line;
		std::vector<char>::reverse_iterator _current, _rbegin, _rend;
	};

	/// A series of text lines in the buffer.
	///
	struct Lines {
		Lines(std::vector<char>::iterator begin, std::vector<char>::iterator end);

		/// Get the first line in the buffer.
		Line begin();

		/// Get the last line in the buffer.
		Line end();

		RLine rbegin();
		RLine rend();

	private:
		std::vector<char>::iterator _begin, _end;
	};

	/// A series of text lines in the buffer.
	///
	struct RLines {
		RLines(std::vector<char>::reverse_iterator rbegin, std::vector<char>::reverse_iterator rend);

		/// Get the first line in the buffer.
		RLine begin();

		/// Get the last line in the buffer.
		RLine end();

	private:
		std::vector<char>::reverse_iterator _rbegin, _rend;
	};

	/// Access the contents of the buffer as a series of text lines.
	///
	/// If the buffer loads a text file with lines separated by an endline
	/// char '\n', you can access each line individually using Line structs.
	///
	/// \returns buffer text lines
	Lines getLines();

	/// Access the contents of the buffer as a series of text lines in reverse
	/// order
	///
	/// If the buffer loads a text file with lines separated by an endline
	/// char '\n' or '\r\n', you can access each line individually using Line structs.
	///
	/// \returns buffer text lines
	RLines getReverseLines();

private:
	std::vector<char> buffer;
	Line currentLine;
};

//--------------------------------------------------
/// Read the contents of a file at path into a buffer.
///
/// Opens as a text file by default.
///
/// \param path file to open
/// \param binary set to false if you are reading a text file & want lines
/// split at endline characters automatically
ofBuffer ofBufferFromFile(const fs::path & path, bool binary = true);

//--------------------------------------------------
/// Write the contents of a buffer to a file at path.
///
/// Saves as a text file by default.
///
/// \param path file to open
/// \param buffer data source to write from
/// \param binary set to false if you are writing a text file & want lines
/// split at endline characters automatically
bool ofBufferToFile(const fs::path & path, const ofBuffer & buffer, bool binary = true);
