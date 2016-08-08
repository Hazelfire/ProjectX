#include "DocsParser.h"
#include <cocos2d.h>
#include <yaml-cpp/yaml.h>
#include "Debug.h"
#include <sstream>
#include "Parser/StringOperations.h"

LuaDocs::FunctionDocs parseFunction(YAML::Node functionNode) {
	FunctionDocs currentFunctionDocs;
	currentFunctionDocs.docs = functionNode["docs"].as<std::string>();

	if (functionNode["overload"]) {
		YAML::Node overloadsNode = functionNode["overloads"];

		for (auto overloadNode : overloadsNode) {
			std::list<std::pair<std::string, std::string>> overload;
			std::stringstream ss(overloadNode.as<std::string>());
			std::string paramater;

			while (std::getline(ss, paramater, ',')) {
				paramater = StringOps::trim(paramater);
				std::string type = paramater.substr(0, paramater.find_first_of(' '));
				std::string paramaterName = paramater.substr(paramater.find_first_of(' ') + 1, paramater.length() - paramater.find_first_of(' ') - 1);
				overload.push_back({ type, paramaterName });
			}
			currentFunctionDocs.overloads.push_back(overload);
		}
	}
	else {
		// No overloads, push an empty function as an overload
		currentFunctionDocs.overloads.push_front(std::list<std::pair<std::string, std::string>>());
	}

	if (functionNode["returns"]) {
		currentFunctionDocs.returnType = functionNode["returns"].as<std::string>();
	}
	else {
		currentFunctionDocs.returnType = "nil";
	}

	return currentFunctionDocs;
}


DocSpec DocsParser::parse(std::string scope) {
	DocSpec re;
	std::string docsFile = cocos2d::FileUtils::getInstance()->getStringFromFile("docs.yml");

	try {
		YAML::Node root = YAML::Load(docsFile);
		YAML::Node scopeNode = root[scope];
		YAML::Node functionsNode = scopeNode["functions"];
		
		// function parsing
		for (auto functionNode : functionsNode) {
			re.functions[functionNode.first.as<std::string>()] = parseFunction(functionNode.second);
		}

		// Object parsing
		YAML::Node objectsNode = scopeNode["objects"];
		for (auto objectNode : objectsNode) {
			ObjectDocs currentObject;

			for (auto functionNode : objectNode["functions"]) {
				currentObject.memberFunctions[functionNode.first.as<std::string>()] = parseFunction(functionNode.second);
			}

			re.objects[objectNode.first.as<std::string>()] = currentObject;
		}

		// parenting!
		if (scopeNode["parent"]) {
			// Parse parent scope
			DocSpec parentScope = parse(scopeNode["parent"].as<std::string>());

			// And.. MERGE
			// functions...
			for (auto parentObject : parentScope.objects){
				re.objects.insert(parentObject);
			}

			for (auto parentFunction : parentScope.functions) {
				re.functions.insert(parentFunction);
			}
		}

		re.folder = scopeNode["folder"].as<std::string>();
	}
	catch (YAML::Exception exception) {
		// Hopefully this wont happen
		// Hopefully...
	}

	return re;
}