#include "IndentStream.h"

std::ostream& tab(std::ostream& stream) {
	IndentStream* pIndentStream = dynamic_cast<IndentStream*>(&stream);
	if (pIndentStream != nullptr) {
		pIndentStream->ib.tab();
	}
	return stream;
}

std::ostream& back(std::ostream& stream) {
	IndentStream* pIndentStream = dynamic_cast<IndentStream*>(&stream);
	if (pIndentStream != nullptr) {
		pIndentStream->ib.back();
	}
	return stream;
}

std::ostream& bat(std::ostream& stream) {
	IndentStream* pIndentStream = dynamic_cast<IndentStream*>(&stream);
	if (pIndentStream != nullptr) {
		pIndentStream->ib.bat();
	}
	return stream;
}

