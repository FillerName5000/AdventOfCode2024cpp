//#include <cstdlib>
//#include <iostream>
//#include <fstream>
//#include <string>
//#include <vector>
//#include <unordered_map> // unordered map requires hash function, which BigInt.hpp does not have
//#include <map>
//#include "BigInt.hpp" // https://github.com/faheel/BigInt
//
///** GENERAL NOTE:
//*
//* Several methods used in this project would be a good fit for extension methods.
//* Given the structure of the project this could lead do dependency issues.
//* The scope for every file is also small enough to not need it.
//*
//*/
//
//namespace DayElevenPartTwo
//{
//	namespace {
//		void createStonesStartState(std::string& line);
//		void coutMapContents(const std::map<BigInt, BigInt>& map);
//		void coutMapTotal(const std::map<BigInt, BigInt>& map);
//
//		const bool toLog = false; // true   false
//		const char kDelim = ' ';
//		const int maxLoops = 75;
//		const int kMultiplier = 2024;
//
//		std::map<BigInt, BigInt> stoneCounts;
//
//		void iterateStones(std::map<BigInt, BigInt> stoneCountsCopy)
//		{
//			auto pair = stoneCountsCopy.begin();
//			while (pair != stoneCountsCopy.end())
//			{
//				const BigInt stone = pair->first;
//				BigInt count = pair->second;
//
//				if (count == 0) {
//					pair++;
//					continue;
//				}
//
//				if (toLog) {
//					std::cout << "Processing stone: " << stone.to_string() << ", Count: " << count.to_string() << "\n";
//				}
//
//				if (stone == 0) {
//					stoneCounts[0] -= count;
//					stoneCounts[1] += count;
//					if (toLog) {
//						std::cout << "Decremented stone 0, Incremented stone 1\n";
//					}
//				}
//				else {
//					std::string stoneStr = stone.to_string();
//					size_t len = stoneStr.length();
//					if (len % 2 == 0) {
//						stoneCounts[stone] -= count;
//						BigInt firstHalf(stoneStr.substr(0, len / 2));
//						BigInt secondHalf(stoneStr.substr(len / 2));
//						stoneCounts[firstHalf] += count;
//						stoneCounts[secondHalf] += count;
//						if (toLog) {
//							std::cout << "Split stone " << stone.to_string() << " into " << firstHalf.to_string()
//								<< " and " << secondHalf.to_string() << "\n";
//						}
//					}
//					else {
//						stoneCounts[stone] -= count;
//						stoneCounts[stone * kMultiplier] += count;
//						if (toLog) {
//							std::cout << "Multiplied stone " << stone.to_string() << " by " << kMultiplier
//								<< " to get " << (stone * kMultiplier).to_string() << "\n";
//						}
//					}
//				}
//				pair++; // iterator increment
//			}
//		}
//
//		BigInt getAmountOfStonesFromLine(std::string& line)
//		{
//			createStonesStartState(line);
//
//			if (toLog) {
//				coutMapContents(stoneCounts);
//				std::cout << '\n';
//			}
//
//			for (size_t i = 0; i < maxLoops; i++)
//			{
//				iterateStones(stoneCounts);
//				if (toLog) coutMapContents(stoneCounts);
//			}
//
//			BigInt total = 0;
//			for (const auto& pair : stoneCounts)
//			{
//				total += pair.second;
//			}
//
//			return total;
//		}
//
//		void handleFile(std::ifstream& inputFile)
//		{
//			std::pair<int, int> startingGuardCoord;
//			if (inputFile.is_open()) {
//				std::string line;
//				BigInt stoneTotal = 0;;
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
//		void coutMapTotal(const std::map<BigInt, BigInt>& map)
//		{
//			BigInt total = 0;
//			for (const auto& pair : map)
//			{
//				total += pair.second;
//			}
//			std::cout << "Total: " << total.to_string() << "\n";
//		}
//
//		void coutMapContents(const std::map<BigInt, BigInt>& map)
//		{
//			const std::string keyColor = "\033[38;5;214m"; 
//			const std::string valueColor = "\033[32m"; 
//			const std::string zeroKeyColor = "\033[31m";
//			const std::string zeroValueColor = "\033[34m";
//			const std::string resetColor = "\033[0m";
//
//			std::cout << "{";
//			for (const auto& pair : map)
//			{
//				if (pair.second == 0)
//				{
//					std::cout << zeroKeyColor << pair.first.to_string() << resetColor << ":"
//						<< zeroValueColor << pair.second.to_string() << resetColor;
//				}
//				else
//				{
//					std::cout << keyColor << pair.first.to_string() << resetColor << ":"
//						<< valueColor << pair.second.to_string() << resetColor;
//				}
//				std::cout << ", ";
//			}
//			std::cout << "}\n";
//			coutMapTotal(stoneCounts);
//			std::cout << "\n\n";
//		}
//
//		void createStonesStartState(std::string& line)
//		{
//			size_t pos = 0;
//			while ((pos = line.find(kDelim)) != std::string::npos) {
//				BigInt value = std::stoll(line.substr(0, pos));
//				stoneCounts[value] += 1;
//				line.erase(0, pos + 1);
//			}
//			if (!line.empty()) {
//				BigInt value = std::stoll(line);
//				stoneCounts[value] += 1;
//			}
//		}
//	}
//	void dayElevenPartTwo() {
//		std::system("cls"); // clear terminal pre-boot
//		std::cout << "Running program DayElevenPartTwo" << '\n';
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
