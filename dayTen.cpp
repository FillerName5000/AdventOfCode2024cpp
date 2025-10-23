#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

namespace DayTen
{
	namespace {
		using square_vect_t = std::vector<std::vector<int>>;
		using coord_t = std::pair<int, int>;

		void handleFile(std::ifstream& inputFile, square_vect_t& intSquare, const int kSquareLen);
		void coutVectorContents(const std::string prefix, const std::vector<coord_t>& vector);
		void fillLineInVector(const std::string line, square_vect_t& intSquare, const int kSquareLen, const int currentRow);
		bool isValidSlope(const square_vect_t& intSquare, const int prevHeight, const coord_t& coordToCheck, const int kSquareLen);
		bool isCoordWithinBounds(const coord_t& nextCoord, const int kSquareLen);

		const bool toLog = false; // true   false
		const int kTestFileLineLen = 8;
		const int kFullFileLineLen = 43; // files validated and pre checked for length and valid chars only. 
		const int kPeakVal = 9;
		const std::vector<coord_t> movements = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };

		std::vector<coord_t> startingCoords;

		void findPeaks(const square_vect_t& intSquare, const coord_t& startCoord, std::set<coord_t>& peakCoordsForThisStart, const int kSquareLen)
		{
			std::vector<coord_t> stack;
			stack.push_back(startCoord);

			while (!stack.empty()) {
				coord_t currentCoord = stack.back();
				stack.pop_back();

				for (const auto& movement : movements) {
					coord_t nextCoord = { currentCoord.first + movement.first, currentCoord.second + movement.second };

					if (isValidSlope(intSquare, intSquare[currentCoord.first][currentCoord.second], nextCoord, kSquareLen)) {
						if (intSquare[nextCoord.first][nextCoord.second] == kPeakVal) {
							peakCoordsForThisStart.insert(nextCoord);
						}
						else {
							stack.push_back(nextCoord);
						}
					}
				}
			}
		}

		size_t calculatePeaksForCoord(const square_vect_t& intSquare, const coord_t startCoord, const int kSquareLen)
		{
			std::set<coord_t> peakCoordsForThisStart;
			findPeaks(intSquare, startCoord, peakCoordsForThisStart, kSquareLen);
			return peakCoordsForThisStart.size();
		}

		size_t calculateTotalPeaks(const square_vect_t& intSquare, const int kSquareLen)
		{
			size_t total = 0;

			for (const auto& startCoord : startingCoords)
			{
				total += calculatePeaksForCoord(intSquare, startCoord, kSquareLen);
			}

			return total;
		}

		void handleFile(std::ifstream& inputFile, square_vect_t& intSquare, const int kSquareLen)
		{
			if (inputFile.is_open()) {
				std::string line;
				int currentRow = 0;
				while (getline(inputFile, line)) {
					if (currentRow >= kSquareLen) throw std::runtime_error("Line read out of bounds");
					fillLineInVector(line, intSquare, kSquareLen, currentRow);
					currentRow++;
				}

				if (toLog) coutVectorContents("Starting coords: ", startingCoords);

				std::cout << "Finished reading file\n";

				inputFile.close(); // not required, handled by scope, about explicit purpose

				size_t totalScore = calculateTotalPeaks(intSquare, kSquareLen);

				std::cout << "Total: " << totalScore << '\n';

				std::cout << "Finished running program\n";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}


		bool isValidSlope(const square_vect_t& intSquare, const int prevHeight, const coord_t& coordToCheck, const int kSquareLen) {
			if (!isCoordWithinBounds(coordToCheck, kSquareLen)) return false;
			return (intSquare[coordToCheck.first][coordToCheck.second] - prevHeight) == 1;
		}

		bool isCoordWithinBounds(const coord_t& nextCoord, const int kSquareLen)
		{
			return nextCoord.first >= 0 && nextCoord.first < kSquareLen && nextCoord.second >= 0 && nextCoord.second < kSquareLen;
		}

		void fillLineInVector(const std::string line, square_vect_t& intSquare, const int kSquareLen, const int currentRow) {
			for (int col = 0; col < kSquareLen; ++col) {
				intSquare[currentRow][col] = line[col] - '0'; //ASCII num parsing
				if (intSquare[currentRow][col] == 0) startingCoords.emplace_back(currentRow, col);
			}
		}

		void coutVectorContents(const std::string prefix, const std::vector<coord_t>& vector)
		{
			std::cout << prefix;
			for (const auto& coord : vector) {
				std::cout << "[" << coord.first << "," << coord.second << "] ";
			}
			std::cout << '\n';
		}
	}
	void dayTen() {
		std::system("cls");
		std::cout << "Running program DayTen" << '\n';
		const bool isFullFile = true; // true  false
		const int kSquareLen = isFullFile ? kFullFileLineLen : kTestFileLineLen;
		square_vect_t intSquare(kSquareLen, std::vector<int>(kSquareLen));

		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("dayTenFull.txt") : inputFile.open("dayTenTest.txt");
		handleFile(inputFile, intSquare, kSquareLen);
	}
}
