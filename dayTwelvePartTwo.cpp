#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set> // no default pair hashing in c++14

namespace DayTwelvePartTwo
{
	namespace {
		using square_vect_t = std::vector<std::vector<char>>;
		using coord_t = std::pair<int, int>;

		enum class Direction { Up, Right, Down, Left, None, Invalid };

		bool isCoordWithinBounds(const coord_t& nextCoord, const int kSquareLen);
		bool isSameNeighbour(const square_vect_t& charSquare, const char prevChar, const coord_t& coordToCheck, const int kSquareLen);
		void fillLineInVector(const std::string line, square_vect_t& charSquare, const int kSquareLen, const int currentRow);

		void coutCharSquareWithChecked(const square_vect_t& charSquare, const int kSquareLen);
		void coutRegisteredBordersOverlay(const std::set<coord_t>& registeredBorders, const int kSquareLen, const std::set<coord_t>& priorityCoords);
		void coutSetContents(const std::string prefix, const std::set<coord_t>& vec);

		Direction getOppositeDirection(Direction direction);
		void handleFile(std::ifstream& inputFile, square_vect_t& charSquare, const int kSquareLen);

		const bool toLog = true; // true   false
		const int kTestFileLineLen = 10;
		const int kFullFileLineLen = 140; // files validated and pre checked for length and valid chars only. 
		const std::vector<coord_t> kMovements = { {-1, 0}, {0, 1}, {1, 0}, {0, -1} };
		const size_t kMovementsSize = kMovements.size();

		std::set<coord_t> checkedCoords;

		size_t getTotalBorders(const square_vect_t& charSquare, const int kSquareLen, const char charInBorder, const std::set<coord_t>& registeredBorders, std::set<coord_t> withinBorderCoords) {
			size_t totalBorders = 0;
			std::set<coord_t> checkedCorners = registeredBorders;
			std::set<coord_t> checkedCornersWithImpact;

			for (const auto& border : registeredBorders) {
				for (size_t i = 0; i < kMovementsSize; ++i) {
					coord_t nextCoord = { border.first + kMovements[i].first, border.second + kMovements[i].second };
					if (isSameNeighbour(charSquare, charInBorder, nextCoord, kSquareLen)) continue;
					if (checkedCorners.find(nextCoord) != checkedCorners.end()) continue;

					int orthogonalCount = 0;
					for (size_t j = 0; j < kMovementsSize; ++j) {
						coord_t orthoCoord = { nextCoord.first + kMovements[j].first, nextCoord.second + kMovements[j].second };
						if (registeredBorders.find(orthoCoord) != registeredBorders.end()) {
							++orthogonalCount;
						}
					}

					if (orthogonalCount >= 2) checkedCorners.insert(nextCoord);
					if (orthogonalCount == 2) {
						++totalBorders;
						checkedCornersWithImpact.insert(nextCoord);
						if (toLog) std::cout << "Coord (x:" << nextCoord.second << ",y:" << nextCoord.first << ") +1 to totalBorders, at: " << totalBorders << "\n";
					}
					else if (orthogonalCount == 3) {
						totalBorders += 2;
						checkedCornersWithImpact.insert(nextCoord);
						if (toLog) std::cout << "Coord (x:" << nextCoord.second << ",y:" << nextCoord.first << ") +2 to totalBorders\n";
					}
					else if (orthogonalCount == 4) {
						if (toLog) std::cout << "Coord (" << nextCoord.first << "," << nextCoord.second << ") throws (4 borders)\n";
						throw std::runtime_error("Invalid surrounded border");
					}
				}
			}
			if (toLog) coutRegisteredBordersOverlay(registeredBorders, kSquareLen, checkedCornersWithImpact);
			return totalBorders;
		}

		size_t getAreaPrice(const square_vect_t& charSquare, const int kSquareLen, const coord_t& startCoord) {
			size_t entriesChecked = 0;

			std::set<coord_t> registeredBorders;

			std::vector<std::pair<coord_t, Direction>> coordsWithOrigin;
			std::set<coord_t> allCoordsAdded;

			coordsWithOrigin.push_back(std::make_pair(startCoord, Direction::None));
			allCoordsAdded.insert(startCoord);

			while (!coordsWithOrigin.empty()) {
				std::pair<coord_t, Direction> top = coordsWithOrigin.back();
				coord_t currentCoord = top.first;
				Direction prevDirection = top.second;
				coordsWithOrigin.pop_back();

				if (checkedCoords.find(currentCoord) != checkedCoords.end()) continue;
				checkedCoords.insert(currentCoord);
				++entriesChecked;

				for (int i = 0; i < kMovementsSize; ++i) {
					Direction currentDirection = static_cast<Direction>(i);
					if (prevDirection == getOppositeDirection(prevDirection)) continue;

					coord_t nextCoord = { currentCoord.first + kMovements[i].first, currentCoord.second + kMovements[i].second };

					if (isSameNeighbour(charSquare, charSquare[currentCoord.first][currentCoord.second], nextCoord, kSquareLen)) {
						coordsWithOrigin.push_back(std::make_pair(nextCoord, currentDirection));
						allCoordsAdded.insert(nextCoord);
					}
					else {
						registeredBorders.insert(nextCoord);
					}
				}
			}

			size_t totalBorders = getTotalBorders(charSquare, kSquareLen, charSquare[startCoord.first][startCoord.second], registeredBorders, allCoordsAdded);

			if (toLog) {
				std::cout << "Total for: " << charSquare[startCoord.first][startCoord.second] << " is " << entriesChecked << " * " << totalBorders << '\n';
				//	coutCharSquareWithChecked(charSquare, kSquareLen);
			}

			return totalBorders * entriesChecked;
		}

		size_t calculateTotalPrice(const square_vect_t& charSquare, const int kSquareLen) {
			size_t total = 0;

			for (int row = 0; row < kSquareLen; ++row) {
				for (int col = 0; col < kSquareLen; ++col) {
					coord_t currentCoord = { row, col };
					if (checkedCoords.find(currentCoord) == checkedCoords.end()) {
						total += getAreaPrice(charSquare, kSquareLen, currentCoord);
					}
				}
			}

			return total;
		}

		void handleFile(std::ifstream& inputFile, square_vect_t& charSquare, const int kSquareLen)
		{
			std::pair<int, int> startingGuardCoord;
			if (inputFile.is_open()) {
				std::string line;
				int currentRow = 0;
				while (getline(inputFile, line)) {
					if (currentRow >= kSquareLen) throw std::runtime_error("Line read out of bounds");
					fillLineInVector(line, charSquare, kSquareLen, currentRow);
					currentRow++;
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				size_t totalPrice = calculateTotalPrice(charSquare, kSquareLen);

				std::cout << "Total price: " << totalPrice;

				std::cout << "\nFinished running program";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}


		bool isSameNeighbour(const square_vect_t& charSquare, const char prevChar, const coord_t& coordToCheck, const int kSquareLen) {
			if (!isCoordWithinBounds(coordToCheck, kSquareLen)) return false;
			return (charSquare[coordToCheck.first][coordToCheck.second] == prevChar);
		}

		bool isCoordWithinBounds(const coord_t& nextCoord, const int kSquareLen)
		{
			return nextCoord.first >= 0 && nextCoord.first < kSquareLen && nextCoord.second >= 0 && nextCoord.second < kSquareLen;
		}

		void fillLineInVector(const std::string line, square_vect_t& charSquare, const int kSquareLen, const int currentRow) {
			for (int col = 0; col < kSquareLen; ++col) {
				charSquare[currentRow][col] = line[col];
			}
		}

		Direction getOppositeDirection(Direction direction) {
			switch (direction) {
			case Direction::Up: return Direction::Down;
			case Direction::Right: return Direction::Left;
			case Direction::Down: return Direction::Up;
			case Direction::Left: return Direction::Right;
			default: return Direction::Invalid;
			}
		}

		void coutSetContents(const std::string prefix, const std::set<coord_t>& vec) {
			std::cout << prefix;
			for (const auto& c : vec) {
				std::cout << "(" << c.first << "," << c.second << ") ";
			}
			std::cout << '\n';
		}

		void coutRegisteredBordersOverlay(const std::set<coord_t>& registeredBorders, const int kSquareLen, const std::set<coord_t>& priorityCoords) {
			std::vector<std::vector<char>> grid(kSquareLen + 1, std::vector<char>(kSquareLen + 1, '.'));
			for (const auto& coord : registeredBorders) {
				int adjustedX = coord.first + 1;
				int adjustedY = coord.second + 1;
				if (adjustedX >= 0 && adjustedX <= kSquareLen && adjustedY >= 0 && adjustedY <= kSquareLen) {
					grid[adjustedX][adjustedY] = '#';
				}
			}
			for (const auto& coord : priorityCoords) {
				int adjustedX = coord.first + 1;
				int adjustedY = coord.second + 1;
				if (adjustedX >= 0 && adjustedX <= kSquareLen && adjustedY >= 0 && adjustedY <= kSquareLen) {
					grid[adjustedX][adjustedY] = '+';
				}
			}
			for (const auto& row : grid) {
				for (const auto& cell : row) {
					std::cout << cell;
				}
				std::cout << '\n';
			}
		}

		void coutCharSquareWithChecked(const square_vect_t& charSquare, const int kSquareLen) {
			for (int row = 0; row < kSquareLen; ++row) {
				for (int col = 0; col < kSquareLen; ++col) {
					coord_t currentCoord = { row, col };
					if (checkedCoords.find(currentCoord) != checkedCoords.end()) {
						std::cout << '.';
					}
					else {
						std::cout << charSquare[row][col];
					}
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
	}

	void dayTwelvePartTwo() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DayTwelvePartTwo" << '\n';
		const bool isFullFile = false; // true   false
		const int kSquareLen = isFullFile ? kFullFileLineLen : kTestFileLineLen;
		square_vect_t charSquare(kSquareLen, std::vector<char>(kSquareLen));

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("dayTwelveFull.txt") : inputFile.open("dayTwelveTest.txt");
		handleFile(inputFile, charSquare, kSquareLen);
	}
}
