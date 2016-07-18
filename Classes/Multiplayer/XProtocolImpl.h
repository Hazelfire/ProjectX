
#include "MyMath.h"

template<char type> class XMessage;
namespace XProtocol {
	/*
	This header file defines the XProtocol and provides a nice interface for it's use, please continue down the page for actual protocols

	*/


#define PREFIX_SIZE 1

#define XMESSAGE_BASE public: \
	XMessage() { \
		type = MNULL; \
		payload = std::vector<std::string>(); \
	}; \
	\
\
	template <char otherType>\
	XMessage(XMessage<otherType> genericMessage){ \
			type = genericMessage.type; \
			payload = genericMessage.payload;\
	}; \
	operator std::string() const { \
		std::string re = ""; \
		re.push_back(type); \
		for (int payloadIndex = 0; payloadIndex < payload.size(); payloadIndex++) { \
			re.append(payload[payloadIndex]); \
			 \
			if (payloadIndex != payload.size() - 1) { \
				re.push_back(MDELIMITER); \
			} \
		} \
		return re; \
	}; \
		\
				\
	bool isType(const char other) { return(type == other); }; \
				\
private: \
				\
	char type; \
	std::vector<std::string> payload; \
public: 


	// preperation for definitions later 


	template <class To, class From>
	To convert(From myItem) {
		return (To)myItem;
	};

	template <>
	std::string convert<std::string, std::string>(std::string myItem);

	template <>
	std::string convert<std::string,  int>( int myInt);

	template <>
	 int convert< int, std::string>(std::string myString);

	template <>
	std::string convert<std::string,  char>( char myChar);

	template <>
	char convert< char, std::string>(std::string myString);

	template <>
	std::string convert<std::string, unsigned char>(unsigned char myChar);
	
	template <>
	unsigned char convert<unsigned char, std::string>(std::string myString);

	template <>
	std::string convert<std::string, unsigned int>(unsigned int myChar);

	template <>
	unsigned int convert<unsigned int, std::string>(std::string myString);

	template <>
	std::string convert<std::string, float>(float myChar);

	template <>
	float convert<float, std::string>(std::string myString);

	template <>
	std::string convert<std::string, double>(double myChar);

	template <>
	double convert<double, std::string>(std::string myString);
	
	template <>
	std::string convert<std::string, Vec2i>(Vec2i myChar);

	template <>
	Vec2i convert<Vec2i, std::string>(std::string myString);



#define MESSAGE_TYPE_0(__TYPE__) \
template <> \
class XMessage<__TYPE__>{ \
XMESSAGE_BASE \
}

#define MESSAGE_TYPE_1(__TYPE__, __ARGUMENT1__) \
template <> \
class XMessage<__TYPE__>{ \
XMESSAGE_BASE \
	XMessage(__ARGUMENT1__ a1) { \
		type = __TYPE__; \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT1__>(a1)); \
	}; \
	void get(__ARGUMENT1__& a1) { \
		a1 = XProtocol::convert<__ARGUMENT1__, std::string>(payload[0]); \
	} \
}

#define MESSAGE_TYPE_2(__TYPE__, __ARGUMENT1__, __ARGUMENT2__) \
template <> \
class XMessage<__TYPE__>{ \
XMESSAGE_BASE \
	XMessage(__ARGUMENT1__ a1, __ARGUMENT2__ a2) { \
		type = __TYPE__; \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT1__>(a1)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT2__>(a2)); \
	}; \
	void get(__ARGUMENT1__& a1, __ARGUMENT2__& a2) { \
		a1 = XProtocol::convert<__ARGUMENT1__, std::string>(payload[0]); \
		a2 = XProtocol::convert<__ARGUMENT2__, std::string>(payload[1]); \
	}; \
}

#define MESSAGE_TYPE_3(__TYPE__, __ARGUMENT1__, __ARGUMENT2__, __ARGUMENT3__) \
template <> \
class XMessage<__TYPE__>{ \
XMESSAGE_BASE \
	XMessage(__ARGUMENT1__ a1, __ARGUMENT2__ a2, __ARGUMENT3__ a3) { \
		type = __TYPE__; \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT1__>(a1)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT2__>(a2)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT3__>(a3)); \
	}; \
	void get(__ARGUMENT1__& a1, __ARGUMENT2__& a2, __ARGUMENT3__& a3) { \
		a1 = XProtocol::convert<__ARGUMENT1__, std::string>(payload[0]); \
		a2 = XProtocol::convert<__ARGUMENT2__, std::string>(payload[1]); \
		a3 = XProtocol::convert<__ARGUMENT3__, std::string>(payload[2]); \
	}; \
}

#define MESSAGE_TYPE_4(__TYPE__, __ARGUMENT1__, __ARGUMENT2__, __ARGUMENT3__, __ARGUMENT4__) \
template <> \
class XMessage<__TYPE__>{ \
XMESSAGE_BASE \
	XMessage(__ARGUMENT1__ a1, __ARGUMENT2__ a2, __ARGUMENT3__ a3, __ARGUMENT4__ a4) { \
		type = __TYPE__; \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT1__>(a1)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT2__>(a2)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT3__>(a3)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT4__>(a4)); \
	}; \
	void get(__ARGUMENT1__& a1, __ARGUMENT2__& a2, __ARGUMENT3__& a3,  __ARGUMENT4__& a4) { \
		a1 = XProtocol::convert<__ARGUMENT1__, std::string>(payload[0]); \
		a2 = XProtocol::convert<__ARGUMENT2__, std::string>(payload[1]); \
		a3 = XProtocol::convert<__ARGUMENT3__, std::string>(payload[2]); \
		a4 = XProtocol::convert<__ARGUMENT4__, std::string>(payload[3]); \
	}; \
}

#define MESSAGE_TYPE_5(__TYPE__, __ARGUMENT1__, __ARGUMENT2__, __ARGUMENT3__, __ARGUMENT4__, __ARGUMENT5__) \
template <> \
class XMessage<__TYPE__>{ \
XMESSAGE_BASE \
	XMessage(__ARGUMENT1__ a1, __ARGUMENT2__ a2, __ARGUMENT3__ a3, __ARGUMENT3__ a4, __ARGUMENT5__ a5) { \
		type = __TYPE__; \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT1__>(a1)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT2__>(a2)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT3__>(a3)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT4__>(a4)); \
		payload.push_back(XProtocol::convert<std::string, __ARGUMENT5__>(a5)); \
	}; \
	void get(__ARGUMENT1__& a1, __ARGUMENT2__& a2, __ARGUMENT3__& a3,  __ARGUMENT4__& a4,  __ARGUMENT5__& a5) { \
		a1 = XProtocol::convert<__ARGUMENT1__, std::string>(payload[0]); \
		a2 = XProtocol::convert<__ARGUMENT2__, std::string>(payload[1]); \
		a3 = XProtocol::convert<__ARGUMENT3__, std::string>(payload[2]); \
		a4 = XProtocol::convert<__ARGUMENT4__, std::string>(payload[3]); \
		a5 = XProtocol::convert<__ARGUMENT5__, std::string>(payload[4]); \
	}; \
}


}



template <char messageType>
class XMessage {
	XMESSAGE_BASE
};

// All the different types of messages, with seperate constructors
using std::string;

// Null, for making new connections, open members
template <>
class XMessage<MNULL> {
public:
	XMessage() {

		type = MNULL;
		payload = std::vector<std::string>();
	};

	XMessage(std::string message) {

		type = message.front();
		std::stringstream payloadStream(message.substr(PREFIX_SIZE, message.length() - PREFIX_SIZE));
		std::string element;
		while (std::getline(payloadStream, element, MDELIMITER)) {

			payload.push_back(element);
		}
	}

	operator std::string() const {

		std::string re = "";
		re.push_back(type);
		for (int payloadIndex = 0; payloadIndex < payload.size(); payloadIndex++) {

			re.append(payload[payloadIndex]);

			if (payloadIndex != payload.size() - 1) {

				re.push_back(MDELIMITER);
			}
		}
		return re;
	}


	bool isType(const char other) { return(type == other); }

	char type;
	std::vector<std::string> payload;
};
