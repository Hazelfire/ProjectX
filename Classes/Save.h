#pragma once
#include <string>
#include "SaveInformation.h"
#include "YAML/SaveParser.h"
#include "FileOperation.h"


#define SAVE_FILE "SaveFile.yml"

class Save {
public:
	// Saves a certain piece of information
	template <class ReadType>
	static ReadType read(ReadType SaveInformation::*member) {
			if (!m_saveInfoLoaded) {
				m_saveInfo = loadInfoFromFile();
				m_saveInfoLoaded = true;
			}

			// returns the member requested
			return m_saveInfo.*member;
	};

	// Reads a certain peice of information, as specified above
	template <class WriteType>
	static void write(WriteType data, WriteType SaveInformation::*member) {
		if (!m_saveInfoLoaded) {
			m_saveInfo = loadInfoFromFile();
			m_saveInfoLoaded = true;
		}

		// Saves the member requested
		m_saveInfo.*member = data;


		YAMLSave::writeSave(m_saveInfo, SAVE_FILE);
	};

private:

	// Loads the information from the save file
	static SaveInformation loadInfoFromFile() {
		return YAMLSave::readSave(SAVE_FILE);
	};

	// The information
	static SaveInformation m_saveInfo;

	static bool m_saveInfoLoaded;
};
