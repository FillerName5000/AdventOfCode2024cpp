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
//#include <algorithm>
//#include "BigInt.hpp" // https://github.com/faheel/BigInt
//
//namespace DayNinePartTwo
//{
//	namespace {
//		const bool toLog = false; // true   false
//		const size_t kEmptySpaceValue = std::numeric_limits<size_t>::max();
//		const size_t kFirstIdVal = 0;
//		const size_t kMaxSpace = 9;
//		const int kMaxIter = 1'000'000;
//		void coutVectorContents(const std::string prefix, const std::vector<size_t>& vector);
//		BigInt calculateChecksum(std::vector<size_t>& compressedDisk);
//
//		size_t findAvailableSpaceStartIndex(const std::vector<size_t>& compressedDisk, const size_t leftMostEmptySpacePos, const size_t spaceRequested, const size_t right)
//		{
//			size_t currentPos = leftMostEmptySpacePos;
//			int iter = 0;
//
//			while (currentPos < right)
//			{
//				size_t spaceLen = 0;
//
//				while (currentPos + spaceLen < right && compressedDisk[currentPos + spaceLen] == kEmptySpaceValue) spaceLen++;
//
//				if (spaceLen >= spaceRequested) return currentPos;
//				currentPos += spaceLen;
//
//				while (currentPos < right && compressedDisk[currentPos] != kEmptySpaceValue) currentPos++; // find next non-empty pos
//
//				iter++;
//				if (iter > kMaxIter) throw std::runtime_error("Max iter breached");
//			}
//
//			return kEmptySpaceValue;
//		}
//
//		std::pair<size_t, size_t> getIdSpaces(const std::vector<size_t>& compressedDisk, size_t rightCopy)
//		{
//			size_t id = compressedDisk[rightCopy];
//			size_t count = 0;
//			int iter = 0;
//
//			while (rightCopy >= 0 && compressedDisk[rightCopy] == id)
//			{
//				count++;
//				if (rightCopy == 0) break;
//				rightCopy--;
//
//				iter++;
//				if (iter > kMaxIter) throw std::runtime_error("Max iter breached");
//			}
//
//			return { id, count };
//		}
//
//		void setDiskFinalState(std::vector<size_t>& compressedDisk)
//		{
//			size_t size = compressedDisk.size();
//			size_t right = size - 1;
//
//			while (right > 0 && right < size)
//			{
//				if (compressedDisk[right] == kEmptySpaceValue)
//				{
//					right--;
//					continue;
//				}
//
//				std::pair<size_t, size_t> idSpaces = getIdSpaces(compressedDisk, right);
//				size_t id = idSpaces.first;
//				size_t spaceRequest = idSpaces.second;
//
//				if (right - spaceRequest <= 0) break;
//
//				size_t currentPos = findAvailableSpaceStartIndex(compressedDisk, 0, spaceRequest, right);
//
//				if (currentPos != kEmptySpaceValue)
//				{
//					std::fill(compressedDisk.begin() + currentPos, compressedDisk.begin() + currentPos + spaceRequest, id);
//					std::fill(compressedDisk.begin() + right - spaceRequest + 1, compressedDisk.begin() + right + 1, kEmptySpaceValue);
//				}
//
//				right -= spaceRequest;
//				if (toLog)
//				{
//					coutVectorContents("", compressedDisk);
//					std::cout << "id: " << id << "; spacerequest: " << spaceRequest << "; right: " << right << '\n';
//				}
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
//			if (toLog) coutVectorContents("Primary state:\n", compressedDisk);
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
//			if (toLog) coutVectorContents("Final state:\n", compressedDisk);
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
//		BigInt calculateChecksum(std::vector<size_t>& compressedDisk)
//		{
//			size_t len = compressedDisk.size();
//			BigInt total = 0;
//
//			for (size_t i = 0; i < len; i++)
//			{
//				if (compressedDisk[i] == kEmptySpaceValue)
//				{
//					continue;
//				}
//				if (toLog)
//				{
//					std::cout << total << " + (" << i << " * " << compressedDisk[i] << ") = ";
//				}
//				total += (i * compressedDisk[i]);
//				if (toLog)
//				{
//					std::cout << total << '\n';
//				}
//			}
//
//			return total;
//		}
//
//		void coutVectorContents(const std::string prefix, const std::vector<size_t>& vector)
//		{
//			std::cout << prefix;
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
//	void dayNinePartTwo() {
//		std::system("cls"); // clear terminal pre-boot
//		std::cout << "Running program DayNinePartTwo" << '\n';
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
//
//// 3310519109964 too low