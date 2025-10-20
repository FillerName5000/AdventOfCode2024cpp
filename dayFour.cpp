#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip> // just for cleaner logging
#include <utility>

namespace DayFour
{
	namespace {  // approach: RAM and simplicity over minimal RAM solution without large arrays
		using square_arr_t = std::vector<std::vector<char>>;
		using coordinates_t = std::vector<std::pair<int, int>>;

		const bool toLog = true; // true   false

		const int kTestFileLineLen = 10;
		const int kFullFileLineLen = 140; // files validated and pre checked for length and valid chars only. 

		const char kFirstChar = 'X';
		const char kSecondChar = 'M';
		const char kThirdChar = 'A';
		const char kFourthChar = 'S';

		enum class ValidDirection {
			North,
			South,
			East,
			West,
			NorthEast,
			NorthWest,
			SouthEast,
			SouthWest
		};

		std::pair<int, int> moveInDirection(std::pair<int, int> coord, ValidDirection direction) {
			switch (direction) {
			case ValidDirection::North:
				return { coord.first - 1, coord.second };
			case ValidDirection::South:
				return { coord.first + 1, coord.second };
			case ValidDirection::East:
				return { coord.first, coord.second + 1 };
			case ValidDirection::West:
				return { coord.first, coord.second - 1 };
			case ValidDirection::NorthEast:
				return { coord.first - 1, coord.second + 1 };
			case ValidDirection::NorthWest:
				return { coord.first - 1, coord.second - 1 };
			case ValidDirection::SouthEast:
				return { coord.first + 1, coord.second + 1 };
			case ValidDirection::SouthWest:
				return { coord.first + 1, coord.second - 1 };
			default:
				throw std::runtime_error::runtime_error("Invalid enum value");
			}
		}

		bool scanInDirection(const square_arr_t& charSquare, std::pair<int, int> coord, ValidDirection direction, const int kArraySize) {
			const char charsToMatch[] = { kSecondChar, kThirdChar, kFourthChar };

			for (char targetChar : charsToMatch) {
				coord = moveInDirection(coord, direction);

				if (coord.first < 0 || coord.first >= kArraySize || coord.second < 0 || coord.second >= kArraySize) {
					return false;
				}

				if (charSquare[coord.first][coord.second] != targetChar) {
					return false;
				}
			}

			return true;
		}

		int calculateValidFindsForFirstChar(const square_arr_t charSquare, const std::pair<int, int> coord, const int kArraySize)
		{
			int validFinds = 0;

			const int firstDirectionEntry = static_cast<int>(ValidDirection::North);
			const int lastDirectionEntry = static_cast<int>(ValidDirection::SouthWest);

			for (int i = firstDirectionEntry; i <= lastDirectionEntry; ++i) { // looping over enums directly is not possible in c++
				ValidDirection direction = static_cast<ValidDirection>(i); 
				if (scanInDirection(charSquare, coord, direction, kArraySize)) {
					++validFinds;
				}
			}
			
			return validFinds;
		}

		int calculateTotal(const square_arr_t charSquare, const int kArraySize, const coordinates_t firstCharCoords)
		{
			int totalFinds = 0;

			for (const auto& coord : firstCharCoords)
			{
				totalFinds += calculateValidFindsForFirstChar(charSquare, coord, kArraySize);
			}

			return totalFinds;
		}

		void fillLineInVector(const std::string line, square_arr_t& charSquare, const int currentRow, coordinates_t& firstCharCoords, const int kArraySize) {
			for (int col = 0; col < kArraySize; ++col) {
				charSquare[currentRow][col] = line[col];
				if (line[col] == kFirstChar) firstCharCoords.push_back(std::pair<int, int>(currentRow, col));
			}
		}

		void handleFile(std::ifstream& inputFile, const int kArraySize, square_arr_t& charSquare, coordinates_t& firstCharCoords)
		{
			if (inputFile.is_open()) {
				std::string line;
				int currentRow = 0;
				while (getline(inputFile, line)) {
					if (currentRow >= kArraySize) throw std::runtime_error::runtime_error("line read out of bounds");
					fillLineInVector(line, charSquare, currentRow, firstCharCoords, kArraySize);
					++currentRow;
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.


				int total = calculateTotal(charSquare, kArraySize, firstCharCoords);

				std::cout << "Total: " << total << '\n';

				std::cout << "Finished running program\n";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}
	}
	void dayFour() {
		std::cout << "Running program dayFour" << '\n';
		const bool isTestFile = false; // true   false
		const int kArraySize = isTestFile ? kTestFileLineLen : kFullFileLineLen;
		square_arr_t charSquare(kArraySize, std::vector<char>(kArraySize));
		coordinates_t firstCharCoords;

		std::string line;
		std::ifstream inputFile;
		(isTestFile) ? inputFile.open("dayFourTest.txt") : inputFile.open("dayFourFull.txt");
		handleFile(inputFile, kArraySize, charSquare, firstCharCoords);
	}
}