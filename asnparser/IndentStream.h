#ifndef INDENTSTREAM_H_
#define INDENTSTREAM_H_

#include <iostream>
#include <streambuf>
#include <locale>
#include <cstdio>

using std::clog;
using std::endl;
using std::streambuf;
using std::fill_n;
using std::ostream;
using std::ostreambuf_iterator;


class IndentBuffer : public streambuf {
public:
	IndentBuffer(streambuf* sbuf) : _sbuf(sbuf), _count(0), _need(true) {}

	int tab() const {
		return _count;
	}
	void tab() {
		_count += 1;
	}
	void back() {
		if (_count > 0) _count -= 1; else clog << "back while count is 0" << endl;
	}
	void bat() {
		if (_count > 0) _count -= 1; else clog << "bat while count is 0" << endl;
	}

protected:
	virtual int_type overflow(int_type c) {

		if (traits_type::eq_int_type(c, traits_type::eof()))
			return _sbuf->sputc(c);

		if (_need) {
			fill_n(std::ostreambuf_iterator<char>(_sbuf), _count, '\t');
			_need = false;
		}

		if (traits_type::eq_int_type(_sbuf->sputc(c), traits_type::eof()))
			return traits_type::eof();

		if (traits_type::eq_int_type(c, traits_type::to_char_type('\n')))
			_need = true;

		return traits_type::not_eof(c);
	}

	streambuf* _sbuf;
	int _count;
	bool _need;
};

class IndentStream : public std::ostream {
  public:
	IndentStream(std::ostream &os) : ib(os.rdbuf()), std::ostream(&ib) {};
	friend std::ostream& tab(std::ostream& stream);
	friend std::ostream& back(std::ostream& stream);
	friend std::ostream& bat(std::ostream& stream);

  private:
	IndentBuffer ib;
};
#endif