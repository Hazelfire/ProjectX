#include <unordered_map>
#include <list>
#include <string>



class LuaDocs {
public:
	LuaDocs(std::string scope);

private:
	struct FunctionDocs {
		std::string docs;
		std::list<std::list<std::pair<std::string, std::string>>> overloads;
		std::string returnType;
	};

	struct ObjectDocs {
		std::unordered_map<std::string, FunctionDocs> memberFunctions;
	};

	struct DocSpec {
		std::unordered_map<std::string, FunctionDocs> functions;
		std::unordered_map<std::string, ObjectDocs> objects;
		std::string folder;
	};
	DocSpec specification;
};