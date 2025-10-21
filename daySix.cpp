#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace DaySix
{
	namespace {
		using square_vect_t = std::vector<std::vector<char>>;
		void coutSquareVector(std::string prefix, const square_vect_t& charSquare); // function declaration on top of the file for the less important functions 
		int calculateTotal(square_vect_t& charSquare);

		enum class Direction { Up, Right, Down, Left };

		std::pair<int, int> getNextCoord(std::pair<int, int> coordCopy, const Direction direction);
		bool isCoordWithinBounds(std::pair<int, int>& nextCoord, const int kSquareLen);

		const bool toLog = false; // true   false
		const int kTestFileLineLen = 10;
		const int kFullFileLineLen = 130; // files validated and pre checked for length and valid chars only. 

		const char guardChar = '^';
		const char obstacleChar = '#';
		const char emptySpaceChar = '.';
		const char crossedSpaceChar = 'X';


		void runGuardRoute(square_vect_t& charSquare, std::pair<int, int>& guardCoord, const int kSquareLen)
		{
			Direction currentDirection = Direction::Up;
			int kMaxIterations = 1'000'000;

			int iterations = 0;
			while (true) {
				if (++iterations > kMaxIterations) {
					throw std::runtime_error("Past max_iter");
				}

			//	if (toLog) coutSquareVector("new state: ", charSquare);

				charSquare[guardCoord.first][guardCoord.second] = crossedSpaceChar;

				std::pair<int, int> nextCoord = getNextCoord(guardCoord, currentDirection);

				if (!isCoordWithinBounds(nextCoord, kSquareLen)) break;

				char nextCoordChar = charSquare[nextCoord.first][nextCoord.second];

				switch (nextCoordChar) {
				case obstacleChar:
					currentDirection = static_cast<Direction>((static_cast<int>(currentDirection) + 1) % 4);
					break;
				case emptySpaceChar:
				case crossedSpaceChar:
					guardCoord = nextCoord;
					break;
				default:
					if (toLog) std::cout << "Invalid char at " << nextCoord.first << '-' << nextCoord.second << ": " << nextCoordChar;
					throw std::runtime_error("Invalid char found");
				}
			}

		}

		void fillLineInVector(const std::string line, const int kSquareLen, square_vect_t& charSquare, std::pair<int, int>& guardCoord, const int currentRow) {
			for (int col = 0; col < kSquareLen; ++col) {
				charSquare[currentRow][col] = line[col];
				if (line[col] == guardChar) guardCoord = std::pair<int, int>(currentRow, col);
			}
		}

		void handleFile(std::ifstream& inputFile, const int kSquareLen, square_vect_t& charSquare, std::pair<int, int>& guardCoord)
		{
			if (inputFile.is_open()) {
				std::string line;
				int currentRow = 0;
				while (getline(inputFile, line)) {
					if (currentRow >= kSquareLen) throw std::runtime_error::runtime_error("line read out of bounds");
					fillLineInVector(line, kSquareLen, charSquare, guardCoord, currentRow);
					++currentRow;
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				if (toLog) coutSquareVector("Starting state: ", charSquare);

				runGuardRoute(charSquare, guardCoord, kSquareLen);

				if (toLog) coutSquareVector("End state: ", charSquare);

				int total = calculateTotal(charSquare);

				std::cout << "Total: " << total << '\n';
				std::cout << "Finished running program\n";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		bool isCoordWithinBounds(std::pair<int, int>& nextCoord, const int kSquareLen)
		{
			return nextCoord.first >= 0 && nextCoord.first < kSquareLen && nextCoord.second >= 0 && nextCoord.second < kSquareLen;
		}

		std::pair<int, int> getNextCoord(std::pair<int, int> coordCopy, const Direction direction)
		{
			switch (direction)
			{
			case (Direction::Up):
				return std::pair<int, int>(coordCopy.first - 1, coordCopy.second);
			case (Direction::Right):
				return std::pair<int, int>(coordCopy.first, coordCopy.second + 1);
			case (Direction::Down):
				return std::pair<int, int>(coordCopy.first + 1, coordCopy.second);
			case (Direction::Left):
				return std::pair<int, int>(coordCopy.first, coordCopy.second - 1);
			default:
				throw std::runtime_error::runtime_error("Invalid enum value");
			}
		}

		int calculateTotal(square_vect_t& charSquare)
		{
			int total = 0;
			for (const auto& row : charSquare) {
				for (const auto& c : row) {
					if (c == crossedSpaceChar) ++total;
				}
			}
			return total;
		}

		void coutSquareVector(std::string prefix, const square_vect_t& charSquare) {
			std::cout << prefix << '\n';
			for (const auto& row : charSquare) {
				for (const auto& ch : row) {
					std::cout << ch;
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
	}
	void daySix() {
		std::cout << "Running program Day Six" << '\n';
		const bool isFullFile = true; // true   false
		const int kSquareLen = isFullFile ? kFullFileLineLen : kTestFileLineLen;
		square_vect_t charSquare(kSquareLen, std::vector<char>(kSquareLen));
		std::pair<int, int> guardCoord;

		std::string line;
		std::ifstream inputFile;
		(isFullFile) ? inputFile.open("daySixFull.txt") : inputFile.open("daySixTest.txt");
		handleFile(inputFile, kSquareLen, charSquare, guardCoord);
	}
}