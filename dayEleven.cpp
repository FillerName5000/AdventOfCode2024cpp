// COMMENTED OUT CODE DISCLAIMER
// To ensure compilation for the other files with the BigInt.hpp or any other external header files, this entire file is commented out. 
// In the current structure that only uses one file at a time, header files are not perfectly organised.
// Having many files in a project you have no intention of using is not a normal use case, therefore organising this perfectly is not a priority.
// To test this file, get the BigInt hpp file from github and add it to the Header Files, in my case in a "libs" filter. 
// Comment out any other cpp files using it. Only the .cpp files should need to be edited.
// /COMMENTED OUT CODE DISCLAIMER
//#include <cstdlib>
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include "BigInt.hpp"
//
///** GENERAL NOTE:
//*
//* Several methods used in this project would be a good fit for extension methods.
//* Given the structure of the project this could lead do dependency issues.
//* The scope for every file is also small enough to not need it.
//*
//*/
//
//
//namespace DayEleven
//{
//	namespace {
//		void coutVectorContents(const std::string prefix, const std::vector<BigInt>& vector);
//		void createStonesStartState(std::string& line);
//
//		const bool toLog = true; // true   false
//		const char kDelim = ' ';
//		const int maxLoops = 25;
//		const int kMultiplier = 2024;
//
//		std::vector<BigInt> stones;
//
//		void iterateStones()
//		{
//			size_t size = stones.size();
//			std::vector<BigInt> newStoneState;
//			for (const auto& stone : stones)
//			{
//				if (stone == 0) {
//					newStoneState.push_back(1);
//					continue;
//				}
//				std::string stoneStr = stone.to_string();
//				size_t len = stoneStr.length();
//				if (len % 2 == 0) {
//					newStoneState.push_back(BigInt(stoneStr.substr(0, len / 2)));
//					newStoneState.push_back(BigInt(stoneStr.substr(len / 2)));
//				}
//				else {
//					newStoneState.push_back(stone * kMultiplier);
//				}
//			}
//			stones = newStoneState;
//		}
//
//		size_t getAmountOfStonesFromLine(std::string& line)
//		{
//			createStonesStartState(line);
//			if (toLog) coutVectorContents("Start state: ", stones);
//
//			for (size_t i = 0; i < maxLoops; i++)
//			{
//				iterateStones();
//				std::string str = "State " + std::to_string(i) + ":\n";
//				if (toLog) std::cout << str << stones.size() << '\n';
//			}
//
//			return 0;
//		}
//
//		void handleFile(std::ifstream& inputFile)
//		{
//			std::pair<int, int> startingGuardCoord;
//			if (inputFile.is_open()) {
//				std::string line;
//				size_t stoneTotal = 0;;
//				while (getline(inputFile, line)) { // single line
//					stoneTotal = getAmountOfStonesFromLine(line);
//				}
//
//				std::cout << "Finished reading file\n";
//
//				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.
//
//				std::cout << "# of stones: " << stoneTotal << '\n';
//
//				std::cout << "\nFinished running program";
//			}
//			else {
//				std::cout << "Unable to open file\n";
//			}
//		}
//
//		void createStonesStartState(std::string& line)
//		{
//			size_t pos = 0;
//			while ((pos = line.find(kDelim)) != std::string::npos) {
//				stones.emplace_back(line.substr(0, pos));
//				line.erase(0, pos + 1);
//			}
//			if (!line.empty()) {
//				stones.emplace_back(line);
//			}
//		}
//
//		void coutVectorContents(const std::string prefix, const std::vector<BigInt>& vector)
//		{
//			std::cout << prefix;
//			std::cout << "[";
//			for (const auto& num : vector) {
//				std::cout << num << ",";
//			}
//			std::cout << "]\n";
//		}
//	}
//	void dayEleven() {
//		std::system("cls"); // clear terminal pre-boot
//		std::cout << "Running program DayEleven" << '\n';
//		const bool isFullFile = true; // true   false
//
//		std::string line;
//		std::ifstream inputFile;
//		if (isFullFile) {
//			std::cout << "Using full file\n\n";
//		}
//		else {
//			std::cout << "Using test file\n\n";
//		}
//		(isFullFile) ? inputFile.open("dayElevenFull.txt") : inputFile.open("dayElevenTest.txt");
//		handleFile(inputFile);
//	}
//}
