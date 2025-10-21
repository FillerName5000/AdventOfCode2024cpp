#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>

namespace DaySixPartTwo
{
	namespace {
		using square_vect_t = std::vector<std::vector<char>>;
		void coutSquareVector(std::string prefix, const square_vect_t& charSquare); // function declaration on top of the file for the less important functions 
		int calculateTotal(square_vect_t& charSquare);

		enum class Direction { Up, Right, Down, Left };

		std::pair<int, int> getNextCoord(std::pair<int, int> coordCopy, const Direction direction);
		bool isCoordWithinBounds(std::pair<int, int> nextCoord, const int kSquareLen);

		const bool toLog = false; // true   false
		const int kTestFileLineLen = 10;
		const int kFullFileLineLen = 130; // files validated and pre checked for length and valid chars only. 
		// only possible algo: put an obstacle on every position in the route & check it?

		const char guardChar = '^';
		const char obstacleChar = '#';
		const char emptySpaceChar = '.';
		const char crossedSpaceChar = 'X';

		bool isRouteWithNewObstacleInfiniteLoop(square_vect_t charSquareCopy, std::pair<int, int> guardCoordCopy, const std::pair<int, int> newObstacleCoord, const int kSquareLen)
		{
			if (!isCoordWithinBounds(newObstacleCoord, kSquareLen))
				throw std::runtime_error("Invalid new coord: (" + std::to_string(newObstacleCoord.first) + ", " + std::to_string(newObstacleCoord.second) + ")");

			if (guardCoordCopy == newObstacleCoord) guardCoordCopy = getNextCoord(guardCoordCopy, Direction::Up); // Both datasets do not have an obstacle immediately ahead of them

			charSquareCopy[newObstacleCoord.first][newObstacleCoord.second] = obstacleChar;

			Direction currentDirection = Direction::Up;
			int iterations = 0;
			const int kMaxIterations = 1'000'000;
			const int kArrayLen = 4;

			std::set<std::pair<int, int>> directionChangeCoords;
			int consecutiveObstacleCount = 0;

			while (true) {
				if (++iterations > kMaxIterations) {
					throw std::runtime_error("Past max_iter");
				}

				charSquareCopy[guardCoordCopy.first][guardCoordCopy.second] = crossedSpaceChar;

				std::pair<int, int> nextCoord = getNextCoord(guardCoordCopy, currentDirection);

				if (!isCoordWithinBounds(nextCoord, kSquareLen)) {
					return false;
				}

				char nextCoordChar = charSquareCopy[nextCoord.first][nextCoord.second];

				switch (nextCoordChar) {
				case obstacleChar:
					if (++consecutiveObstacleCount > 4) throw std::runtime_error("Security guard is stuck");

					currentDirection = static_cast<Direction>((static_cast<int>(currentDirection) + 1) % 4);
					if (consecutiveObstacleCount > 1) break;

					if (directionChangeCoords.find(guardCoordCopy) != directionChangeCoords.end()) {
						return true;
					}
					directionChangeCoords.insert(guardCoordCopy);
					break;
				case emptySpaceChar:
				case crossedSpaceChar:
					guardCoordCopy = nextCoord;
					consecutiveObstacleCount = 0;
					break;
				default:
					if (toLog) std::cout << "Invalid char at " << nextCoord.first << '-' << nextCoord.second << ": " << nextCoordChar;
					throw std::runtime_error("Invalid char found");
				}
			}
		}

		int calculateTotal(square_vect_t& charSquare, std::pair<int, int> guardCoord, const int kSquareLen)
		{
			int total = 0;
			for (int row = 0; row < kSquareLen; ++row) {
				for (int col = 0; col < kSquareLen; ++col) {
					if (charSquare[row][col] == crossedSpaceChar) {
						std::pair<int, int> newObstacleCoord = { row, col };
						if (toLog) std::cout << "Testing coord: " << newObstacleCoord.first << '-' << newObstacleCoord.second << '\n';
						if (isRouteWithNewObstacleInfiniteLoop(charSquare, guardCoord, newObstacleCoord, kSquareLen)) {
							total++;
							if (toLog) std::cout << "Total is: " << total << '\n';
						}
					}
				}
			}
			return total;
		}

		void runGuardRoute(square_vect_t& charSquare, std::pair<int, int> guardCoordCopy, const int kSquareLen)
		{
			Direction currentDirection = Direction::Up;
			int kMaxIterations = 1'000'000;

			int iterations = 0;
			while (true) {
				if (++iterations > kMaxIterations) {
					throw std::runtime_error("Past max_iter");
				}

				//	if (toLog) coutSquareVector("new state: ", charSquare);

				charSquare[guardCoordCopy.first][guardCoordCopy.second] = crossedSpaceChar;

				std::pair<int, int> nextCoord = getNextCoord(guardCoordCopy, currentDirection);

				if (!isCoordWithinBounds(nextCoord, kSquareLen)) break;

				char nextCoordChar = charSquare[nextCoord.first][nextCoord.second];

				switch (nextCoordChar) {
				case obstacleChar:
					currentDirection = static_cast<Direction>((static_cast<int>(currentDirection) + 1) % 4);
					break;
				case emptySpaceChar:
				case crossedSpaceChar:
					guardCoordCopy = nextCoord;
					break;
				default:
					if (toLog) std::cout << "Invalid char at " << nextCoord.first << '-' << nextCoord.second << ": " << nextCoordChar;
					throw std::runtime_error("Invalid char found");
				}
			}
			// ^ is removed after run and replaced with x in test file
		}

		void fillLineInVector(const std::string line, const int kSquareLen, square_vect_t& charSquare, std::pair<int, int>& guardCoord, const int currentRow) {
			for (int col = 0; col < kSquareLen; ++col) {
				charSquare[currentRow][col] = line[col];
				if (line[col] == guardChar) guardCoord = std::pair<int, int>(currentRow, col);
			}
		}

		void handleFile(std::ifstream& inputFile, const int kSquareLen, square_vect_t& charSquare, std::pair<int, int>& guardCoord)
		{
			std::pair<int, int> startingGuardCoord;
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

				int total = calculateTotal(charSquare, guardCoord, kSquareLen);

				std::cout << "\nTotal: " << total << '\n';
				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}

		bool isCoordWithinBounds(const std::pair<int, int> nextCoord, const int kSquareLen)
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
	void daySixPartTwo() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program Day Six PartTwo" << '\n';
		const bool isFullFile = true; // true   false
		const int kSquareLen = isFullFile ? kFullFileLineLen : kTestFileLineLen;
		square_vect_t charSquare(kSquareLen, std::vector<char>(kSquareLen));
		std::pair<int, int> guardCoord;

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("daySixFull.txt") : inputFile.open("daySixTest.txt");
		handleFile(inputFile, kSquareLen, charSquare, guardCoord);
	}
}
// 6,3;7,5;7,6;8,1;8,3;9,8