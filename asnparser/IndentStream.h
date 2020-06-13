#include <iostream>     // cout
#include <algorithm>    // stable_partition
#include <vector>       // vector
#include <locale>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cassert>

using namespace std;

ostream& tab(ostream& stream);
ostream& back(ostream& stream);
ostream& bat(ostream& stream);

class Foo {
public:
	Foo() {}
private:
	Foo*	foo;
};

// OxBADC0DE

class IndentFacet : public codecvt<char, char, mbstate_t> {
  public:
	explicit IndentFacet(int indent_level = 0, size_t ref = 0)
		: codecvt<char, char, mbstate_t>(ref), count(indent_level) {}
	explicit IndentFacet(const int* tabs, size_t ref = 0)
		: current(tabs), codecvt<char, char, mbstate_t>(ref) {}
	typedef codecvt_base::result result;
	typedef codecvt<char, char, mbstate_t> parent;
	typedef parent::intern_type intern_type;
	typedef parent::extern_type extern_type;
	typedef parent::state_type  state_type;

	int &state(state_type &s) const {
		return *reinterpret_cast<int *>(&s);
	}

	void push() {
		count += 1;
	}
	void pop() {
		count = count > 0 ? --count : 0;
	}
	void clear() {
		count = 0;
	}
			int& level()		{ return count; }
	const	int& level() const	{ return count; }

  protected:
	virtual result do_out(state_type &need_indentation,
						  const intern_type *from, const intern_type *from_end, const intern_type *&from_next,
						  extern_type *to, extern_type *to_end, extern_type *&to_next
						 ) const override;

	// Override so the do_out() virtual function is called.
	virtual bool do_always_noconv() const throw() override {
		return count == 0;
	}
	int count = 0;
	const int* current = nullptr;

};

IndentFacet::result IndentFacet::do_out(state_type &need_indentation,
										const intern_type *from, const intern_type *from_end, const intern_type *&from_next,
										extern_type *to, extern_type *to_end, extern_type *&to_next) const {
	result res = codecvt_base::noconv;
	for (; (from < from_end) && (to < to_end); ++from, ++to) {
		// 0 indicates that the last character seen was a newline.
		// thus we will print a tab before it. Ignore it the next
		// character is also a newline
		if ((state(need_indentation) == 0) && (*from != '\n')) {
			res = codecvt_base::ok;
			state(need_indentation) = 1;
			if (current) {
				int tabs = *(current + count - 1) / 4;
				for (int i = 0; i < tabs; ++i) {
					*to = '\t';
					++to;
				}
			} else {
				for (int i = 0; i < count; ++i) {
					*to = '\t';
					++to;
				}
			}
			if (to == to_end) {
				res = codecvt_base::partial;
				break;
			}
		}
		*to = *from; // Copy the next character.

		// If the character copied was a '\n' mark that state
		if (*from == '\n') {
			state(need_indentation) = 0;
		}
	}

	if (from != from_end) {
		res = codecvt_base::partial;
	}
	from_next = from;
	to_next = to;

	return res;
};



/// I hate the way I solved this, but I can't think of a better way
/// around the problem.  I even asked stackoverflow for help:
///
///   http://stackoverflow.com/questions/32480237/apply-a-facet-to-all-stream-output-use-custom-string-manipulators
///
///

static const int indentno = ios_base::xalloc();
static const int facetno  = ios_base::xalloc();
static int* tabs = nullptr;;
//static int  tabs[] = {4, 12, 16, 0};

ostream & push(ostream& os) {
	IndentFacet* facet = (IndentFacet*)os.pword(facetno);
	if (!facet) {
		facet = new IndentFacet(tabs);
		os.pword(facetno) = facet;
	}
	facet->push();
	os.imbue(locale(os.getloc(), facet));
	return os;
}

ostream& pop(ostream& os) {
	IndentFacet* facet = (IndentFacet*)os.pword(facetno);
	if (facet)
		facet->pop();
	return os;
}

/// Clears the ostream indentation set, but NOT the IndentRAAI.
ostream& clear(ostream& os) {
	IndentFacet* facet = (IndentFacet*)os.pword(facetno);
	if (facet)
		facet->clear();
	return os;
}



/// Provides a RAII guard around your manipulation.
class IndentRAAI {
  public:
	IndentRAAI(ostream& os) : oref(os) {
		start_level = ((IndentFacet*)os.pword(facetno))->level();
	}
	~IndentRAAI() {
		reset();
	}

	/// Resets the streams indentation level to the point itw as at
	/// when the guard was created.
	void reset() {
		((IndentFacet*)oref.pword(facetno))->level() = start_level;
	}

  private:
	ostream& oref;
	int start_level;
};


void demo1() {
	IndentRAAI rg(cout);
	cout << "<DEMO START>\n"
		 << push << "This is after a push... "
		 << push << "that last push" << endl
		 << "only affects me on a new line of course."
		 << pop << endl << "Pop has the expected effect." << endl;
	rg.reset();
	cout << "IndentRAAI.reset()  brings me back to the original IndentRAAI state." << endl
		 << "<DEMO STOP>" << endl ;
}

void rai_demo() {
	IndentRAAI rg(cout);
	cout << "The safe indenter object just provides a RAII guard around identation." << endl
		 << push
		 << "If I forget to call reset, or an exception is thrown..." << endl;
	throw runtime_error("oops");
	cout << "The pop never gets called:" << pop;
}



void trivial() {
	/// This probably has to be called once for every program:
	// http://stackoverflow.com/questions/26387054/how-can-i-use-stdimbue-to-set-the-locale-for-stdwcout
	ios_base::sync_with_stdio(false);

	ostream& os = cout;
	os << "I want to push indentation levels:"<< endl << push
	   << "To arbitrary depths" << endl
	   << "To arbitrary depths" << endl << push
	   << "and pop them"<< endl << pop
	   << "back down"<< endl << pop
	   << "like this."<< endl << pop;

}

void indent_demo() {
	ostream& os = cout;
	IndentRAAI rg(os);
	os << "Here I call demo starting at zero indentation level:" << endl << endl;
	demo1();
	os << push
	   << endl << endl << "Now within an indent level:" << endl;
	demo1();

	os << "Demo left the indentation level where it found it." << endl
	   << clear << "Clear pust me at zero." << endl;

	try {
		os << push << push
		   << endl << endl << "The rai guard, resets, it doesn't clear:" << endl;
		rai_demo();
	} catch (runtime_error& e) {
		cerr << endl << "=cerr of course has its own indentation level=" << endl << endl;
		os << "But for instructional purposes we see that the RAII guard retored" << endl
		   << "the original indent level." << endl;

	}
	os << clear
	   << "I can also call clear to make sure I'm at zero at any particular point.\n";
}




bool IsOdd(int i) {
	return (i % 2) == 1;
}

int main(int argc, char** argv) {
#if 0
	trivial();
	indent_demo();
	return 0;

	vector<int> myvector;

	// set some values:
	for (int i = 1; i < 10; ++i) myvector.push_back(i); // 1 2 3 4 5 6 7 8 9

	vector<int>::iterator bound;
	bound = stable_partition(myvector.begin(), myvector.end(), IsOdd);

	// print out content:
	cout << "odd elements:";
	for (vector<int>::iterator it = myvector.begin(); it != bound; ++it)
		cout << ' ' << *it;
	cout << '\n';

	cout << "even elements:";
	for (vector<int>::iterator it = bound; it != myvector.end(); ++it)
		cout << ' ' << *it;
	cout << '\n';
	return 0;
#endif

}