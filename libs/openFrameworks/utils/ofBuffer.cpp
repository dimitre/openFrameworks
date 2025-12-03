#include "ofBuffer.h"
#include "ofAppRunner.h" // ofCore.toDataPath

#include <fstream>

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofBuffer
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------

//--------------------------------------------------
ofBuffer::ofBuffer()
:currentLine(end(),end()){
}

//--------------------------------------------------
ofBuffer::ofBuffer(const char * buffer, std::size_t size)
:buffer(buffer,buffer+size)
,currentLine(end(),end()){
}

//--------------------------------------------------
ofBuffer::ofBuffer(std::istream & stream, std::size_t ioBlockSize)
:currentLine(end(),end()){
	set(stream, ioBlockSize);
}

//--------------------------------------------------
bool ofBuffer::set(std::istream & stream, std::size_t ioBlockSize){
	if(stream.bad()){
		clear();
		return false;
	}else{
		buffer.clear();
	}

	std::vector<char> aux_buffer(ioBlockSize);
	while(stream.good()){
		stream.read(&aux_buffer[0], ioBlockSize);
		append(aux_buffer.data(), stream.gcount());
	}
	return true;
}

//--------------------------------------------------
void ofBuffer::setall(char mem){
	buffer.assign(buffer.size(), mem);
}

//--------------------------------------------------
bool ofBuffer::writeTo(std::ostream & stream) const {
	if(stream.bad()){
		return false;
	}
	stream.write(buffer.data(), buffer.size());
	return stream.good();
}

//--------------------------------------------------
void ofBuffer::set(const char * buffer, std::size_t size){
	this->buffer.assign(buffer, buffer+size);
}

//--------------------------------------------------
void ofBuffer::set(const std::string & text){
	set(text.c_str(), text.size());
}

//--------------------------------------------------
void ofBuffer::append(const std::string& buffer){
	append(buffer.c_str(), buffer.size());
}

//--------------------------------------------------
void ofBuffer::append(const char * buffer, std::size_t size){
	this->buffer.insert(this->buffer.end(), buffer, buffer + size);
}

//--------------------------------------------------
void ofBuffer::reserve(std::size_t size){
	buffer.reserve(size);
}

//--------------------------------------------------
void ofBuffer::clear(){
	buffer.clear();
}

//--------------------------------------------------
void ofBuffer::allocate(std::size_t size){
	resize(size);
}

//--------------------------------------------------
void ofBuffer::resize(std::size_t size){
	buffer.resize(size);
}


//--------------------------------------------------
char * ofBuffer::getData(){
	return buffer.data();
}

//--------------------------------------------------
const char * ofBuffer::getData() const{
	return buffer.data();
}

//--------------------------------------------------
char * ofBuffer::getBinaryBuffer(){
	return getData();
}

//--------------------------------------------------
const char * ofBuffer::getBinaryBuffer() const {
	return getData();
}

//--------------------------------------------------
std::string ofBuffer::getText() const {
	if(buffer.empty()){
		return "";
	}
	return std::string(buffer.begin(), buffer.end());
}

//--------------------------------------------------
ofBuffer::operator std::string() const {
	return getText();
}

//--------------------------------------------------
ofBuffer & ofBuffer::operator=(const std::string & text){
	set(text);
	return *this;
}

//--------------------------------------------------
std::size_t ofBuffer::size() const {
	return buffer.size();
}

//--------------------------------------------------
std::string ofBuffer::getNextLine(){
	if(currentLine.empty()){
		currentLine = getLines().begin();
	}else{
		++currentLine;
	}
	return currentLine.asString();
}

//--------------------------------------------------
std::string ofBuffer::getFirstLine(){
	currentLine = getLines().begin();
	return currentLine.asString();
}

//--------------------------------------------------
bool ofBuffer::isLastLine(){
	return currentLine == getLines().end();
}

//--------------------------------------------------
void ofBuffer::resetLineReader(){
	currentLine = getLines().begin();
}

//--------------------------------------------------
std::vector<char>::iterator ofBuffer::begin(){
	return buffer.begin();
}

//--------------------------------------------------
std::vector<char>::iterator ofBuffer::end(){
	return buffer.end();
}

//--------------------------------------------------
std::vector<char>::const_iterator ofBuffer::begin() const{
	return buffer.begin();
}

//--------------------------------------------------
std::vector<char>::const_iterator ofBuffer::end() const{
	return buffer.end();
}

//--------------------------------------------------
std::vector<char>::reverse_iterator ofBuffer::rbegin(){
	return buffer.rbegin();
}

//--------------------------------------------------
std::vector<char>::reverse_iterator ofBuffer::rend(){
	return buffer.rend();
}

//--------------------------------------------------
std::vector<char>::const_reverse_iterator ofBuffer::rbegin() const{
	return buffer.rbegin();
}

//--------------------------------------------------
std::vector<char>::const_reverse_iterator ofBuffer::rend() const{
	return buffer.rend();
}

//--------------------------------------------------
ofBuffer::Line::Line(std::vector<char>::iterator _begin, std::vector<char>::iterator _end)
	:_current(_begin)
	,_begin(_begin)
	,_end(_end){

	if(_begin == _end){
		line =  "";
		return;
	}

	_current = std::find(_begin, _end, '\n');
	if(_current - 1 >= _begin && *(_current - 1) == '\r'){
		line = std::string(_begin, _current - 1);
	}else{
		line = std::string(_begin, _current);
	}
	if(_current != _end){
		_current+=1;
	}
}

//--------------------------------------------------
const std::string & ofBuffer::Line::operator*() const{
	return line;
}

//--------------------------------------------------
const std::string * ofBuffer::Line::operator->() const{
	return &line;
}

//--------------------------------------------------
const std::string & ofBuffer::Line::asString() const{
	return line;
}

//--------------------------------------------------
ofBuffer::Line & ofBuffer::Line::operator++(){
	*this = Line(_current,_end);
	return *this;
}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Line::operator++(int) {
	Line tmp(*this);
	operator++();
	return tmp;
}

//--------------------------------------------------
bool ofBuffer::Line::operator!=(Line const& rhs) const{
	return rhs._begin != _begin || rhs._end != _end;
}

//--------------------------------------------------
bool ofBuffer::Line::operator==(Line const& rhs) const{
	return rhs._begin == _begin && rhs._end == _end;
}

bool ofBuffer::Line::empty() const{
	return _begin == _end;
}



//--------------------------------------------------
ofBuffer::RLine::RLine(std::vector<char>::reverse_iterator _rbegin, std::vector<char>::reverse_iterator _rend)
	:_current(_rbegin)
	,_rbegin(_rbegin)
	,_rend(_rend){

	if(_rbegin == _rend){
		line =  "";
		return;
	}
	_current = std::find(_rbegin+1, _rend, '\n');
	line = std::string(_current.base(), _rbegin.base() - 1);
	if(_current < _rend-1 && *(_current + 1) == '\r'){
		_current+=1;
	}
}

//--------------------------------------------------
const std::string & ofBuffer::RLine::operator*() const{
	return line;
}

//--------------------------------------------------
const std::string * ofBuffer::RLine::operator->() const{
	return &line;
}

//--------------------------------------------------
const std::string & ofBuffer::RLine::asString() const{
	return line;
}

//--------------------------------------------------
ofBuffer::RLine & ofBuffer::RLine::operator++(){
	*this = RLine(_current,_rend);
	return *this;
}

//--------------------------------------------------
ofBuffer::RLine ofBuffer::RLine::operator++(int) {
	RLine tmp(*this);
	operator++();
	return tmp;
}

//--------------------------------------------------
bool ofBuffer::RLine::operator!=(RLine const& rhs) const{
	return rhs._rbegin != _rbegin || rhs._rend != _rend;
}

//--------------------------------------------------
bool ofBuffer::RLine::operator==(RLine const& rhs) const{
	return rhs._rbegin == _rbegin && rhs._rend == _rend;
}

bool ofBuffer::RLine::empty() const{
	return _rbegin == _rend;
}

//--------------------------------------------------
ofBuffer::Lines::Lines(std::vector<char>::iterator begin, std::vector<char>::iterator end)
:_begin(begin)
,_end(end){}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Lines::begin(){
	return Line(_begin,_end);
}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Lines::end(){
	return Line(_end,_end);
}


//--------------------------------------------------
ofBuffer::RLines::RLines(std::vector<char>::reverse_iterator rbegin, std::vector<char>::reverse_iterator rend)
:_rbegin(rbegin)
,_rend(rend){}

//--------------------------------------------------
ofBuffer::RLine ofBuffer::RLines::begin(){
	return RLine(_rbegin,_rend);
}

//--------------------------------------------------
ofBuffer::RLine ofBuffer::RLines::end(){
	return RLine(_rend,_rend);
}

//--------------------------------------------------
ofBuffer::Lines ofBuffer::getLines(){
	return ofBuffer::Lines(begin(), end());
}

//--------------------------------------------------
ofBuffer::RLines ofBuffer::getReverseLines(){
	return ofBuffer::RLines(rbegin(), rend());
}

//--------------------------------------------------
std::ostream & operator<<(std::ostream & ostr, const ofBuffer & buf){
	buf.writeTo(ostr);
	return ostr;
}

//--------------------------------------------------
std::istream & operator>>(std::istream & istr, ofBuffer & buf){
	buf.set(istr);
	return istr;
}

//--------------------------------------------------
ofBuffer ofBufferFromFile(const fs::path & path, bool binary){
	std::ifstream fileFrom(ofCore.toDataPath(path));
	return ofBuffer(fileFrom);

//	ofFile f(path,ofFile::ReadOnly, binary);
//	return ofBuffer(f);
}

//--------------------------------------------------
bool ofBufferToFile(const fs::path & path, const ofBuffer& buffer, bool binary){
//	ofFile f(path, ofFile::WriteOnly, binary);
//	return buffer.writeTo(f);
	
	std::ofstream fileOut(ofCore.toDataPath(path));
	return buffer.writeTo(fileOut);
}
