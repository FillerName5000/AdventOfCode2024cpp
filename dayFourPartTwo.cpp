#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip> // just for cleaner logging
#include <utility>

namespace DayFourPartTwo
{
	namespace {  // approach: RAM and simplicity over minimal RAM solution without large arrays
		using square_arr_t = std::vector<std::vector<char>>;
		using coordinates_t = std::vector<std::pair<int, int>>;

		const bool toLog = false; // true   false

		const int kTestFileLineLen = 10;
		const int kFullFileLineLen = 140; // files validated and pre checked for length and valid chars only. 

		const char kMiddleChar = 'A';
		const char kThirdChar = 'M';
		const char kFourthChar = 'S';

		bool isValidCoord(std::pair<int, int> coord, const int kArraySize)
		{
			if (coord.first < 0 || coord.first >= kArraySize || coord.second < 0 || coord.second >= kArraySize) {
				return false;
			}
			return true;
		}

		bool areSurroundingCoordsValid(std::pair<int, int> coordCopy, const int kArraySize)
		{
			std::pair<int, int> coordToCheck = coordCopy;
			coordToCheck.first -= 1;
			coordToCheck.second += 1;
			if (!isValidCoord(coordToCheck, kArraySize)) return false;

			coordToCheck = coordCopy;
			coordToCheck.first += 1;
			coordToCheck.second += 1;
			if (!isValidCoord(coordToCheck, kArraySize)) return false;

			coordToCheck = coordCopy;
			coordToCheck.first -= 1;
			coordToCheck.second -= 1;
			if (!isValidCoord(coordToCheck, kArraySize)) return false;

			coordToCheck = coordCopy;
			coordToCheck.first += 1;
			coordToCheck.second -= 1;
			if (!isValidCoord(coordToCheck, kArraySize)) return false;
			return true; 
		}

		bool isValidOppositeDiagonalChar(char sourceChar, char oppositeToCheck)
		{
			if (sourceChar == kThirdChar)
			{
				return oppositeToCheck == kFourthChar;
			}
			return oppositeToCheck == kThirdChar;
		}

		bool isValidDiagonalChar(char c)
		{
			return c == kThirdChar || c == kFourthChar;
		}

		bool isDiagonalValid(const square_arr_t charSquare, const int kArraySize, const std::pair<int, int> middleCharCoord, const bool startTopLeft)
		{
			if (!areSurroundingCoordsValid(middleCharCoord, kArraySize)) return false;
			if (startTopLeft)
			{
				std::pair<int, int> coordToCheck = middleCharCoord;
				coordToCheck.first -= 1;
				coordToCheck.second -= 1;
				char topLeftChar = charSquare[coordToCheck.first][coordToCheck.second];
				if (!isValidDiagonalChar(topLeftChar)) return false;

				coordToCheck = middleCharCoord;
				coordToCheck.first += 1;
				coordToCheck.second += 1;
				if (!isValidOppositeDiagonalChar(topLeftChar, charSquare[coordToCheck.first][coordToCheck.second])) return false;
			}
			else 
			{
				std::pair<int, int> coordToCheck = middleCharCoord;
				coordToCheck.first += 1;
				coordToCheck.second -= 1;
				char topRightChar = charSquare[coordToCheck.first][coordToCheck.second];
				if (!isValidDiagonalChar(charSquare[coordToCheck.first][coordToCheck.second])) return false;

				coordToCheck = middleCharCoord;
				coordToCheck.first -= 1;
				coordToCheck.second += 1;
				if (!isValidOppositeDiagonalChar(topRightChar, charSquare[coordToCheck.first][coordToCheck.second])) return false;
			}
			return true;
		}

		bool isMiddleCharInValidX(const square_arr_t charSquare, const int kArraySize, const std::pair<int, int> middleCharCoord)
		{
			if (!isDiagonalValid(charSquare, kArraySize, middleCharCoord, true)) return false;
			if (!isDiagonalValid(charSquare, kArraySize, middleCharCoord, false)) return false;
			return true;
		}

		int calculateTotal(const square_arr_t charSquare, const int kArraySize, const coordinates_t middleCharCoords)
		{
			int totalFinds = 0;

			for (const auto& coord : middleCharCoords)
			{
				if (isMiddleCharInValidX(charSquare, kArraySize, coord)) totalFinds++;
				if (toLog) std::cout << coord.first << '-' << coord.second <<'\n';
			}

			return totalFinds;
		}

		void fillLineInVector(const std::string line, square_arr_t& charSquare, const int currentRow, coordinates_t& middleCharCoords, const int kArraySize) {
			for (int col = 0; col < kArraySize; ++col) {
				charSquare[currentRow][col] = line[col];
				if (line[col] == kMiddleChar) middleCharCoords.push_back(std::pair<int, int>(currentRow, col));
			}
		}

		void handleFile(std::ifstream& inputFile, const int kArraySize, square_arr_t& charSquare, coordinates_t& middleCharCoords)
		{
			if (inputFile.is_open()) {
				std::string line;
				int currentRow = 0;
				while (getline(inputFile, line)) {
					if (currentRow >= kArraySize) throw std::runtime_error::runtime_error("line read out of bounds");
					fillLineInVector(line, charSquare, currentRow, middleCharCoords, kArraySize);
					++currentRow;
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				if (toLog) {
					std::cout << "Full Content of 2D Array:\n";
					for (const auto& row : charSquare) {
						for (const char& ch : row) {
							std::cout << ch;
						}
						std::cout << '\n';
					}
				}

				int total = calculateTotal(charSquare, kArraySize, middleCharCoords);

				std::cout << "Total: " << total << '\n';

				if (toLog) {
					std::cout << "Valid Coordinates:\n";
					for (const auto& coord : middleCharCoords) {
						if (isMiddleCharInValidX(charSquare, kArraySize, coord)) {
							std::cout << "Coordinate: (" << coord.first << ", " << coord.second << ")\n";

							int row = coord.first;
							std::cout << "Row " << row << ": ";
							for (const char& ch : charSquare[row]) {
								std::cout << ch;
							}
							std::cout << '\n';
						}
					}
				}

				std::cout << "Finished running program\n";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}
	}
	void dayFourPartTwo() {
		std::cout << "Running program dayFour Part Two" << '\n';
		const bool isTestFile = false; // true   false
		const int kArraySize = isTestFile ? kTestFileLineLen : kFullFileLineLen;
		square_arr_t charSquare(kArraySize, std::vector<char>(kArraySize));
		coordinates_t middleCharCoords;

		std::string line;
		std::ifstream inputFile;
		(isTestFile) ? inputFile.open("dayFourTest.txt") : inputFile.open("dayFourFull.txt");
		handleFile(inputFile, kArraySize, charSquare, middleCharCoords);
	}
}