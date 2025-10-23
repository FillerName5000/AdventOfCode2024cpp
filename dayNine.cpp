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
//#include <sstream>
//#include <vector>
//#include <limits>
//#include "BigInt.hpp"
//
//namespace DayNine
//{
//	namespace {
//		const bool toLog = false; // true   false
//		const size_t kEmptySpaceValue = std::numeric_limits<size_t>::max();;
//		void coutVectorContents(const std::vector<size_t>& vector);
//
//		BigInt calculateChecksum(std::vector<size_t>& compressedDisk)
//		{
//			size_t len = compressedDisk.size();
//			BigInt total = 0;
//
//			for (size_t i = 0; i < len; i++)
//			{
//				if (compressedDisk[i] == kEmptySpaceValue)
//				{
//					return total;
//				}
//				total += (i * compressedDisk[i]);
//			}
//
//			return total;
//		}
//
//		void setDiskFinalState(std::vector<size_t>& compressedDisk)
//		{
//			size_t left = 0;
//			size_t right = compressedDisk.size() - 1;
//
//			long maxIter = 10'000'000;
//			long iter = 0;
//
//			while (left < right)
//			{
//				if (compressedDisk[right] != kEmptySpaceValue)
//				{
//					for (left; left < right; left++)
//					{
//						if (compressedDisk[left] == kEmptySpaceValue)
//						{
//							compressedDisk[left] = compressedDisk[right];
//							compressedDisk.erase(compressedDisk.begin() + right);
//							--right;
//							break;
//						}
//					}
//				}
//				else
//				{
//					--right;
//				}
//				iter++;
//				if (iter > maxIter) throw std::runtime_error("Max iter breached");
//			}
//		}
//		
//		std::vector<size_t> setDiskPrimaryState(const std::string& uncompressedLine)
//		{
//			std::vector<size_t> compressedDisk;
//			size_t len = uncompressedLine.size();
//			size_t id = 0;
//
//			for (size_t i = 0; i < len; i++)
//			{
//				size_t parsedLen = static_cast<size_t>(uncompressedLine[i] - '0'); // only digits in input data, ASCII conversion
//				if (i % 2 == 0)
//				{
//					compressedDisk.insert(compressedDisk.end(), parsedLen, id);
//					id++;
//				}
//				else
//				{
//					compressedDisk.insert(compressedDisk.end(), parsedLen, kEmptySpaceValue);
//				}
//			}
//
//			if (toLog) coutVectorContents(compressedDisk);
//
//			return compressedDisk;
//		}
//
//		BigInt handleUncompressedLine(const std::string& uncompressedLine)
//		{
//			std::vector<size_t> compressedDisk = setDiskPrimaryState(uncompressedLine);
//
//			setDiskFinalState(compressedDisk);
//
//			if (toLog) coutVectorContents(compressedDisk);
//
//			return calculateChecksum(compressedDisk);
//		}
//
//		void handleFile(std::ifstream& inputFile)
//		{
//			std::pair<int, int> startingGuardCoord;
//			if (inputFile.is_open()) {
//				std::string line;
//				BigInt checksum;
//				while (getline(inputFile, line)) {
//					checksum = handleUncompressedLine(line);
//				}
//
//				std::cout << "\nFinished reading file\n";
//
//				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.
//
//				std::cout << "Checksum: " << checksum;
//
//				std::cout << "\nFinished running program";
//			}
//			else {
//				std::cout << "Unable to open file\n";
//			}
//		}
//
//		void coutVectorContents(const std::vector<size_t>& vector)
//		{
//			for (const auto& num : vector) {
//				if (num == kEmptySpaceValue) {
//					std::cout << "M"; //MAX_VAL
//				}
//				else {
//					std::cout << num;
//				}
//			}
//			std::cout << '\n';
//		}
//
//	}
//	void dayNine() {
//		std::system("cls"); // clear terminal pre-boot
//		std::cout << "Running program DayNine" << '\n';
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
//		(isFullFile) ? inputFile.open("dayNineFull.txt") : inputFile.open("dayNineTest.txt");
//		handleFile(inputFile);
//	}
//}
